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

// ----------------------------------------------------------------------

// ------------------ User Class ------------------------------------

class User{
public:
    // Constructor
    User(QString);

    // Destructor
    ~User();

private:
    // User Data (InAccessible)
    unsigned long userId = 0;   // if 0, unidentified
    // User Name
    QString userName;
public:
    // Create New User Class
    void createUser();

    // Check User have all required data
    bool checkUser();

    // Get Functions
    QString getUserName();

    // Set Param by Name with QString and Enumerator
    void setParam(UserParameters userParam, QString value);
};


#endif // USER_HPP
