#include"user.hpp"

User::User(QString userName)
    : userName(userName)
{

}

User::~User(){

}

void User::createUser(){

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
    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}
