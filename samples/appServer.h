//appServer.h

#ifndef __appServer_h
#define __appServer_h

#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include <vector>
#include <fstream>  //get public key
#include <iostream> // std::cin, std::cout
#include <algorithm> //find client

#include "mqtt/async_client.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace std;
using namespace utils;

const string PUBLIC_KEY_PATH = "/home/shiranlupo/.ssh/id_rsa.pub";
//TODO generic path shiranlupo= sever, get user
const string SUBSCIBERS_LIST = "subscribersList";

namespace BupApp
{

    enum msgType
    {
        NEW_CLIENT,
        ID,
        PWD,
        BACKUP,
        DISSCONNECT
    };

    const string SUBSCIBERS_LIST = "subscribersList";

    class appServer : private mqttConfigs
    {
    private:
        /* data */
        string m_publicKey;
        mqtt::async_client_ptr m_appServer;
        vector<utils::client> m_clients;
        utils::client m_currClient;

        string m_commonServerClientTopic;
        mqtt::const_message_ptr m_msgPtr;
        string m_subscriberIp;
        string m_pathToBackUp;

        void connectToServer();
        void setupConnection();
        void handleBackupRequest();
        void handleNewSubscriber();
        //msgType getTopicType(string topic);
        client searchForClient();
    
        //  void mkSubscriberDir(); //TODO
        // setBackupTarget(); TODO

    public:
        appServer(int argc, const char *argv[]);
        ~appServer();
        void getPublicKey(string &pubkey);

        // // void init();
        void working();
        //  void disconnect();
    };

} // namespace BupApp

#endif // end ifndef