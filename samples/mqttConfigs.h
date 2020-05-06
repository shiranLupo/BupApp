///mqttConfigs.h

#ifndef __mqttConfigs_h
#define __mqttConfigs_h

#include <iostream>
#include <cstdlib>
#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"

using namespace std;

const int QOS = 1;
const bool RETAINED = false;
const bool NO_LOCAL = true;

namespace BupApp
{

class mqttConfigs
{
private:
    /* data */
    string m_localIp;
    string m_brokerAdress;
    int m_qos;
    bool m_retained;
    mqtt::connect_options_ptr m_connectOpt;

public:
    mqttConfigs(int argc, const char *argv[]);
    ~mqttConfigs();
    string getLocalIp();
    string getBrokerAddress();
    int getQos();
    bool getRetained();
    mqtt::connect_options_ptr getConnectionOpt();

    bool getConfigs(); //will use ini file to get ip and broker address
};
} // namespace BupApp

#endif //end of ifndef