#include"user.hpp"
#include<QJsonObject>
User::User(){

}

User::User(QJsonObject&)
{

}

User::~User(){

}


QString User::getUserName(){
    return userName;
}

void User::setParam(UserParameters userParam, QString value){
    switch (userParam) {
    case USER_ID:
        userId = value.toULongLong();
        break;
    case USER_NAME:
        userName = value;
        break;
    case USER_PASSWORD:
        userPassword = value;
        break;
    case USER_FIRST_NAME:
        userFirstName = value;
        break;
    case USER_SECOND_NAME:
        userSecondName = value;
        break;
    case USER_SURNAME:
        userSurname = value;
        break;


    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}

bool User::checkUser(){
    // User Id
    if(userId == 0)
        return false;
    // User Name
    if(userName.isEmpty())
        return false;
    // User Password
    if(userPassword.isEmpty())
        return false;
    // User First Name
    if(userFirstName.isEmpty())
        return false;
    // User Surname
    if(userSurname.isEmpty())
        return false;


    // _PH_ DONT DELETE THIS PH (Check All Params)
}
