#include"readfilerules.hpp"
#include<QJsonObject>
#include"user.hpp"
#include"mytcpsocket.hpp"


ReadFileRules::ReadFileRules(FileType fileType, App* parent = nullptr)
    : parent(parent), fileType(fileType)
{}

ReadFileRules::ReadFileRules(QJsonObject& jsonObject, App* parent = nullptr)
    : parent(parent)
{

}

ReadFileRules::ReadFileRules(QJsonObject& jsonObject, FileType fileType, App* parent = nullptr)
    : parent(parent), fileType(fileType)
{

}

ReadFileRules::~ReadFileRules(){

}

bool ReadFileRules::check(User &user){
    if(maxDecrementing){
        if(numbOfFilters != 0){
            if(checkFilters(user))
                maxRead--;
        }
        else {
            maxRead--;
        }
        if(maxRead == 0)
            return false;
    }else{
        if(user.getUserId() == 0)
            return false;
        else {
            return true;
        }
    }
    return true;
}

bool ReadFileRules::check(User &user, MyTcpSocket* actualSocket){
    if(maxDecrementing){
        if(numbOfFilters != 0){
            if(checkFilters(user))
                maxRead--;
        }
        else {
            maxRead--;
        }
        if(maxRead == 0)
            return false;
        else{
            actualSocket->processReadedUserFromFile(user);
            return true;
        }
    }else{
        if(user.getUserId() == 0)
            return false;
        else{
            actualSocket->processReadedUserFromFile(user);
            return true;
        }
    }
}

bool ReadFileRules::checkFilters(User &user){
    QString tempStr;
    QString tempFilterStr;
    int filterStrLen = 0;
    for(uint i = 0; i < numbOfFilters; i++){
        tempStr = user.getParam((*(fileTypeFilter.clientsFileFilter + i)).param);
        tempFilterStr = (*(fileTypeFilter.clientsFileFilter + i)).filterStr;
        filterStrLen = tempFilterStr.length();
        if(filterStrLen > tempStr.length()){
            return false;
        }else{
            for(uint j = 0; j < filterStrLen; j++)
                if(tempStr.at(j) != tempFilterStr.at(j))
                    break;
        }
    }
}



App* ReadFileRules::getParent(){
    return parent;
}


