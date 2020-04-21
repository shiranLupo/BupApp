// mqttpp_chat.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// The "chat" application is practically the "Hello World" application for
// messaging systems. This allows a user to type in message to send to a
// "group" while seeing all the messages that the other members of the group
// send.
//
// This application is an MQTT publisher/subscriber using the C++
// asynchronous client interface, employing callbacks to receive messages
// and status updates.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker.
//  - Publishing messages.
//  - Subscribing to a topic
//  - Receiving messages (callbacks) through a lambda function
//
// USAGE:
//     mqttpp_chat <user> <group>

/*******************************************************************************
 * Copyright (c) 2019 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
#include "mqtt/topic.h"

using namespace std;

const string TOPIC{"clientList"}; //here server get msgs from client to subscribe to the backup servies
const char *STUB_LOCAL_IP = "100.10.102.5 ";
//const string DFLT_BROKER_ADDRESS{"100.10.102.9 "};//TODO get broker ip
const string DFLT_BROKER_ADDRESS{"tcp://localhost:1883"};
// The QoS to use for publishing and subscribing
const int QOS = 1;

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    //optional to change broker by cmnd line TODO change set server by config
    string brokerAddress = (argc > 1) ? string(argv[1]) : DFLT_BROKER_ADDRESS;
    cout << "Initializing contact with broker '" << brokerAddress << "'..." << endl;

    // Tell the broker we don't want our own messages sent back to us.
    const bool NO_LOCAL = true;

    auto lwt = mqtt::make_message(TOPIC, "server was disconnected>>>", QOS, false);

    // Set up the connect options
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_mqtt_version(MQTTVERSION_5);
    connOpts.set_clean_start(true);
    connOpts.set_will_message(lwt);

    mqtt::async_client serverClient(brokerAddress, "");

    // Set a callback for connection lost.
    // This just exits the app.
    serverClient.set_connection_lost_handler([](const std::string &) {
        std::cout << "*** Connection Lost  ***" << std::endl;
        exit(2);
    });

    string clientTopic;
    string pathToBackUp;

    try
    {
        std::cout << "Connecting to the server " << brokerAddress
                  << "..." << std::endl;
        auto tok = serverClient.connect(connOpts);
        tok->wait();
        std::cout << "...Ok" << std::endl;

        // Subscribe to the topic using "no local" so that
        // we don't get own messages sent back to us
        std::cout << "subscribing to clients list..." << std::endl;
        serverClient.start_consuming();
        serverClient.subscribe(TOPIC, QOS)->wait();

        std::cout << "...Ok" << std::endl;

        // Consume messages
        while (true)
        {
            std::chrono::duration<int, std::milli> ms(3000); // 3000 seconds
            mqtt::const_message_ptr mp;
            auto result = serverClient.try_consume_message_for(&mp,ms);
            if (!result)
                break;
            clientTopic =  mp->get_payload();

            //TODO insert to vec tor of clients
            //TODO here should perform subscription
        }

        serverClient.stop_consuming();
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "\nERROR: Unable to connect. "
                  << exc.what() << std::endl;
        return 1;
    }

    //TODO loop via all client and perform
try
{
   std::cout << "subscribing to client " << clientTopic << " back up requests list..." << std::endl;
    std::cout << "...Ok" << std::endl;
    serverClient.start_consuming();
    clientTopic = clientTopic+"backup";
    serverClient.subscribe(clientTopic, QOS)->wait();
    //TODO for each msg recieved: cout: backing up
    std::cout << "working on client " << clientTopic << " requests ..." << std::endl;
    // TODO handle backup request
     // Consume messages
        while (true)
        {
            std::chrono::duration<int, std::milli> ms(3000); // 3000 seconds
            mqtt::const_message_ptr mp;
            auto result = serverClient.try_consume_message_for(&mp,ms);
            if (!result)
                break;
            pathToBackUp =  mp->get_payload();

            //TODO insert to vec tor of clients
            //TODO here should perform subscription
        }
    serverClient.publish(clientTopic, "succes : backUpPath");
    std::cout << "...Ok" << std::endl;
}
catch(const std::exception& e)
{
    std::cerr << e.what() << '\n';
}

    


    return 0;
}
//setting connection 
//while
//start consuming msg 
//if topic is subscribtion
   //subscribe to the client topic
   // add client to clints list - 
// else :// it is backup request
   // do backup from msgTopic= ip +path
    // for publish success : to msgTopic