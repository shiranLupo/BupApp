

//appClient.h

#include <iostream>
#include <cstdlib>
#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"

using namespace std;

namespace BupApp
{

const string SUBSCIBERS_LIST = "subscribersList";
const string BACKUPS = "\'backups";
const string PASSWORD = "\'password";

class mqttConfigs
{
private:
    /* data */
    string m_localIp;
    string m_brokerAdress;
    int m_qos;
    bool m_retained;

public:
    mqttConfigs(int argc, const char *argv[]);
    ~mqttConfigs();
    string getLocalIp();
    string getBrokerAddress();
    int getQos();
    bool getRetained();

    bool getConfigs(); //will use ini file to get ip and broker address
};

class appClient : private mqttConfigs
{
private:
    /* data */
    mqtt::async_client_ptr m_appClient;
    string m_clientId;
    string m_clientPwd;
    string m_backupChnlTopic;
    string m_subscribeToServerTopic;
    string m_pwdTopic;

    string m_msgTopic;
    string m_msgPayload;
    // string m_newSubscriberIp;
    // string m_pathToBackUp;

    void connectToServer();
    void setupConnection();
    void handleBackupRequest();
    void handleServerReplyMsg();

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