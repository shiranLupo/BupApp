//utils.cpp

#include "utils.h"

namespace utils
{

     // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    const std::string CurrentDateTime()
    {
        time_t     now = time(NULL);
        struct tm  tstruct;
        char       buf[80];
       // localtime_r(&tstruct, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        return buf;
    }


    const string CLogger::m_sFileName = "Log.txt";
    CLogger *CLogger::m_pThis = NULL;
    ofstream CLogger::m_Logfile;
   
    CLogger::CLogger() {}

    CLogger::~CLogger()
    {
        m_Logfile.close();
    }

    CLogger *CLogger::GetLogger()
    {
        if (m_pThis == NULL)
        {
            m_pThis = new CLogger();
            // m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
            m_Logfile.open(m_sFileName.c_str());
        }
        return m_pThis;
    }

    void CLogger::Write(const char *format, ...)
    {
        string msg(format);
        m_Logfile<<msg<<endl;
    }

    void CLogger::Write(const string &sMessage)
    {
      //  m_Logfile << CurrentDateTime() << ":\t";
        m_Logfile << sMessage << "\n";
    }

    CLogger &CLogger::operator<<(const string &sMessage)
    {
        m_Logfile << "\n";
 //                 << CurrentDateTime() << ":\t";
        m_Logfile << sMessage << "\n";
        return *this;
    }

        bool CLog::m_bInitialised;
        int CLog::m_nLevel;

       
        client::client(std::string msg)
        {
            std::istringstream ss(msg);
            vector<string> tokens{
                istream_iterator<string>{ss},
                istream_iterator<string>{}};

            m_ip = (isIP4(tokens[0]) ? tokens[0] : "");
            m_user = (tokens.size() == 2 ? tokens[1] : "");
            m_pwd = (tokens.size() == 3 ? tokens[2] : "");
            m_backupPath = "";
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

        client::client(const client &l) // copy constructor
        {
            this->m_ip = l.m_ip;
            this->m_user = l.m_user;
            this->m_pwd = l.m_pwd;
            this->m_backupPath = l.m_backupPath;
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

        //NOTICE: per OS (ubunto and resberrian)
        string getFullFilePath(string targetFile, string user)
        {
            // cout << "getFullFilePath : "
            //     << "/home/" << user + "/" << targetFile << endl;
            return ("/home/" + user + "/" + targetFile);
        }

        void addStrToFile(string strToAppend, string targetFile, string user)
        {
            std::ofstream out;
            try
            {
                /* code */
                // std::ios::app is the open mode "append" meaning
                // new data will be written to the end of the file.
                out.open(getFullFilePath(targetFile, user), std::ios::app);

                std::string str = strToAppend;
                out << str;

                out.close();
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        string getTxtFromFile(string path)
        {
            fstream in_file(path);

            try
            {
                /* code */
                if (!in_file)
                {
                    throw;
                    cout << "Error getTxtFromFile: can not open file" << endl;
                    return ("");
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }

            string ret;
            char c;
            while (in_file.get(c)) // loop getting single characters
            {
                ret += c;
            }
            return (ret);
        }
        string client::getBackupPathTarget()
        {
            return (m_backupPath);
        }

        void client::setBackupTarget(string path)
        {
            m_backupPath = path;
        }

        bool isTxtExist(string txt, string filePath)
        {
            fstream in_file(filePath);
            if (!in_file)
            {
                cout << "Error isTxtExist: can not open file" << endl;
                return ("");
            }
            string existFile = getTxtFromFile(filePath);
            return (existFile.find(txt) == string::npos ? false : true);
        }

        string execCmnd(string cmnd)
        {

            std::array<char, 128> buffer;
            std::string result;
            auto pipe = popen(cmnd.c_str(), "r");

            if (!pipe)
                throw std::runtime_error("popen() failed!");

            while (!feof(pipe))
            {
                if (fgets(buffer.data(), 128, pipe) != nullptr)
                    result += buffer.data();
            }

            auto rc = pclose(pipe);

           // return (rc == EXIT_SUCCESS ? "succeed!" : "failed! " + result);
           return (rc == EXIT_SUCCESS ? "succeed!" : "failed! " );
        }

    } //end namespace utils