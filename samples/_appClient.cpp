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
        //TODOadd member client :: m_clientInfo(m_subscribeMsg);

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

            m_appClient->publish(m_subscribeToServerTopic, m_subscribeMsg, getQos(), getRetained())->wait();
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
        m_appClient->start_consuming();

        //TODO let threads handle the msg
        handleServerReplyMsg();
        handleBackupRequest();

        m_appClient->stop_consuming();
    }

    void ::BupApp::appClient::disconnect()
    {
        cout << "dissconnect" << endl;
    }

    void BupApp::appClient::handleServerReplyMsg()
    {
        while (true)
        {
            try
            { // mqtt::const_message_ptr mp;
                //   cout << "try consume_message" << endl;
                auto msgPtr = m_appClient->consume_message();
                if (!msgPtr)
                {
                    break;
                }
                cout << "msg was recieved" << endl;

                string msgTopic = msgPtr->get_topic();
                string msgPayload = msgPtr->get_payload();
                cout<<msgPayload<<endl;

                if (msgTopic == m_privateChnl)
                {
                    if (msgPayload.find("ssh-rsa") != string::npos)
                    {
                        //TODO handlePublicKey
                        cout << "PublicKey was recieve, handle publickey ..." << endl;
                        handlePubKeyMsg(msgPayload);
                    }
                    else
                    {
                        cout << "Backup succeed! data is here : " << msgPayload << endl;
                    }
                }
                else if (msgTopic == m_subscribeToServerTopic)
                {
                    //TODO handlePubKeyMsg();
                    // add key to authorized list in .ssh dir
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    void BupApp::appClient::handlePubKeyMsg(string msg)
    {
        m_serverPublicKey = msg;
        utils::addStrToFile(m_serverPublicKey, SERVER_PUBLIC_KEY_TARGET);
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
