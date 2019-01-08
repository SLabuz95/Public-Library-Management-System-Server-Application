#ifndef USER_HPP
#define USER_HPP
// ------------------ Macros --------------------------------------------

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
    User(QJsonObject&);

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


public:

    // Check User have all required data
    bool checkUser();

    // Get Functions
    QString getUserName();

    // Set Param by Name with QString and Enumerator
    void setParam(UserParameters userParam, QString value);
};


#endif // USER_HPP
