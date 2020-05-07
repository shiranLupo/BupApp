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
                                                         m_privateChnl(mqttConfigs::getLocalIp()), m_clientInfo("- - -")
    {
        cout << "appClient Ctor" << endl;
        cout << "Insert yor ip, user name and pwd" << endl;
        getline(cin, m_subscribeMsg); //TODO double ip input- remove one
        m_clientInfo = client(m_subscribeMsg);
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
             m_clientInfo.printClient();
            auto lwt = mqtt::make_message(m_clientInfo.getIp(), m_clientInfo.getIp() + " was disconnected>>>", QOS, RETAINED);
            mqttConfigs::getConnectionOpt()->set_will_message(lwt);

            //publish subscribe messeg TODO: wait until complite( server meanwhile will publish client new topic)
            cout << "\nSending subscribtion msg to server..." << endl;

            m_appClient->publish(m_subscribeToServerTopic, m_subscribeMsg, getQos(), getRetained())->wait();
            cout << "...OK" << endl;
            //TODO recieve ssh

            m_appClient->start_consuming();

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

       // TODO let threads handle the msg
        // std::thread mqttThread(handleServerReplyMsg);
        // handleServerReplyMsg();
        // vector<thread>some_threads;
        // for (int i = 0; i < 4; ++i)
        //     some_threads.push_back(std::thread(&appClient::handleBackupRequest, this));
        // for (auto &t : some_threads)
        //     t.join();

    //    std::thread(&appClient::handleBackupRequest);
    //     std::thread(&appClient::handleServerReplyMsg);
        std::thread t2(&appClient::handleBackupRequest, *this);
        std::thread t1(&appClient::handleServerReplyMsg, this);
        t1.join();
        t2.join();
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
                cout << msgPayload << endl;

                if (msgTopic == m_privateChnl)
                {
                    if (msgPayload.find("ssh-rsa") != string::npos)
                    {
                        //TODO handlePublicKey
                        cout << "PublicKey was recieve, handle publickey ..." << endl;
                        handlePubKeyMsg(msgPayload, m_clientInfo.getUser());
                    }
                    else
                    {
                        cout << "Backup succeed! data is here : " << msgPayload << endl;
                    }
                }
                else if (msgTopic == m_subscribeToServerTopic)
                {
                    //TODO handlePubKeyMsg(); //TODO add condition id this key already exist nno need to add
                    // add key to authorized list in .ssh dir
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

    void BupApp::appClient::handlePubKeyMsg(string msg, string user)
    {
        m_serverPublicKey = msg;
        utils::addStrToFile(m_serverPublicKey, SERVER_PUBLIC_KEY_TARGET, user);
    }

    void BupApp::appClient::handleBackupRequest()
    {
        while (true)
        {

            //start while loop, wait for request for backup

            //publish  messeg to privat topic that was created by server upon subscribe
            cout << "\nEnter path for backup..." << endl;
            this_thread::sleep_for(chrono::seconds(1));
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
