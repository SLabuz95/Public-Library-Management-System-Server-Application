#ifndef READFILERULES_HPP
#define READFILERULES_HPP
// ------------------ Macros --------------------------------------------
#define READ_FILE_RULES_FILTER_TEXT ("filter")
#define READ_FILE_RULES_FILE_TYPE_TEXT ("fileType")
#define READ_FILE_RULES_START_ID_TEXT ("startId")
#define READ_FILE_RULES_MAX_READ_TEXT ("maxRead")
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

    unsigned long long startIdPoint = 0;

    uint maxRead = 1;

public:
    //  Is Rule Finished?
    bool check(User&);

    // Get Functions
    App* getParent();

    // Rules and File Initialization
    bool initialize(QFile&);

};


#endif // READFILERULES_HPP
