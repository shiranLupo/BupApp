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
        cout << "Starting app local BupApp client..." << endl;

        cout << "Insert yor ip, user name and pwd" << endl;
        getline(cin, m_subscribeMsg); //TODO double ip input- remove one
        cout << endl
             << endl;
        m_clientInfo = client(m_subscribeMsg);
        connectToServer();
        setupConnection();

        cout << "Client initialization succeed ..." << endl
             << endl
             << endl;
    }

    appClient::~appClient()
    {
    }

    void BupApp::appClient::connectToServer()
    {
        string backupChnlTopic = mqttConfigs::getLocalIp();

        //optional to change broker by cmnd line TODO change set server by config
        cout << "Initializing contact with broker " << mqttConfigs::getBrokerAddress() << "...";
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
            auto lwt = mqtt::make_message(m_clientInfo.getIp(), m_clientInfo.getIp() + " was disconnected>>>", QOS, RETAINED);
            mqttConfigs::getConnectionOpt()->set_will_message(lwt);

            //publish subscribe messeg TODO: wait until complite( server meanwhile will publish client new topic)
            cout << "Sending subscribtion msg to public server channle...";

            m_appClient->publish(m_subscribeToServerTopic, m_subscribeMsg, getQos(), getRetained())->wait();
            cout << "...OK" << endl;

            m_appClient->start_consuming();

            cout << "Subscribtion to private backup channle...";
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
                //TODO recieve ssh
                cout << "Try consume massage..." << endl;
                auto msgPtr = m_appClient->consume_message();
                if (!msgPtr)
                {
                    break;
                }

                string msgTopic = msgPtr->get_topic();
                string msgPayload = msgPtr->get_payload();
                cout << msgPayload << endl;

                if (msgTopic == m_privateChnl)
                {
                    if (msgPayload.find("ssh-rsa") != string::npos)
                    {
                        //TODO handlePublicKey
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
        cout << "PublicKey was recieve, handle publickey ...";
        m_serverPublicKey = msg;

        if (!utils::isTxtExist(msg, getFullFilePath( SERVER_PUBLIC_KEY_TARGET , user)))
        {
            utils::addStrToFile(m_serverPublicKey, SERVER_PUBLIC_KEY_TARGET, user);
        }
        cout << "...OK" << endl;
    }

    void BupApp::appClient::handleBackupRequest()
    {
        cout << "PublicKey was recieve, handle publickey ...";

        while (true)
        {

            cout << "Enter path for backup..." << endl;
            string path;
            getline(std::cin, path);

            try
            {
                cout << "Sending backup path msg to server...";
                m_appClient->publish(m_privateChnl, path, getQos(), getRetained())->wait();
                cout << "...OK" << endl;
            }

            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

} //end of namespace BupApp
