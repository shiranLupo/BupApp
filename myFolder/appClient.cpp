// appClient.cpp

//
// This is a backUp service c++ application C++ - client.
//
// The "backUp" application allow a client to backup his directory on subscribed servers.
// the servers are local pcs with avalaible memory for backup .
// the server rpi is configured to backup client- pc. 
//
// This application is an MQTT publisher/subscriber using the C++
// asynchronous client interface, employing callbacks to receive messages
// and status updates.
//
// method:
//  - Connecting to an MQTT server/broker.
//  - Publishing topic (backUp req), verify a backup server is listening 
//  - Subscribing to a topic(confim and path -to be publish by server)
//  - get directory path to copy from cmd, send it as a msg
//  - Receiving messages (callbacks) were data is stored
//
//
// USAGE:
//     mqttpp_chat <user> <group>

/*******************************************************************************
 *2020 Corona times, Shiran Lupo Madmon
/*******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
#include "mqtt/topic.h"

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	// The broker/appClint address pc
	const std::string SERVER_ADDRESS("10.100.102.6");

	// The QoS to use for publishing and subscribing
	const int QOS = 1; //TODO

	// Tell the broker we don't want our own messages sent back to us.
	const bool NO_LOCAL = true;

	if (argc != 3) {
		std::cout << "USAGE: mqttpp_chat <user> <group>" << std::endl;
		return 1;
	}

	std::string chatUser  { argv[1] },
				chatGroup { argv[2] },
				chatTopic { "chat/"+chatGroup };

	// LWT message is broadcast to other users if out connection is lost

	auto lwt = mqtt::make_message(chatTopic, "<<<"+chatUser+" was disconnected>>>", QOS, false);

	// Set up the connect options

	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_mqtt_version(MQTTVERSION_5);
	connOpts.set_clean_start(true);
	connOpts.set_will_message(lwt);

	mqtt::async_client cli(SERVER_ADDRESS, "");

	// Set a callback for connection lost.
	// This just exits the app.

	cli.set_connection_lost_handler([](const std::string&) {
		std::cout << "*** Connection Lost  ***" << std::endl;
		exit(2);
	});

	// Set the callback for incoming messages

	cli.set_message_callback([](mqtt::const_message_ptr msg) {
		std::cout << msg->get_payload_str() << std::endl;
	});

	// We publish and subscribe to one topic,
	// so a 'topic' object is helpful.

	mqtt::topic topic { cli, "chat/"+chatGroup, QOS };

	// Start the connection.

	try {
		std::cout << "Connecting to the chat server at '" << SERVER_ADDRESS
			<< "'..." << std::flush;
		auto tok = cli.connect(connOpts);
		tok->wait();

		// Subscribe to the topic using "no local" so that
		// we don't get own messages sent back to us

		std::cout << "Ok\nJoining the group..." << std::flush;
		auto subOpts = mqtt::subscribe_options(NO_LOCAL);
		topic.subscribe(subOpts)->wait();
		std::cout << "Ok" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << "\nERROR: Unable to connect. "
			<< exc.what() << std::endl;
		return 1;
	}

	// Let eveyone know that a new user joined the conversation.

	topic.publish("<<" + chatUser + " joined the group>>");

	// Read messages from the console and publish them.
	// Quit when the use enters an empty line.

	std::string usrMsg;

	while (std::getline(std::cin, usrMsg) && !usrMsg.empty()) {
		usrMsg = chatUser + ": " + usrMsg;
		topic.publish(usrMsg);
	}

	// Let eveyone know that the user left the conversation.

	topic.publish("<<" + chatUser + " left the group>>")->wait();

	// Disconnect

	try {
		std::cout << "Disconnecting from the chat server..." << std::flush;
		cli.disconnect()->wait();
		std::cout << "OK" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << exc.what() << std::endl;
		return 1;
	}

 	return 0;
}

