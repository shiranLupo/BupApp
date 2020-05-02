// utils.h
#ifndef _utils_h
#define _utils_h

#include <arpa/inet.h> //ip4 vlaidation
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

namespace utils
{


class client
{
private:
    /* data */
    string m_ip;
    string m_user;
    string m_pwd;
    
    bool isIP4(string msgTopic);

    
public:
    client(string msg);
    ~client();
    void printClient();
};


class CLog
{
public:
    enum
    {
        All = 0,
        Debug,
        Info,
        Warning,
        Error,
        Fatal,
        None
    };
    static void Write(int nLevel, const char *szFormat, ...);
    static void SetLevel(int nLevel);

protected:
    static void CheckInit();
    static void Init();

private:
    CLog();
    static bool m_bInitialised;
    static int m_nLevel;
};

} //end namespace  utils

#endif //end ifndef _utils_h