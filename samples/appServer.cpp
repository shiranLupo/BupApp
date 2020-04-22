// appServer.cpp
//
//
//setting connection
//while
//start consuming msg
//if topic is subscribtion
//subscribe to the client topic
// add client to clints list -
// else :// it is backup request
// do backup from msgTopic= ip +path
// for publish success : to msgTopic
//

//TODO in wich topic to handle disconnection lwr and unsbscribe.
//handle lwr
//handle duplicated topic!

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

const string subscribersListTopic{"subscribersList"}; //here server get msgs from client to subscribe to the backup servies
const char *STUB_LOCAL_IP = "100.10.102.5 ";
const string LOCAL_IP = "100.10.102.5 ";

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

    auto lwt = mqtt::make_message(subscribersListTopic, "Server was disconnected>>>", QOS, false);
    // Set up the connect options
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_mqtt_version(MQTTVERSION_5);
    connOpts.set_clean_start(true);
    connOpts.set_will_message(lwt);

    mqtt::async_client serverClient(brokerAddress, "");
    mqtt::subscribe_options subOptions(NO_LOCAL);

    // Set a callback for connection lost.
    // This just exits the app.
    serverClient.set_connection_lost_handler([](const std::string &) {
        std::cout << "*** Connection Lost  ***" << endl;
        exit(2);
    });

    try
    {
        std::cout << "Connecting server to broker in: " << brokerAddress<< " ..." << endl;
        auto tok = serverClient.connect(connOpts);
        tok->wait();
        std::cout << "...OK" << endl;

        // Subscribe to the topic using "no local" so that
        // we don't get own messages sent back to us
        std::cout << "Subscribing to clients list..." << std::endl;
        serverClient.subscribe(subscribersListTopic, QOS, subOptions)->wait();

        serverClient.start_consuming();
        std::cout << "...OK" << endl;

        string msgTopic;
        string msgPayload;
        string subscriberIp;
        string pathToBackUp;
        // Consume messages
        while (true)
        {
            // mqtt::const_message_ptr mp;
            auto msgPtr = serverClient.consume_message();
            if (!msgPtr)
            {
                break;
            }
            msgTopic = msgPtr->get_topic();
            msgPayload = msgPtr->get_payload();
            //TODO insert to vector of clients
            //TODO here should perform subscription
            if (msgTopic == subscribersListTopic)
            {
                //handle subscription
                subscriberIp = msgPayload;
                cout << "Subscribtion request msg was recieved from: "<< subscriberIp << endl;
                mqtt::topic clientsOnlineTopic(serverClient, subscribersListTopic + "\'" + subscriberIp + "\'online", QOS, true);
                clientsOnlineTopic.subscribe()->wait();

                //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic

                mqtt::topic topicPerClientOnline(serverClient, subscriberIp + "\'online", QOS, true);
                topicPerClientOnline.subscribe()->wait();

                std::cout << "Subscribing to new client backup req channel..." << std::endl;
                //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic
                mqtt::topic topicPerClientBackups(serverClient, subscriberIp + "\'backups", QOS, true);
                topicPerClientBackups.subscribe()->wait();
                std::cout << "...OK" << endl;
            }
            else //backup request topic
            {
                //handle unsbsribe or lwt from clients
                subscriberIp = msgTopic;
                pathToBackUp = msgPayload;
                cout << "BackUp request msg was recieved from: "<< subscriberIp << endl;
                string subscriberTopic =  subscriberIp + "\'backups";
                string replyPayload = LOCAL_IP + "\'" + "..............";
                // do backup from msgTopic= ip +path
                // for publish success : to msgTopic
                //msgTopic = path to client +
                pathToBackUp = msgPayload;
                cout << "Backingup data from : " << subscriberIp << "\'" << pathToBackUp << endl;
                //TODO real backup
                mqtt:: message_ptr replyPtr= mqtt::make_message( subscriberTopic,replyPayload,QOS, true);
                cout << "Publishing to client : " << subscriberIp << " path of backup"<< endl;

                serverClient.publish(replyPtr)->wait();
                std::cout << "...OK" << endl;

            }
        }

        serverClient.stop_consuming();
        //TODO usbscribing to all topics
    }
    catch (const mqtt::exception &exc)
    {
        std::cerr << "\nERROR: Unable to connect. "
                  << exc.what() << std::endl;
        return 1;
    }

    return 0;
}
