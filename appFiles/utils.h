// utils.h
#ifndef _utils_h
#define _utils_h

#include <time.h>
#include <arpa/inet.h> //ip4 vlaidation
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdarg>
#include <vector>
//#include <algorithm>
#include <iterator>
#include <fstream>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <array>
#include <stdarg.h> //logger

using namespace std;

namespace utils
{

    bool isIP4(string msgTopic);

    class client
    {
    private:
        /* data */
        string m_ip;
        string m_user;
        string m_pwd;
        string m_backupPath;

    public:
        client(string msg);
        ~client();
        client(const client &); // copy constructor

        void printClient();
        string getIp();
        string getUser();
        string getBackupPathTarget();
        bool operator==(client lft);
        bool operator!=(client lft);
        void setBackupTarget(string);
    };

    string getFullFilePath(string targetFile, string user);
    void addStrToFile(string strToAppend, string targetFile, string user);
    string getTxtFromFile(string path);
    bool isTxtExist(string txt, string filePath);
    string execCmnd(string cmnd);
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

    class CLogger
    {
    public:
        void Write(const std::string &sMessage);

        //  @param format string for the message to be logged.
       void Write(const char *format, ...);

        // << overloaded function to Logs a message
        CLogger &operator<<(const string &sMessage);

        static CLogger *GetLogger();

        ~CLogger();

    private:
  
        CLogger();
   
        CLogger(const CLogger &){}; 
    
        CLogger &operator=(const CLogger &) 
        { return *this; }; 
    
        static const std::string m_sFileName;

    //   Singleton logger class object pointer.   
        static CLogger *m_pThis;
        static ofstream m_Logfile;
    };

} //end namespace  utils

#endif //end ifndef _utils_h