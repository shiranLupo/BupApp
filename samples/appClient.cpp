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
// client method:
//  - Connecting to an MQTT server/broker.
//  - Publishing topic (backUp req), wait and verify a backup server is listening (callback)
//  - Subscribing to a topic(confim and path -to be publish by server)
//  - get directory path to copy from cmd, send it as a msg
//  - Receiving messages (callbacks) were data is stored

//server method
//  - Connecting to an MQTT server/broker.
//  - Subscribing to a topic(backUp req/client addres) /all clients backup reqs- use wilde card
//  - once msg is recieved- approch to copy the data to local directory
//  - publish a topic(confim and path

//
// USAGE:
//     mqttpp_chat <user> <group>

/*******************************************************************************
 *2020 Corona times, Shiran Lupo Madmon
/*******************************************************************************/

#include <iostream>
#include <cstdlib>
#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"

using namespace std;

const char *STUB_LOCAL_IP = "100.10.102.9 ";

const string DFLT_BROKER_ADDRESS{"tcp://localhost:1883"};
//const string DFLT_SERVER_ADDRESS { "100.10.102.6" }; //pc ip

const string TOPIC1("clientList");

const int QOS = 1; //TODO check QOS (0,1,2)


const auto TIMEOUT = std::chrono::seconds(10);

/////////////////////////////////////////////////////////////////////////////

/**
 * A callback class for use with the main MQTT client.
 */
class callback : public virtual mqtt::callback
{
public:
    void connection_lost(const string &cause) override
    {
        cout << "\nConnection lost" << endl;
        if (!cause.empty())
            cout << "\tcause: " << cause << endl;
    }

    void delivery_complete(mqtt::delivery_token_ptr tok) override
    {
        cout << "\tDelivery complete for token: "
             << (tok ? tok->get_message_id() : -1) << endl;
    }
};

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char const *argv[])
{
    string appClientIP(STUB_LOCAL_IP); //TODO get local ip
    string TOPIC2(appClientIP + "backupPaths");

    //optional to change broker by cmnd line TODO change set server by config
    string brokerAddress = (argc > 1) ? string(argv[1]) : DFLT_BROKER_ADDRESS;
    cout << "Initializing contact with broker " << brokerAddress << "..." << endl;

    mqtt::async_client appClient(brokerAddress, "");

    try
    {
        //TODO do we need connect option obj? what its pros
        //connecting client to broker, wait until complete;
        appClient.connect()->wait();
        cout << "...OK" << endl;

        //publish subscribe messeg TODO: wait until complite( server meanwhile will publish client new topic)
        cout << "\nSending subscribtion msg to server..." << endl;
        // example: https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c

        //     mqtt::message subscribeToService(TOPIC, appClientIP.c_str(), QOS, true);
        //     mqtt::message_ptr msgPtr(&subscribeToService);
        //     mqtt::token_ptr pulishToken(appClient.publish(msgPtr));

        mqtt::message_ptr subscribeMsgToken = mqtt::make_message(TOPIC1, appClientIP, QOS, true);
        mqtt::token_ptr publishToken(appClient.publish(subscribeMsgToken));
        publishToken->wait();
        cout << "...OK" << endl;

       // publishToken->get_subscribe_response();

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    //start while loop, wait for request for backup
    try
    {
        //publish  messeg to privat topic that was created by server upon subscribe
        cout << "\nEnter path for backup..." << endl;
        string path;
        getline(std::cin,path);
       
        cout << "\nSending backup request msg to server..." << endl;


        mqtt::message_ptr subscribeMsgToken = mqtt::make_message(TOPIC2 ,path , QOS, true);
        mqtt::delivery_token_ptr publishToken(appClient.publish(subscribeMsgToken));
        publishToken->wait();
        cout << "...OK" << endl;
      //  publishToken->get_subscribe_response();

    }

    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
