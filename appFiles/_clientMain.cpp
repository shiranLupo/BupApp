//@appClientTest.cpp
#include "appClient.h"
#include "mqttConfigs.h"
#include "utils.h"


using namespace  BupApp;
using namespace utils;

int main(int argc, char const *argv[])
{
    cout<<"test main"<<endl;
    
    BupApp::appClient pc(argc, argv);
    pc.init();
    pc.working();
    pc.disconnect();
    
//     string key = getTxtFromFile("/home/shiranlupo/.ssh/id_rsa.pub");
//     cout<<key << key.size()<<endl;
//    utils::addStrToFile(key, "/home/shiranlupo/.ssh/authorized_keys");

    return 0;
}
