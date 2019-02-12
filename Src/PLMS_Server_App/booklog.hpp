#ifndef BOOKLOG_HPP
#define BOOKLOG_HPP

// ------------------ Macros --------------------------------------------
#define BOOK_LOG_JSON_KEY_TEXT ("bookLog")
#define WRITE_PARAM_TO_FILE(bookO, paramN) (bookO.getStrForFile(paramN).toUtf8())
#define DATE_TIME_FORMAT ("dd.MM.yyyy hh:mm:ss")
#define OPTIONAL_DATE_TIME_FORMAT ("dd.MM.yyyy")

// Memory Pointer Macros ------------------------------------------------------------------------------
#define SET_PTR_DO(ptr, newPtr) {if(ptr) {delete ptr; ptr = nullptr;} ptr = newPtr;}
#define SET_PTR_DOA(ptrA, newPtrA) {if(ptrA) {delete []ptrA; ptrA = nullptr;}; ptrA = newPtrA;}
#define SET_PTR_NDO(ptr, newPtr) {ptr = newPtr;}
// Include macros
//#include "mainmacros.hpp"

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"booklogparameters.hpp"
#include"userpermissions.hpp"
#include"booklogaction.hpp"
#include<QDateTime>

// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class QJsonObject;
class QJsonArray;
// ----------------------------------------------------------------------

// ------------------ Book Class ------------------------------------

class BookLog{
public:
    // Constructor
    BookLog();
    BookLog(QJsonObject);

    // Destructor
    ~BookLog();

private:
    // Book Data (InAccessible)
    QString pesel;   // if 0, unidentified  *
    QString firstName;
    QString surname;
    UserPermissions permissions = NUMB_OF_USER_PERMISSIONS;
    BookLogAction action = NUMB_OF_BOOK_LOG_ACTIONS;
    QDateTime dateTime = QDateTime::currentDateTime();

    // Optional
    unsigned long long bookId = 0;
    QDateTime dateTimeOptional;
    uint userIDComment = 0;
    QString commentContent;

    bool jsonReadCommentsError = false;

public:
    bool endOfBookLogFile = false;
    // Check book have all required data (function for file)
    bool checkBookLogFromFile();

    // Get Functions

    // Set Param by Name with QString and Enumerator
    void setParam(BookLogParameters bookLogParam, QString value);

    // Get string Value  of Param by Name
    QString getParam(BookLogParameters bookLogParam);

    // Get String for File
    QString getStrForFile(BookLogParameters bookParam);

    // Check Book parameters (dont check bookId)
    bool checkBookLogParameters();

    bool readJson(QJsonObject&);
    void writeJson(QJsonObject&);

    unsigned long long getFileDataStrLength();
    unsigned long long getUserId();
    QDateTime& getDateTimeOptional();
    unsigned long long getBookId();
    BookLogAction getAction();
    UserPermissions getUserPermissions();
    void addBookId(unsigned long long);

};


#endif // BOOKLOG_HPP
