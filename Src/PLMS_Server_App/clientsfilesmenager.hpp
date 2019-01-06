#ifndef CLIENTSFILESMENAGER_HPP
#define CLIENTSFILESMENAGER_HPP
// ------------------ Macros --------------------------------------------
#define CLIENTS_FILE_NAME (QString("clients"))
#define CLIENTS_FILE_BACKUP_NAME (QString("clientsBU"))
#define CLIENTS_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku !!! -------------"))

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
// ----------------------------------------------------------------------

// ------------------ App Class -----------------------------------------

class ClientsFilesMenager{

public:
    // Constructor
    ClientsFilesMenager(App*);

    // Destructor
    ~ClientsFilesMenager();

private:

    // Parent
    App* parent = nullptr;

    bool createClientsFile();
    bool createClientsFileBackUp();
    uint8_t restoreClientsFile();
    void clearMemory();
    bool readNextClient(User&, QFile&);
    UserParameters checkUserParameters(QString&);

public:
    void addClient();
    void editClient();
    void removeClient();

private:
    bool readClientsFile(ReadFileRules& rules);
    void writeClientsFile();

};


#endif // CLIENTSFILESMENAGER_HPP
