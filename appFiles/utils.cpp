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

    client:: client(const client& l) // copy constructor 
    {
        this->m_ip = l.m_ip;
        this->m_user = l.m_user;
        this->m_pwd = l.m_pwd;
    }

    string client::getIp()
    {
        return (m_ip);
    }
    string client::getUser()
    {
        return (m_user);
    }

    bool client::operator==(client lft)
    {
        return (this->getUser() == lft.getUser() && this->getIp() == lft.getIp() ? true : false);
    }

    bool client::operator!=(client lft)
    {
        return (!(*this == lft));
    }

    bool isIP4(string msgTopic)
    {

        // cout << "Perfome ip4 check" << endl;
        unsigned long dst;
        return (inet_pton(AF_INET, msgTopic.c_str(), &dst) == 1 ? true : false);
    }

    void addStrToFile(string strToAppend, string targetFile , string user)
    {

        cout << targetFile << endl;
        cout << strToAppend << endl;
        auto start = std::chrono::system_clock::now();
        std::ofstream out;

        // std::ios::app is the open mode "append" meaning
        // new data will be written to the end of the file.
        string fullPath= "/home/" + user + "/" + targetFile;
        cout<<fullPath<<endl;
        out.open(fullPath, std::ios::app);

        std::string str = strToAppend;
        out << str;

        out.close();
    }

    string getTxtFromFile(string path)
    {
        fstream in_file(path);
        if (!in_file)
        {
            cout << "can getTxtFromFile: in opening file" << endl;
            return ("");
        }

        string ret;
        char c;
        while (in_file.get(c)) // loop getting single characters
        {
            ret += c;
        }
        return (ret);
    }

} //end namespace utils