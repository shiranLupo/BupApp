// _serverTest.cpp

#include "appServer.h"

using namespace BupApp;

int main(int argc, char const *argv[])
{
    cout << "main" << endl;
    BupApp::appServer server(argc, argv);

    //std::cout << server.getPublicKey() << std::endl;
    server.working();
    return 0;
}
