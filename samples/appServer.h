//appServer.h

#ifndef __appServer_h
#define __appServer_h

#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include <vector>
#include "mqtt/async_client.h"
#include "mqttConfigs.h"

using namespace std;

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

struct client
{
    string clientId;
    string lientPwd;
    string ip;
};

class appServer : private mqttConfigs
{
private:
    /* data */
    mqtt::async_client_ptr m_appServer;
    vector<client> m_clients;
    string m_commonServerClientTopic;
    string m_pwdTopic;

    string m_msgTopic;
    string m_msgPayload;
    std::string m_subscriberIp;
    string m_pathToBackUp;

    void connectToServer();
    void setupConnection();
    void handleBackupRequest();
    void handleNewSubscriber();
    msgType getTopicType(string topic);
    //void mkSubscriberDir(); //TODO

public:
    appServer(int argc, const char *argv[]);
    ~appServer();

   // void init();
   void working();
  //  void disconnect();

};

} // namespace BupApp

#endif // end ifndef