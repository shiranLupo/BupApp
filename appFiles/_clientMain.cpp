//@appClientTest.cpp
#include "appClient.h"
#include "mqttConfigs.h"
#include "utils.h"


using namespace  BupApp;
using namespace utils;

int main(int argc, char const *argv[])
{
    BupApp::appClient pc(argc, argv);
    pc.init();
    pc.working();
    pc.disconnect();
    
    return 0;
}
