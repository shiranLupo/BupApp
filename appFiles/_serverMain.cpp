// _serverTest.cpp

#include "appServer.h"

using namespace BupApp;

int main(int argc, char const *argv[])
{
    BupApp::appServer server(argc, argv);
    server.init();
    server.working();
    return 0;
}
