

//appClient.h

#ifndef __appClient_h
#define __appClient_h

#include <string>
#include <thread> // For sleep
#include <atomic>
#include <chrono>
#include <cstring>
#include "mqtt/async_client.h"
#include "mqttConfigs.h"
#include "utils.h"

using namespace std;

namespace BupApp
{

    static bool GOT_PUBLIC_KEY = false;

    enum msgType
    {
        PUBLIC_KEY,


    };

    const string SERVER_PUBLIC_KEY_TARGET = ".ssh/authorized_keys";
    const string SUBSCIBERS_LIST = "subscribersList";

    class appClient : private mqttConfigs
    {
    private:
        /* data */
        mqtt::async_client_ptr m_appClient;
        string m_subscribeMsg;
        string m_privateChnl;
        string m_publicChnl;
        utils::client m_clientInfo;

        string m_msgTopic;
        string m_msgPayload;
        string m_serverPublicKey;

        std::thread m_cmdThread;
        std::thread m_commThread;

        bool isMsgTypeOf(string type, string &msg);

        void setClientInfo();
        void connectToServer();
        void clientSetups();
        void getPubicKey();
        void handleBackupRequest();
        void handleServerReplyMsg();
        void getPublicKeyMsg();
        void handlePubKeyMsg(string msg, string user);

    public:
        appClient(int argc, const char *argv[]);
        ~appClient();

        void init();
        void working();
        void disconnect();

        // string getBackupRequestPath();
        // string getBackupRepleyPath();
    };

} //end of namespace BupApp

#endif // end of ifndef