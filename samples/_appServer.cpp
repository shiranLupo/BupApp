//_appServer.cpp
#include "appServer.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace BupApp;
using namespace utils;

namespace BupApp
{

appServer::appServer(int argc, const char *argv[]) : mqttConfigs(argc, argv),
                                                     m_appServer(NULL),
                                                     m_commonServerClientTopic(SUBSCIBERS_LIST),
                                                     m_subscriberIp(""), m_msgPayload(""), m_msgTopic(""), m_pathToBackUp("")

{
    CLog::Write(CLog::Debug, "appServer Ctor\n");
    connectToServer();
    setupConnection();
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
        auto msgPtr = m_appServer->consume_message();
        if (!msgPtr)
        {
            break;
        }
        m_msgTopic = msgPtr->get_topic();
        m_msgPayload = msgPtr->get_payload();
        cout << "OK... msg recieved (topic): " << m_msgPayload << " (" << m_msgTopic << ")" << endl;
        msgType type = getTopicType(m_msgTopic);

        switch (type)
        {
        case BACKUP:
            /* code */
            break;

        default:
            break;
        }
        if (m_msgTopic == m_commonServerClientTopic)
        {
            handleNewSubscriber(); //: add to vec, open topic and new Dir
        }
        else if (client::isIP4(m_msgTopic))
        {
            handleBackupRequest();
        }

        else
        {
            cout << "msg recieved is not new suscriber or backup req " << endl;

            // // cout << "Sl080518" << endl;
            // string replyPayload = LOCAL_IP + "/" + "..............";

            // mqtt::message_ptr replyPtr = mqtt::make_message(subscriberTopic, replyPayload, QOS, true);
            // cout << "Publishing to client : " << subscriberIp << " path of backup" << endl;

            // serverClient.publish(replyPtr)->wait();
            // std::cout << "...OK" << endl;
        }
    }

    m_appServer->stop_consuming();
    //TODO usbscribing to all topics
}

void BupApp::appServer::handleNewSubscriber()
{
    //handle subscription
    m_subscriberIp = m_msgPayload;
    cout << "Subscribtion request msg was recieved from: " << m_subscriberIp << endl;
    // mqtt::topic clientsOnlineTopic(appServer, m_commonServerClientTopic + "/" + m_subscriberIp + "/online", QOS, true);
    // clientsOnlineTopic.subscribe()->wait();

    //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic
    mqtt::topic topicPerClientOnline(*m_appServer, m_subscriberIp + "/online", QOS, RETAINED);
    topicPerClientOnline.subscribe()->wait();

    //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic
    mqtt::topic topicPerClientPwd(*m_appServer, m_subscriberIp + "/password", QOS, RETAINED);
    topicPerClientPwd.subscribe()->wait();
    mqtt::topic topicPerClientid(*m_appServer, m_subscriberIp + "/id", QOS, RETAINED);
    topicPerClientid.subscribe()->wait();

    std::cout << "Subscribing to new client backup req channel..." << std::endl;
    //TODO publish in online==true msg, lwr will change it to false . and subscribe to this topic
    mqtt::topic topicPerClientBackups(*m_appServer, m_subscriberIp, QOS, RETAINED);
    topicPerClientBackups.subscribe()->wait();
    std::cout << "...OK" << endl;
}

void BupApp::appServer::handleBackupRequest()
{

    //need to find msgTopic in subscriber vector
    cout << "msg topic is ip4 valid" << endl;
    m_subscriberIp = m_msgTopic;

    m_pathToBackUp = m_msgPayload;
    cout << "BackUp request msg was recieved from: " << m_subscriberIp << endl;
    // // do backup from msgTopic= ip +path
    // // for publish success : to msgTopic
    // //msgTopic = path to client +
    // //TODO make back up dir per cleint
    string pathTarget = m_subscriberIp + ":~/Desktop/";
    //string pathTarget = "~/Desktop/" + subscriberIp;

    // cout << "Backingup data from : " << subscriberIp << "/" << pathToBackUp << endl;
    // //TODO real backup
    string cmnd = "scp -r shiranLupo@" + m_subscriberIp + ":" + m_pathToBackUp + " " + pathTarget;
    // // //  string cmnd ="scp -r pi@10.100.102.2:~/Desktop\blafolder11 ~/Desktop";
    cout << "cmnd for sys is: " << cmnd << endl;
    system(cmnd.c_str());
}

msgType BupApp::appServer ::getTopicType(string topic)
{
    if (topic == m_commonServerClientTopic)
        return (NEW_CLIENT);
    else if (isIP4(topic))
        return (BACKUP);
    else if (topic.find("id") != string::npos)
        return (ID);
    else if (topic.find("password") != string::npos)
        return (PWD);
}

} // namespace BupApp