//@appClientTest.cpp
#include "appClient.h"

using namespace  BupApp;

int main(int argc, char const *argv[])
{
    cout<<"test main"<<endl;
    
    BupApp::appClient pc(argc, argv);
    pc.working();
    pc.disconnect();
    

    return 0;
}
