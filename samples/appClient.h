

//appClient.h

#ifndef __appClient_h
#define __appClient_h

#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"
#include "mqttConfigs.h"

using namespace std;



namespace BupApp
{

const string SERVER_PUBLIC_KEY_TARGET = "/home/shiranlupo/.ssh/authorized_keys";
const string SUBSCIBERS_LIST = "subscribersList";

class appClient : private mqttConfigs
{
private:
    /* data */
    mqtt::async_client_ptr m_appClient;
    string m_subscribeMsg;
    string m_privateChnl;
    string m_subscribeToServerTopic;

    string m_msgTopic;
    string m_msgPayload;
    string m_serverPublicKey;
    // string m_newSubscriberIp;
    // string m_pathToBackUp;

    void connectToServer();
    void setupConnection();
    void handleBackupRequest();
    void handleServerReplyMsg();
    void handlePubKeyMsg(string msg);

public:
    appClient(int argc, const char *argv[]);
    ~appClient();

    void init();
    void working();
    void disconnect();

    string getBackupRequestPath();
    string getBackupRepleyPath();
};

} //end of namespace BupApp

#endif // end of ifndef