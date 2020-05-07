//_appServer.cpp
#include "appServer.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace std;
using namespace BupApp;
using namespace utils;
using namespace this_thread;

namespace BupApp
{

    appServer::appServer(int argc, const char *argv[]) : mqttConfigs(argc, argv),
                                                         m_appServer(NULL), m_msgPtr(NULL),
                                                         m_commonServerClientTopic(SUBSCIBERS_LIST), m_currClient("- - -"),
                                                         m_subscriberIp(""), m_pathToBackUp("")

    {
        //CLog::Write(CLog::Debug, "appServer Ctor\n");
        cout << "app server ctor" << endl;
        // std::string m_publicKey = getPublicKey();
        getPublicKey(m_publicKey);
        cout << "this is the public key to send 1:" << m_publicKey << endl;
        connectToServer();
        setupConnection();
        //TODO set dir for backup, deafualt is Desktop;
    }

    appServer::~appServer()
    {
    }

    void BupApp::appServer::connectToServer()
    {
        //optional to change broker by cmnd line TODO change set server by config
        cout << "Initializing contact with broker " << mqttConfigs::getBrokerAddress() << "..." << endl;
        m_appServer = std::make_shared<mqtt::async_client>(getBrokerAddress(), "");

        // Set a callback for connection lost.
        // This just exits the app.
        m_appServer->set_connection_lost_handler([](const std::string &) {
            std::cout << "*** Connection Lost  ***" << endl;
            exit(2);
        });
        try
        {
            //TODO do we need connect option obj? what its pros
            //connecting client to broker, wait until complete;
            m_appServer->connect(*getConnectionOpt())->wait();
            cout << "...OK" << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void BupApp::appServer::setupConnection()
    {
        mqtt::subscribe_options subOptions(NO_LOCAL);

        try
        {
            // Subscribe to the topic using "no local" so that
            // we don't get own messages sent back to us

            std::cout << "Subscribing to clients list..." << std::endl;
            m_appServer->subscribe(m_commonServerClientTopic, QOS, subOptions)->wait();

            std::cout << "...OK" << endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    void BupApp::appServer::working()
    {

        cout << "start Consuming msgs" << endl;
        m_appServer->start_consuming();
        while (true)
        {
            cout << "try consume msgs" << endl;
            m_msgPtr = m_appServer->consume_message();
            if (!m_msgPtr)
            {
                //TODO check reconnect, seperate it to handle
                // if (!m_appServer->is_connected())
                // {
                //     cout << "Lost connection. Attempting reconnect" << endl;
                //     if (tryReconnect())
                //     {
                //         m_appServer->subscribe(m_commonServerClientTopic, QOS);
                //         //TODO handle reconnection with all clients??
                //         cout << "Reconnected" << endl;
                //         continue;
                //     }
                //     else
                //     {
                //         cout << "Reconnect failed." << endl;
                //         break;
                //     }
                // }
                // else
                //     break;

                break;
            }

            if (m_msgPtr->get_topic() == m_commonServerClientTopic)
            {
                //TODO if subscriber allredy exist do not operate handle
                handleNewSubscriber(); //: add to vec, open topic and new Dir
            }
            // TODO msg class
            // TODO seprete client class from utils
            else
            {
                if (isIP4(m_msgPtr->get_topic())) //&& !isClientExist(m_msgPtr->get_topic()))
                {
                    cout << "msg topic is ip4 valid" << endl;
                    handleBackupRequest();
                    // TODO handleReplyBackupRequest();
                }

                //TODO handle disconnect , do not remove from vector
                //TODO handle usbscribe , remove from vector
            }
        }

        m_appServer->stop_consuming();
        //TODO usbscribing to all topics
    }

    bool BupApp::appServer::tryReconnect()
    {
        constexpr int N_ATTEMPT = 30;

        for (int i = 0; i < N_ATTEMPT && !m_appServer->is_connected(); ++i)
        {
            try
            {
                m_appServer->reconnect();
                return true;
            }
            catch (const mqtt::exception &)
            {
                std::this_thread::sleep_for(chrono::seconds(1));
            }
        }
        return false;
    }

    void BupApp::appServer::handleNewSubscriber()
    {
        string msgTopic = m_msgPtr->get_topic();
        string msgPayload = m_msgPtr->get_payload();
        try
        {
            utils::client currClient(msgPayload);
            currClient.printClient();

            cout << "Subscribtion request msg was recieved from: " << currClient.getIp() << endl;

            std::cout << "Subscribing to new client backup req channel..." << std::endl;
            mqtt::topic topicPerClient(*m_appServer, currClient.getIp(), QOS, RETAINED);

            auto lwt = mqtt::make_message(topicPerClient.to_string(), "Server was disconnected>>>", QOS, RETAINED);
            mqttConfigs::getConnectionOpt()->set_will_message(lwt);
            
            topicPerClient.subscribe()->wait();
            std::cout << "...OK" << endl;

            std::cout << "Sending new subscriber public key via private chnl..." << std::endl;
            cout << "this is the public key to send 2:" << m_publicKey << endl;

            topicPerClient.publish(m_publicKey.c_str(), m_publicKey.size(), QOS, RETAINED)->wait();
            std::cout << "...OK" << endl;

            //TODOcreate dir per client
            //currClient.setBackupTarget();

            m_clients.push_back(currClient);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    //TODO in init server only once

    void BupApp::appServer::getPublicKey(string &pubkey)
    {
        cout << "getPubKey" << endl;
        pubkey = getTxtFromFile(PUBLIC_KEY_PATH);

        cout << pubkey.size() << endl
             << pubkey << endl;

        if (pubkey.size() != 415)
        {
            cout << "can not get public key. Error: not correct key" << endl;
            pubkey = "";
        }
    }

    void BupApp::appServer::handleBackupRequest()
    {

        string m_subscriberIp = m_msgPtr->get_topic();
        string m_pathToBackUp = m_msgPtr->get_payload();
        //TODO should find the client in the vector
        string user = searchForClient(m_subscriberIp).getUser();

        cout << "BackUp request msg was recieved from: " << m_subscriberIp << endl;
        cout << "from this is the user : " << user << endl;

        string pathTarget = ":~/Desktop/";
        //TODO check path + user working?>>>> user +"@" +mqttConfigs::getLocalIp()+":" + ":~/Desktop/"

        if (m_subscriberIp != mqttConfigs::getLocalIp())
        {

            //TODO make back up dir per cleint
            //string pathTarget = "~/Desktop/" + subscriberIp;

            string cmnd = "scp -r " + user + "@" + m_subscriberIp + ":" + m_pathToBackUp + " " + pathTarget;
            cout << "cmnd for sys is: " << cmnd << endl;
            system(cmnd.c_str());
        }
        else
        {
            cout << "This is local backup: no need to use scp" << endl;
            //TODO hanlde local backup
        }
    }

    client BupApp::appServer::searchForClient(string ip)
    {
        auto itr = m_clients.begin();
        for (; itr != m_clients.end() && itr->getIp() != ip; itr++)
        {
        }

        if (itr != m_clients.end())
        {
            return (*itr);
        }

        return (client(""));
    }
    bool BupApp::appServer::isClientExist(client &cli)
    {
        auto itr = m_clients.begin();
        for (; itr != m_clients.end() && *itr != cli; itr++)
        {
        }

        return (itr != m_clients.end() ? true : false);
    }

    bool BupApp::appServer::isClientExist(string ip)
    {
        client ref("");
        client found = searchForClient(ip);
        return (found == ref ? false : true);
    }

    // msgType BupApp::appServer::getTopicType(string topic)
    // {
    //     if (topic == m_commonServerClientTopic)
    //         return (NEW_CLIENT);
    //     else if (isIP4(topic))
    //         return (BACKUP);
    //     else if (topic.find("id") != string::npos)
    //         return (ID);
    //     else if (topic.find("password") != string::npos)
    //         return (PWD);
    // }

} // namespace BupApp