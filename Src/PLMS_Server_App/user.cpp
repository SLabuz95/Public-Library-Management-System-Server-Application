#include"user.hpp"
#include<QJsonObject>
User::User(){

}

User::User(QJsonObject jsonObj)
{

}

User::~User(){

}


QString User::getUserName(){
    return userName;
}

unsigned long long User::getUserId(){
    return userId;
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
    case USER_PESEL:
        userPesel = value;
        break;


    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}

void User::setUserId(unsigned long long newUserId){
    userId = newUserId;
}

bool User::checkUser(){
    if(userId == 0)
        return true;
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

QString User::getParam(UserParameters userParam){
    switch(userParam){
    case USER_ID:
        return QString::number(userId);
    case USER_NAME:
        return userName;
    case USER_PESEL:
        return userPesel;
    case USER_SURNAME:
        return userSurname;
    case USER_PASSWORD:
        return userPassword;
    case USER_FIRST_NAME:
        return userFirstName;
    case USER_SECOND_NAME:
        return userSecondName;
    default:
        return QString();
    }
}

QString User::getStrForFile(UserParameters userParam){
    switch(userParam){
    case USER_ID:
        return QString(USER_PARAMETERS_USER_ID) + QString("=") + QString::number(userId) + QString("\n");
    case USER_NAME:
        return QString(USER_PARAMETERS_USER_NAME) + QString("=") + userName + QString("\n");
    case USER_PESEL:
        return QString(USER_PARAMETERS_USER_PESEL) + QString("=") + userPesel + QString("\n");
    case USER_SURNAME:
        return QString(USER_PARAMETERS_USER_SURNAME) + QString("=") + userSurname + QString("\n");
    case USER_PASSWORD:
        return QString(USER_PARAMETERS_USER_PASSWORD) + QString("=") + userPassword + QString("\n");
    case USER_FIRST_NAME:
        return QString(USER_PARAMETERS_USER_FIRST_NAME) + QString("=") + userFirstName + QString("\n");
    case USER_SECOND_NAME:
        return QString(USER_PARAMETERS_USER_SECOND_NAME) + QString("=") + userSecondName + QString("\n");
    case USER_END_PARAMETER_TOKEN:
        return QString(USER_PARAMETERS_USER_END_PARAMETER_TOKEN) + QString("=\n");
    default:
        return QString();
    }
}
