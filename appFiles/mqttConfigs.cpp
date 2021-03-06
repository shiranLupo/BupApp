// mqttConfigs.cpp
#include "mqttConfigs.h"

///////////////////////////////////////////////////////////////////////////////
namespace BupApp
{
    BupApp::mqttConfigs::mqttConfigs(int argc, const char *argv[]) : m_qos(QOS),
                                                                     m_retained(RETAINED),
                                                                     m_connectOpt(NULL),
                                                                     m_subOptions(NO_LOCAL)
    {
        //optional to change broker by cmnd line TODO change set server by config
        try
        {
            if (argc != 3)
            {
                throw;
            }
        }
        catch (int argc)
        {
            std::cerr << "Broker address or local adress or both need to be insert"
                      << " as follow: ./BupApp [local_ip] [broker_ip]" << endl;
        }

        m_localIp = argv[1];
        m_brokerAdress = argv[2];

        // Set up the connect options
        m_connectOpt = std::make_shared<mqtt::connect_options>();

        m_connectOpt->set_keep_alive_interval(20);
        m_connectOpt->set_mqtt_version(MQTTVERSION_5);
        m_connectOpt->set_clean_start(true);
        // m_connectOpt->set_clean_session(true);
    }

    BupApp::mqttConfigs::~mqttConfigs()
    {
    }

    std::string BupApp::mqttConfigs::getLocalIp()
    {
        return (mqttConfigs::m_localIp);
    }

    std::string BupApp::mqttConfigs::getBrokerAddress()
    {
        return (mqttConfigs::m_brokerAdress);
    }

    bool BupApp::mqttConfigs::getRetained()
    {
        return (m_retained);
    }

    int BupApp::mqttConfigs::getQos()
    {
        return (m_qos);
    }

    mqtt::connect_options_ptr BupApp::mqttConfigs::getConnectionOpt()
    {
        return (m_connectOpt);
    }

    mqtt::subscribe_options BupApp::mqttConfigs::getSubOptions()
    {
        return (m_subOptions);
    }

} // namespace BupApp