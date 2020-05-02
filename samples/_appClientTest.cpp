//@appClientTest.cpp
#include "appClient.h"
#include "mqttConfigs.h"
#include "utils.h"


using namespace  BupApp;
using namespace utils;

int main(int argc, char const *argv[])
{
    cout<<"test main"<<endl;
    
    // BupApp::appClient pc(argc, argv);
    // pc.working();
    // pc.disconnect();
    
    string msg = "10.100.102.2 shiranLupo ";
    client shiran(msg);
    shiran.printClient();

    return 0;
}
