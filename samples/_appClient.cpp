//@appClient.cpp

#include "appClient.h"

using namespace std;

namespace BupApp
{

appClient::appClient(/* args */)
{
    cout<<"appClient Ctor"<<endl;

}

appClient::~appClient()
{
    cout<<"appClient Dtor"<<endl;

}

///////////////////////////////////////////////////////////////

BupApp::appClientConfigs::appClientConfigs(/* args */)
{
    cout<<"appClientConfigs Ctor"<<endl;
}

BupApp::appClientConfigs::~appClientConfigs()
{
    cout<<"appClientConfigs Dtor"<<endl;

}

} //end of namespace BupApp
