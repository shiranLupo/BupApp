//_appServer.cpp
#include "appServer.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace BupApp;
using namespace this_thread;

namespace BupApp
{

    appServer::appServer(int argc, const char *argv[]) : mqttConfigs(argc, argv),
                                                         m_appServer(NULL), m_msgPtr(NULL),
                                                         m_publicChnl(SUBSCIBERS_LIST)
    {
    }

    void BupApp::appServer::init()
    {
        cout << "Starting app server..." << endl;

        getPublicKey(m_publicKey);
        connectToServer();
        serverSetups();
        cout << "Server initialization succeed...." << endl
             << endl
             << endl;
        //TODO set dir for backup, deafualt is Desktop;
    }

    appServer::~appServer()
    {
    }

    void BupApp::appServer::connectToServer()
    {
        //TODO optional to change broker by cmnd line TODO change set server by config
        cout << "Initializing: connecting to broker " << mqttConfigs::getBrokerAddress() << "...";
        m_appServer = std::make_shared<mqtt::async_client>(getBrokerAddress(), "");

        // TODO Set a callback for connection lost.
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

    void BupApp::appServer::serverSetups()
    {
        cout << "Setup communication channles with service clients ...";
        mqtt::subscribe_options subOptions(NO_LOCAL);

        try
        {
            // Subscribe to the topic using "no local" so that
            // we don't get own messages sent back to us
            m_appServer->subscribe(m_publicChnl, QOS, subOptions)->wait();
            //clear public channle
            //m_appServer->publish(m_publicChnl,new byte[0],QOS,RETAINED);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
        std::cout << "...OK" << endl;

        string cmnd = "mkdir " + MAIN_BACKUP_PATH;
        system(cmnd.c_str());
    }

    void BupApp::appServer::working()
    {
        cout << "Server starts consuming msgs: initialize.... " << endl;
        m_appServer->start_consuming();
        while (true)
        {
            cout << "Try consume msg..." << endl
                 << endl
                 << endl;
            m_msgPtr = m_appServer->consume_message();
            if (!m_msgPtr && !checkReconnect())
            {
                break;
            }
            //TODO chech this search 
            else if (m_msgPtr->get_topic() == m_publicChnl) //&& !isClientExist(m_msgPtr->get_topic()))
            {
                //TODO if subscriber allredy exist do not operate handle
                handleNewSubscriber();
            }
            // TODO msg class
            // TODO seprete client class from utils
            else if (isIP4(m_msgPtr->get_topic()))
            {
                m_threadPool.push_back(thread(&appServer::handleBackupRequest, this));
                //TODO handle disconnect , do not remove from vector
                //TODO handle usbscribe , remove from vector
            }
        }

        m_appServer->stop_consuming();
        //TODO usbscribing to all topics
    }

    bool BupApp::appServer::checkReconnect()
    {
        if (!m_appServer->is_connected())
        {
            cout << "Lost connection. Attempting reconnect" << endl;
            if (tryReconnect())
            {
                
            m_appServer->subscribe(m_publicChnl, QOS, getSubOptions())->wait();
               
                //TODO handle reconnection with all clients??
                cout << "Reconnected" << endl;
                //server was succesfully reconnected
                return (true);
            }
            else
            {
                cout << "Reconnect failed." << endl;
            }
        }
        //no need to reconnect or failed to connect
        return (false);
    }

    void BupApp::appServer::handleNewSubscriber()
    {
        cout << "Server handles new subscriber request ....";
        //: add to vec, open topic and new Dir
        string msgTopic = m_msgPtr->get_topic();
        string msgPayload = m_msgPtr->get_payload();
        try
        {
            utils::client currClient(msgPayload);
            cout << "from: " << currClient.getIp() << " " << currClient.getUser() << endl;

            //preparing lwt msg
            cout << "Setting lwt ...";
            auto lwt = mqtt::message(currClient.getIp(), "Server was disconnected>>>", QOS, RETAINED);
            mqtt::will_options will(lwt);
            mqttConfigs::getConnectionOpt()->set_will(will);
            cout << "OK " << endl;

            std::cout << "Server subscribig to new client backup req channel...";
            mqtt::topic topicPerClient(*m_appServer, currClient.getIp(), QOS, RETAINED);
            
            topicPerClient.subscribe(getSubOptions())->wait();

            std::cout << "...OK" << endl;

            std::cout << "Sending new subscriber public key via private chnl...";
            topicPerClient.publish(m_publicKey.c_str(), m_publicKey.size(), QOS, RETAINED)->wait();
            std::cout << "...OK" << endl;

            //TODOcreate dir per client
            currClient.setBackupTarget(MAIN_BACKUP_PATH + "/" + currClient.getIp());
            string cmnd = "mkdir " + currClient.getBackupPathTarget();
            system(cmnd.c_str());

            m_clients.push_back(currClient);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }

        cout << "Server handled new subscriber request succed...." << endl
             << endl
             << endl;
    }

    //TODO in init server only once

    void BupApp::appServer::getPublicKey(string &pubkey)
    {
        cout << "Extract Server public key....";
        pubkey = getTxtFromFile(PUBLIC_KEY_PATH);

        if (pubkey.size() != PUBLIC_KEY_SIZE)
        {
            cout << "Can not get public key. Error: not correct key" << endl;
            pubkey = "";
        }
        cout << "OK" << endl;
        // cout << pubkey.size() << endl<< pubkey << endl;
    }

    void BupApp::appServer::handleBackupRequest()
    {
        //cout << this_thread::get_id() << endl;

        string subscriberIp = m_msgPtr->get_topic();
        string backUpPath = m_msgPtr->get_payload();

       //TODO is it neccerry???
        if (backUpPath.size() > 0)
        {
            //TODO should find the client in the vector
            client currClient = searchForClient(subscriberIp);

            cout << "Server handles backUp request. msg was recieved from: " << subscriberIp << endl;
            cout << "from this user : " << currClient.getUser() << endl;
            cout << "to this path : " << currClient.getBackupPathTarget() << endl;

            //TODO check path + user working?>>>> user +"@" +mqttConfigs::getLocalIp()+":" + ":~/Desktop/"

            string cmnd;
            if (subscriberIp != mqttConfigs::getLocalIp())
            {
                cmnd = "scp -r " + currClient.getUser() + "@" + subscriberIp + ":" + backUpPath + " " + currClient.getBackupPathTarget();
               // cout << "cmnd for sys is: " << cmnd << endl;
            }
            else
            {
                cout << "This is local backup: no need to use scp" << endl;
                cmnd = "cp -r " + backUpPath + " " + currClient.getBackupPathTarget();
            }

            string reply = execCmnd(cmnd);

            cout << "publish reply to client ...";
            m_appServer->publish(subscriberIp, reply, getQos(), getRetained())->wait();

            cout << "OK" << endl;
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

    // msgType BupApp::appServer::getTopicType(string topic)
    // {
    //     if (topic == m_publicChnl)
    //         return (NEW_CLIENT);
    //     else if (isIP4(topic))
    //         return (BACKUP);
    //     else if (topic.find("id") != string::npos)
    //         return (ID);
    //     else if (topic.find("password") != string::npos)
    //         return (PWD);
    // }

} // namespace BupApp