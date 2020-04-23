

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

class appClientConfigs
{
private:
    /* data */
   string m_localIp;
   string m_brokerAdress;


public:
    appClientConfigs();
    ~appClientConfigs();
    string getLocalIp();
    string getBrokerAdress();
    bool getConfigs(); //will use ini file to get ip and broker address

};



class appClient : private appClientConfigs
{
private:
    /* data */
    bool connectToServer(); 
    void handleBackupRequest();
    void handleServerReplyMsg();

public:
    appClient(/* args */);
    ~appClient();

    void init();
    void working();
    void disconnect();

    string getBackupRequestPath();
    string getBackupRepleyPath();
};



} //end of namespace BupApp