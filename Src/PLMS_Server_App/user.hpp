#ifndef USER_HPP
#define USER_HPP
// ------------------ Macros --------------------------------------------
#define USER_JSON_KEY_TEXT ("user")
#define WRITE_PARAM_TO_FILE(userO, paramN) userO.getStrForFile(paramN).toUtf8()

// Include macros
//#include "mainmacros.hpp"

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"userparametersenum.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class QJsonObject;
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


public:

    // Check User have all required data (function for file)
    bool checkUserFromFile();

    // Get Functions
    QString getUserName();
    unsigned long long getUserId();

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
};


#endif // USER_HPP
