#ifndef READFILERULES_HPP
#define READFILERULES_HPP
// ------------------ Macros --------------------------------------------
#define READ_FILE_RULES_JSON_KEY_TEXT ("readRules")
#define READ_FILE_RULES_FILTER_TEXT ("filter")
#define READ_FILE_RULES_FILE_TYPE_TEXT ("fileType")
#define READ_FILE_RULES_START_ID_TEXT ("startId")
#define READ_FILE_RULES_MAX_READ_TEXT ("maxRead")
#define READ_FILE_RULES_FILTER_PARAM_TEXT ("param")
#define READ_FILE_RULES_FILTER_VALUE_TEXT ("value")
// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"userparametersenum.hpp"
#include"filetypeenum.hpp"
#include"bookparameters.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class User;
class QJsonObject;
class QFile;
class MyTcpSocket;
// ----------------------------------------------------------------------

// Clients Filter Struct Definition
struct ClientsFileFilter
{
    UserParameters param;
    QString filterStr;
};

// Books Filter Struct Definition
struct BooksFileFilter
{
    BookParameters param;
    QString filterStr;
};

// ------------------ Read File Rules Class -----------------------------------------

class ReadFileRules{
public:
    // Constructor
    ReadFileRules(FileType fileType, App* parent);
    ReadFileRules(QJsonObject& jsonObject, App* parent);
    ReadFileRules(QJsonObject& jsonObject, FileType fileType, App* parent);

    // Destructor
   ~ReadFileRules();

private:
    // Parent
    App* parent = nullptr;

    FileType fileType = NUMB_OF_FILE_TYPES;

    union FileTypeFilter{
        ClientsFileFilter *clientsFileFilter;
        BooksFileFilter *booksFileFilter;
    } fileTypeFilter;

    uint numbOfFilters = 0;
    unsigned long long startIdPoint = 0;

    bool maxDecrementing = false;
    uint maxRead = 1;

    bool constructingError = false;

    void readJson(QJsonObject&);
public:
    //  Is Rule Finished?
    bool check(User&);
    bool check(User&, MyTcpSocket*);

    bool checkFilters(User&);

    // Get Functions
    App* getParent();
    bool isConstructingError();

    // Rules and File Initialization
    bool initialize(QFile&);

};


#endif // READFILERULES_HPP
