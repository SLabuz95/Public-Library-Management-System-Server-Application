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
#define READ_FILE_RULES_NEXT_POSSIBLE_READ_ID ("nextId")
#define READ_FILE_RULES_SKIP_NUMB ("skipNumb")
#define READ_FILE_RULES_FULL_FILTER ("fullCheck")
// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"userparametersenum.hpp"
#include"filetypeenum.hpp"
#include"bookparameters.hpp"
#include"booklogparameters.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class User;
class QJsonObject;
class QFile;
class MyTcpSocket;
class Book;
class BookLog;
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

// BookLogs Filter Struct Definition
struct BookLogsFileFilter
{
    BookLogParameters param;
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
        ClientsFileFilter *clientsFileFilter = nullptr;
        BooksFileFilter *booksFileFilter;
        BookLogsFileFilter * bookLogsFileFilter;
    } fileTypeFilter;

    uint numbOfFilters = 0;
    unsigned long long startIdPoint = 0;

    bool maxDecrementing = false;   // Write File Purpose if false
    uint maxRead = 1;
    unsigned long long  skipNumb = 0;
    bool fullFilter = false;

    bool nextPossibleReadId = false;

    bool constructingError = false;

    void readJson(QJsonObject&);
public:
    //  Is Rule Finished?
    bool check(User&, MyTcpSocket*);
    bool check(Book&, MyTcpSocket*);
    bool check(BookLog&, MyTcpSocket*);

    bool checkFilters(User&);
    bool checkFilters(Book&);
    bool checkFilters(BookLog&);

    // Get Functions
    App* getParent();
    bool isConstructingError();
    unsigned long long getStartIdPoint();
    unsigned long long getSkipNumber();

    // Rules and File Initialization
    bool initialize(QFile&);

};


#endif // READFILERULES_HPP
