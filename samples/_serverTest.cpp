// _serverTest.cpp

#include "appServer.h"

using namespace BupApp;

int main(int argc, char const *argv[])
{
    appServer server(argc, argv);  
    server.working();  
    return 0;
}
