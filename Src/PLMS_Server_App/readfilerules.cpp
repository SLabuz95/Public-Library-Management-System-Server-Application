#include"readfilerules.hpp"
#include<QJsonObject>
#include"user.hpp"
#include"mytcpsocket.hpp"
#include<QJsonArray>


ReadFileRules::ReadFileRules(FileType fileType, App* parent = nullptr)
    : parent(parent), fileType(fileType)
{
    if(fileType == NUMB_OF_FILE_TYPES)
        constructingError = true;
}

ReadFileRules::ReadFileRules(QJsonObject& jsonObject, App* parent = nullptr)
    : parent(parent)
{
    readJson(jsonObject);
    if(constructingError)
        switch(fileType){
        case FILE_TYPE_BOOKS_FILE:
            delete []fileTypeFilter.booksFileFilter;
            break;
        case FILE_TYPE_CLIENTS_FILE:
            delete []fileTypeFilter.clientsFileFilter;
            break;
        default:
            break;
        }
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

bool ReadFileRules::isConstructingError(){
    return constructingError;
}

void ReadFileRules::readJson(QJsonObject& o){
    if(o.value(READ_FILE_RULES_FILE_TYPE_TEXT) != QJsonValue::Undefined){
        fileType = static_cast<FileType>(o.value(READ_FILE_RULES_FILE_TYPE_TEXT).toString().toUInt());
        if(fileType == NUMB_OF_FILE_TYPES){
            constructingError = true;
            return;
        }
    }
    if(o.value(READ_FILE_RULES_START_ID_TEXT) != QJsonValue::Undefined){
        startIdPoint = o.value(READ_FILE_RULES_START_ID_TEXT).toString().toULongLong();
    }
    if(o.value(READ_FILE_RULES_MAX_READ_TEXT) != QJsonValue::Undefined){
        maxRead = o.value(READ_FILE_RULES_MAX_READ_TEXT).toString().toUInt();
        maxDecrementing = true;
    }
    if(o.value(READ_FILE_RULES_FILTER_TEXT) != QJsonValue::Undefined){
        QJsonArray a = o.value(READ_FILE_RULES_FILTER_TEXT).toArray();
        if((numbOfFilters = a.count()) == 0)
            return;
        switch(fileType){
        case FILE_TYPE_BOOKS_FILE:
        {
            QJsonObject tempFilterObj;
            fileTypeFilter.booksFileFilter = new BooksFileFilter[numbOfFilters];
            for(int i = 0; i < numbOfFilters; i++){
                if(a.at(i) != QJsonValue::Undefined){
                    tempFilterObj = a.at(i).toObject();
                }else{
                    constructingError = true;
                    return;
                }
                if(tempFilterObj.value(READ_FILE_RULES_FILTER_PARAM_TEXT) != QJsonValue::Undefined){
                    (*(fileTypeFilter.booksFileFilter + i)).param = static_cast<BookParameters>(tempFilterObj.value(READ_FILE_RULES_FILTER_PARAM_TEXT).toString().toUInt());
                }else{
                    constructingError = true;
                    return;
                }
                if(tempFilterObj.value(READ_FILE_RULES_FILTER_VALUE_TEXT) != QJsonValue::Undefined){
                    (*(fileTypeFilter.booksFileFilter + i)).filterStr = tempFilterObj.value(READ_FILE_RULES_FILTER_VALUE_TEXT).toString();
                }else{
                    constructingError = true;
                    return;
                }
            }
        }
            break;
        case FILE_TYPE_CLIENTS_FILE:
        {
            QJsonObject tempFilterObj;
            fileTypeFilter.clientsFileFilter = new ClientsFileFilter[numbOfFilters];
            for(int i = 0; i < numbOfFilters; i++){
                if(a.at(i) != QJsonValue::Undefined){
                    tempFilterObj = a.at(i).toObject();
                }else{
                    constructingError = true;
                    return;
                }
                if(tempFilterObj.value(READ_FILE_RULES_FILTER_PARAM_TEXT) != QJsonValue::Undefined){
                    (*(fileTypeFilter.clientsFileFilter + i)).param = static_cast<UserParameters>(tempFilterObj.value(READ_FILE_RULES_FILTER_PARAM_TEXT).toString().toUInt());
                }else{
                    constructingError = true;
                    return;
                }
                if(tempFilterObj.value(READ_FILE_RULES_FILTER_VALUE_TEXT) != QJsonValue::Undefined){
                    (*(fileTypeFilter.clientsFileFilter + i)).filterStr = tempFilterObj.value(READ_FILE_RULES_FILTER_VALUE_TEXT).toString();
                }else{
                    constructingError = true;
                    return;
                }
            }
        }
            break;
        default:
            break;
        }
    }else{
        constructingError = true;
        return;
    }
}
