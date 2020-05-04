//@appClient.cpp

#include "appClient.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace std;
using namespace utils;

namespace BupApp
{

appClient::appClient(int argc, const char *argv[]) : mqttConfigs(argc, argv),
                                                     m_appClient(NULL),
                                                     m_subscribeMsg(""),
                                                     m_subscribeToServerTopic(SUBSCIBERS_LIST),
                                                     m_privateChnl(mqttConfigs::getLocalIp())
{
    cout << "appClient Ctor" << endl;
    cout << "Insert yor ip, user name and pwd" << endl;
    getline(cin, m_subscribeMsg);

    connectToServer();
    setupConnection();
}

appClient::~appClient()
{
    cout << "appClient Dtor" << endl;
}

void BupApp::appClient::connectToServer()
{
    string backupChnlTopic = mqttConfigs::getLocalIp();

    //optional to change broker by cmnd line TODO change set server by config
    cout << "Initializing contact with broker " << mqttConfigs::getBrokerAddress() << "..." << endl;
    m_appClient = std::make_shared<mqtt::async_client>(getBrokerAddress(), "");
    try
    {
        //TODO do we need connect option obj? what its pros
        //connecting client to broker, wait until complete;
        m_appClient->connect()->wait();
        cout << "...OK" << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void BupApp::appClient::setupConnection()
{
    try
    {

        //publish subscribe messeg TODO: wait until complite( server meanwhile will publish client new topic)
        cout << "\nSending subscribtion msg to server..." << endl;
        // example: https://stackoverflow.com/questions/49335001/get-local-ip-address-in-c

        m_appClient->publish(m_subscribeToServerTopic, m_subscribeMsg , getQos(), getRetained())->wait();
        cout << "...OK" << endl;
//TODO recieve ssh

        cout << "\nSubscribtion to get server disconnecting msg..." << endl;
        //clients should be subscribed to this topic only to get lwt, consider case in consume
        m_appClient->subscribe(m_subscribeToServerTopic, getQos())->wait();
        cout << "...OK" << endl;

        cout << "\nSubscribtion to backup chanle (send backup req, recieve backup location)..." << endl;
        m_appClient->subscribe(m_privateChnl, getQos())->wait();
        cout << "...OK" << endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void BupApp::appClient::working()
{

    //TODO let threads handle the msg
    handleBackupRequest();
    handleServerReplyMsg();
}

void ::BupApp::appClient::disconnect()
{
    cout << "dissconnect" << endl;
}

void BupApp::appClient::handleServerReplyMsg()
{

    //TODO insert to vector of clients
    //TODO here should perform subscription

    while (true)
    {
        try
        { // mqtt::const_message_ptr mp;
            cout << "try consume_message" << endl;
            auto msgPtr = m_appClient->consume_message();
            if (!msgPtr)
            {
                break;
            }

            m_msgTopic = msgPtr->get_topic();
            m_msgPayload = msgPtr->get_payload();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    if (m_msgTopic == m_privateChnl)
    {
        cout << "Backup succeed! data is here : " << m_msgPayload << endl;
    }
    else if( m_msgTopic == m_subscribeToServerTopic)
    {    
        //TODO handlePubKeyMsg();
        // add key to authorized list in .ssh dir

    }
    
}

void BupApp::appClient::handleBackupRequest()
{
    while (true)
    {

        //start while loop, wait for request for backup

        //publish  messeg to privat topic that was created by server upon subscribe
        cout << "\nEnter path for backup..." << endl;
        string path;
        getline(std::cin, path);

        try
        {
            cout << "\nSending backup path msg to server..." << endl;
            m_appClient->publish(m_privateChnl, path, getQos(), getRetained())->wait();
            cout << "...OK" << endl;
            //  publishToken->get_subscribe_response();
        }

        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

} //end of namespace BupApp
