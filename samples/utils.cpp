//utils.cpp
#include "utils.h"

namespace utils
{

    bool CLog::m_bInitialised;
    int CLog::m_nLevel;

    void CLog::Write(int nLevel, const char *szFormat, ...)
    {
        CheckInit();
        if (nLevel >= m_nLevel)
        {
            va_list args;
            va_start(args, szFormat);
            vprintf(szFormat, args);
            va_end(args);
        }
    }
    void CLog::SetLevel(int nLevel)
    {
        m_nLevel = nLevel;
        m_bInitialised = true;
    }
    void CLog::CheckInit()
    {
        if (!m_bInitialised)
        {
            Init();
        }
    }
    void CLog::Init()
    {
        int nDfltLevel(CLog::All);
        // Retrieve your level from an environment variable,
        // registry entry or wherecer
        SetLevel(nDfltLevel);
    }

    client::client(std::string msg)
    {
        std::istringstream ss(msg);
        vector<string> tokens{
            istream_iterator<string>{ss},
            istream_iterator<string>{}};

        m_ip = (isIP4(tokens[0]) ? tokens[0] : "");
        m_user = (tokens.size() == 2 ? tokens[1] : "");
        m_pwd = (tokens.size() == 3 ? tokens[2] : "");
    }

    client::~client()
    {
    }

    void client::printClient()
    {
        cout << m_ip << endl;
        cout << m_user << endl;
        cout << m_pwd << endl;
    }

    string client::getIp()
    {
        return (m_ip);
    }
    string client::getUser()
    {
        return (m_user);
    }

    bool isIP4(string msgTopic)
    {

        cout << "perfome ip4 check" << endl;
        //int inet_pton(int af, const char *src, void *dst);
        unsigned long dst;
        return (inet_pton(AF_INET, msgTopic.c_str(), &dst) == 1 ? true : false);
    }

} //end namespace utils