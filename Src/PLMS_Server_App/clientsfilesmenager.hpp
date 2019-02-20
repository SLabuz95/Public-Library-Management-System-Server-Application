#ifndef CLIENTSFILESMENAGER_HPP
#define CLIENTSFILESMENAGER_HPP
// ------------------ Macros --------------------------------------------
#define CLIENTS_FILE_NAME (QString("clients"))
#define CLIENTS_FILE_BACKUP_NAME (QString("clientsBU"))
#define TEMP_FILE_NAME (QString("temp"))
#define CLIENTS_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"clients\" !!! -------------"))
#define TEMP_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"temp\" !!! -------------"))

#define USER_FAST_ACCESS_STEP (20)
#define MAX_ACTIVITY_VALUE (100)

// Include macros
// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<stdint.h>
#include"userparametersenum.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class User;
class QFile;
class ReadFileRules;
class QString;
class MyTcpSocket;
// ----------------------------------------------------------------------

// ------------------ App Class -----------------------------------------

struct UserFastAccess{
    unsigned long long id = 0;
    unsigned long long filePosition = 0;
};

struct UserLoggedFastAccess{
    unsigned long long id = 0;
    unsigned long long filePosition = 0;
    short activity = MAX_ACTIVITY_VALUE;
};

class ClientsFilesMenager{

public:
    // Constructor
    ClientsFilesMenager(App*);

    // Destructor
    ~ClientsFilesMenager();

private:
    // Parent
    App* parent = nullptr;

    // Actual TcpSocket
    MyTcpSocket* actualSocket = nullptr;

    // Fast Access Table
    UserFastAccess* userFastAccess = nullptr;
    UserLoggedFastAccess* loggedUsers = nullptr;
    unsigned int numbOfLoggedUsers = 0;
    unsigned int numbOfUserFastAccess = 0;
    bool allocUserFastAccess = false;

    unsigned long long filePos = 0;

    bool fileOperation = false;

    bool createClientsFileBackUp();
    void clearMemory();
    bool readNextClient(User&, QFile&);
    bool writeNextClient(User&, QFile&);
    UserParameters checkUserParameters(QString&);
    bool init();
public:
    bool createClientsFile();
    uint8_t restoreClientsFile();
    void addEditRemoveClient(MyTcpSocket* newActualSocket);
    void addEditRemoveClient(MyTcpSocket* newActualSocket, User& user);
    void readClients(MyTcpSocket* newActualSocket);
    void readClients(MyTcpSocket* newActualSocket, ReadFileRules& rfr);
    void loginClient(MyTcpSocket* newActualSocket);
    void logoutClient(MyTcpSocket* newActualSocket);
    void extendActivity(MyTcpSocket* newActualSocket);
    void insertFastLoggedClient(unsigned long long userId, unsigned long long filePos);
    void removeFastLoggedClient(unsigned long long userId);
    void checkOrReduceActivity();
    unsigned long long getActualFilePos();
    bool isFileOperation();

    // ----------
private:
    bool readClientsFile(ReadFileRules& rules);
    bool writeClientsFile();
    bool writeClientsFile(User& requestUser);

    void reallocFastClients(UserFastAccess*, unsigned int);  // Change table
    void insertFastClient(unsigned int index, unsigned long long userId, unsigned long long filePos);

    unsigned long long getFilePos(unsigned long long id);

};


#endif // CLIENTSFILESMENAGER_HPP
