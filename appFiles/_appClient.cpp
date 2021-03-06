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
                                                         m_publicChnl(SUBSCIBERS_LIST),
                                                         m_privateChnl(mqttConfigs::getLocalIp()), m_clientInfo("- - -")
    {
    }

    void BupApp::appClient::init()
    {
        cout << "Starting local BupApp client..." << endl;

        setClientInfo();
        connectToServer();
        clientSetups();
        getPublicKeyMsg(); //TODO should me init time limited? as no cli will wait to loop for ever

        cout << "Client initialization succeed ..." << endl
             << endl;
    }

    appClient::~appClient()
    {
        m_cmdThread.join();
        m_commThread.join();
        m_appClient->stop_consuming();
        m_appClient->unsubscribe(m_privateChnl);
        m_appClient->unsubscribe(m_publicChnl);
        m_appClient->disconnect()->wait();
        cout << "client is  dissconnecting" << endl;
    }

    void appClient::setClientInfo()
    {
        cout << "Insert user name and pwd" << endl;
        string clientInfo;
        getline(cin, clientInfo); //TODO double ip input- remove one
        cout << endl
             << endl;
        m_subscribeMsg = mqttConfigs::getLocalIp() + " " + clientInfo;
        m_clientInfo = client(m_subscribeMsg);
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

    void BupApp::appClient::clientSetups()
    {
        try
        {
            cout << "Setting lwt ...";
            auto lwt = mqtt::make_message(m_clientInfo.getIp(), m_clientInfo.getIp() + " was disconnected>>>", QOS, RETAINED);
            mqttConfigs::getConnectionOpt()->set_will_message(lwt);
            cout << "OK" << endl;

            //publish subscribe messeg TODO: wait until complite( server meanwhile will publish client new topic)
            cout << "Sending subscribtion msg to public server channle...";

            m_appClient->publish(m_publicChnl, m_subscribeMsg, getQos(), getRetained())->wait();
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

        m_cmdThread = thread{&appClient::handleBackupRequest, this};
        m_commThread = thread{&appClient::handleServerReplyMsg, this};
    }

    void BupApp::appClient::disconnect()
    {
    }

    bool BupApp::appClient::isMsgTypeOf(string type, string &msg)
    {
        return (msg.find(type) != string::npos ? true : false);
    }

    void BupApp::appClient::getPublicKeyMsg()
    {
        while (!GOT_PUBLIC_KEY)
        {
            try
            {
                cout << "Waiting for server response..." << endl;
                auto msgPtr = m_appClient->consume_message();
                if (!msgPtr)
                {
                    break;
                }

                string msgTopic = msgPtr->get_topic();
                string msgPayload = msgPtr->get_payload();

                if (msgTopic == m_privateChnl)
                {
                    //TODO isTypeOf(string type, msg) //is this ssh?
                    if (isMsgTypeOf("ssh-rsa", msgPayload))
                    {
                        //TODO handlePublicKey
                        handlePubKeyMsg(msgPayload, m_clientInfo.getUser());
                        GOT_PUBLIC_KEY = true;
                    }
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

        if (!utils::isTxtExist(msg, getFullFilePath(SERVER_PUBLIC_KEY_TARGET, user)))
        {
            utils::addStrToFile(m_serverPublicKey, SERVER_PUBLIC_KEY_TARGET, user);
        }
        cout << "...OK" << endl;
    }

    void BupApp::appClient::handleBackupRequest()
    {

        while (true)
        {

            cout << "Enter path for backup. Path format: ~/[folder2]/[folder1]..." << endl;
            string path;
            getline(std::cin, path); //TODO is it better to get pathi via argv?

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

    void BupApp::appClient::handleServerReplyMsg()
    {
        while (true)
        {
            try
            {
                CLogger::GetLogger()->Write("handleServerReplyMsg: Waiting for server response...");
                auto msgPtr = m_appClient->consume_message();
                if (!msgPtr)
                {
                    break;
                }

                string msgTopic = msgPtr->get_topic();
                string msgPayload = msgPtr->get_payload();

                if (msgTopic == m_privateChnl)
                {
                    //get success or fail msg about backup
                    cout << msgPayload << endl;
                    CLogger::GetLogger()->Write(m_msgPayload); 
                }
                else
                {
                    //get msg from public chnl  
                    CLogger::GetLogger()->Write("handleServerReplyMsg: msg was recieved not from privat channle: ");
                    CLogger::GetLogger()->Write(msgTopic + msgPayload + "\n");

                    cout << "msg was recieved not from privat channle: " << msgTopic << " " << msgPayload << endl;
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }

} //end of namespace BupApp
