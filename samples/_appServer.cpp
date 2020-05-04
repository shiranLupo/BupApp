//_appServer.cpp
#include "appServer.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace BupApp;
using namespace utils;

namespace BupApp
{

    appServer::appServer(int argc, const char *argv[]) : mqttConfigs(argc, argv), m_publicKey(""),
                                                         m_appServer(NULL), m_msgPtr(NULL),
                                                         m_commonServerClientTopic(SUBSCIBERS_LIST), m_currClient("- - -"),
                                                         m_subscriberIp(""), m_pathToBackUp("")

    {
        //CLog::Write(CLog::Debug, "appServer Ctor\n");
        cout << "app server ctor" << endl;
        std::string m_publicKey = getPublicKey();

        connectToServer();
        setupConnection();
        //TODO set dir for backup, deafualt is Desktop;
    }

    appServer::~appServer()
    {
    }

    void BupApp::appServer::connectToServer()
    {

        auto lwt = mqtt::make_message(m_commonServerClientTopic, "Server was disconnected>>>", QOS, RETAINED);
        mqttConfigs::getConnectionOpt()->set_will_message(lwt);
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

        m_appServer->start_consuming();
        while (true)
        {
            m_msgPtr = m_appServer->consume_message();
            if (!m_msgPtr)
            {
                break;
            }
            // string msgTopic = m_msgPtr->get_topic();
            // string msgPayload = m_msgPtr->get_payload();
            // cout << "OK... msg recieved (topic): " << msgPayload << " (" << msgTopic << ")" << endl;

            if (m_msgPtr->get_topic() == m_commonServerClientTopic)
            {
                handleNewSubscriber(); //: add to vec, open topic and new Dir
            }
            // TODO msg class
            // TODO seprete client class from utils
            else
            {
                if (isIP4(m_msgPtr->get_topic()))
                {
                    cout << "msg topic is ip4 valid" << endl;
                    handleBackupRequest();
                    // TODO handleReplyBackupRequest();
                }

                // else
                // {
                //     cout << "msg recieved is not new suscriber or backup req " << endl;

                //     // // cout << "Sl080518" << endl;
                //     // string replyPayload = LOCAL_IP + "/" + "..............";

                //     // mqtt::message_ptr replyPtr = mqtt::make_message(subscriberTopic, replyPayload, QOS, true);
                //     // cout << "Publishing to client : " << subscriberIp << " path of backup" << endl;

                //     // serverClient.publish(replyPtr)->wait();
                //     // std::cout << "...OK" << endl;
                // }
            }
        }

        m_appServer->stop_consuming();
        //TODO usbscribing to all topics
    }

    void BupApp::appServer::handleNewSubscriber()
    {
        string msgTopic = m_msgPtr->get_topic();
        string msgPayload = m_msgPtr->get_payload();
        try
        {
            //handle subscription
            //m_currClient(m_msgPayload); // TODO appropreite operator
            utils::client currClient(msgPayload);
            currClient.printClient();

            cout << "Subscribtion request msg was recieved from: " << currClient.getIp() << endl;
            //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic

            std::cout << "Subscribing to new client backup req channel..." << std::endl;
            mqtt::topic topicPerClient(*m_appServer, currClient.getIp(), QOS, RETAINED);
            topicPerClient.subscribe()->wait();
            std::cout << "...OK" << endl;

            std::cout << "Sending new subscriber public key via private chnl..." << std::endl;
            topicPerClient.publish(m_publicKey)->wait();
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

    string BupApp::appServer::getPublicKey()
    {
        cout << "getPubKey" << endl;

        fstream in_file(PUBLIC_KEY_PATH);
        if (!in_file)
        {
            cout << "can not get public key. Error: in opening file" << endl;
            return ("");
        }

        string ret;
        char c;
        while (in_file.get(c)) // loop getting single characters
        {
            ret += c;
        }

        if (ret.size() != 415)
        {
            cout << "can not get public key. Error: not correct key" << endl;
            return ("");
        }
        return (ret);
    }

    void BupApp::appServer::handleBackupRequest()
    {

        string m_subscriberIp = m_msgPtr->get_topic();
        string m_pathToBackUp = m_msgPtr->get_payload();
        //TODO should find the client in the vector
        string user = searchForClient().getUser();

        cout << "BackUp request msg was recieved from: " << m_subscriberIp << endl;
        cout << "from this is the user : "<< user <<endl;


        string pathTarget = mqttConfigs::getLocalIp() + ":~/Desktop/";

        if (m_subscriberIp != mqttConfigs::getLocalIp())
        {

            //TODO make back up dir per cleint
            //string pathTarget = "~/Desktop/" + subscriberIp;

            string cmnd = "scp -r "+ user + "@" + m_subscriberIp + ":" + m_pathToBackUp + " " + pathTarget;
            cout << "cmnd for sys is: " << cmnd << endl;
            system(cmnd.c_str());
        }
        else
        {
            cout << "This is local backup: no need to use scp" << endl;
            //TODO hanlde local backup
        }
    }

    client BupApp::appServer::searchForClient()
    {
        auto itr = m_clients.begin();
        for (; itr != m_clients.end() && itr->getIp() != m_msgPtr->get_topic(); itr++)
        {
        }

        if (itr != m_clients.end())
        {
            return (*itr);
        }

        return (client(""));
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