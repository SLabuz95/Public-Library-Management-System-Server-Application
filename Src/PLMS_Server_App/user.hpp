#ifndef USER_HPP
#define USER_HPP
// ------------------ Macros --------------------------------------------
#define USER_JSON_KEY_TEXT ("user")
#define WRITE_PARAM_TO_FILE(userO, paramN) (userO.getStrForFile(paramN).toUtf8())

// Memory Pointer Macros ------------------------------------------------------------------------------
#define SET_PTR_DO(ptr, newPtr) {if(ptr) {delete ptr; ptr = nullptr;} ptr = newPtr;}
#define SET_PTR_DOA(ptrA, newPtrA) {if(ptrA) {delete []ptrA; ptrA = nullptr;}; ptrA = newPtrA;}
#define SET_PTR_NDO(ptr, newPtr) {ptr = newPtr;}
// Include macros
//#include "mainmacros.hpp"
// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"userparametersenum.hpp"
#include"userpermissions.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class QJsonObject;
class QJsonArray;
// ----------------------------------------------------------------------

// ------------------ User Class ------------------------------------

class User{
public:
    // Constructor
    User();
    User(QJsonObject);

    // Destructor
    ~User();

private:
    // User Data (InAccessible)
    unsigned long long userId = 0;   // if 0, unidentified  *
    // User Name
    QString userName;   //*
    // User Password
    QString userPassword;   //*
    // User First Name
    QString userFirstName;  //*
    // User Second Name
    QString userSecondName; // Optional
    // User Surname
    QString userSurname;    //*
    // User Pesel
    QString userPesel;      //*
    // User Book Id's
    unsigned long long* bookId = nullptr;
    ushort numbOfBookId = 0;
    // User Permissions
    UserPermissions userPermissions = NUMB_OF_USER_PERMISSIONS;

public:

    // Check User have all required data (function for file)
    bool checkUserFromFile();

    // Get Functions
    QString getUserName();
    unsigned long long getUserId();
    unsigned long long* getBookId();
    ushort getNumbOfBookId();
    UserPermissions getUserPermissions();

    void addBookId(unsigned long long newBook);
    void removeBookId(unsigned long long removeBook);

    // Set User Id
    void setUserId(unsigned long long newUserId);

    // Set Param by Name with QString and Enumerator
    void setParam(UserParameters userParam, QString value);

    // Get string Value  of Param by Name
    QString getParam(UserParameters userParam);

    // Get String for File
    QString getStrForFile(UserParameters userParam);

    // Check user parameters (dont check clientId)
    bool checkUserParameters();

    void readJson(QJsonObject&);
    void writeJson(QJsonObject&);

    unsigned long long getFileDataStrLength();

    void merge(User&);
    void mergeBookId(User&);
};


#endif // USER_HPP
