#include"readfilerules.hpp"
#include<QJsonObject>
#include"user.hpp"
#include"mytcpsocket.hpp"
#include<QJsonArray>
#include"book.hpp"


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
    if(fileType == NUMB_OF_FILE_TYPES)
    {
        constructingError = true;
        return;
    }
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

ReadFileRules::~ReadFileRules(){
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
/*
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
*/
bool ReadFileRules::check(User &user, MyTcpSocket* actualSocket){
    if(user.getUserId() == 0)
        return false;
    if(maxDecrementing){
        if(startIdPoint <= user.getUserId()){
            if(maxRead > 0){
                if(numbOfFilters != 0){
                    if(checkFilters(user)){
                        maxRead--;
                        actualSocket->processReadedUserFromFile(user);
                    }
                }else{
                    maxRead--;
                    actualSocket->processReadedUserFromFile(user);
                }
            }else{
                if(nextPossibleReadId)
                    return false;
                else{
                    if(numbOfFilters != 0){
                        if(checkFilters(user)){
                            nextPossibleReadId = true;
                            actualSocket->addNextPossibleReadId(user.getUserId());
                            return false;
                        }
                    }else{
                        nextPossibleReadId = true;
                        actualSocket->addNextPossibleReadId(user.getUserId());
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool ReadFileRules::check(Book &book, MyTcpSocket* actualSocket){
    if(book.getBookId() == 0)
        return false;
    if(maxDecrementing){
        if(startIdPoint <= book.getBookId()){
            if(maxRead > 0){
                if(numbOfFilters != 0){
                    if(checkFilters(book)){
                        maxRead--;
                        actualSocket->processReadedBookFromFile(book);
                    }
                }else{
                    maxRead--;
                    actualSocket->processReadedBookFromFile(book);
                }
            }else{
                if(nextPossibleReadId)
                    return false;
                else{
                    if(numbOfFilters != 0){
                        if(checkFilters(book)){
                            nextPossibleReadId = true;
                            actualSocket->addNextPossibleReadId(book.getBookId());
                            return false;
                        }
                    }else{
                        nextPossibleReadId = true;
                        actualSocket->addNextPossibleReadId(book.getBookId());
                        return false;
                    }
                }
            }
        }
    }
    return true;
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

bool ReadFileRules::checkFilters(Book &book){
    QString tempStr;
    QString tempFilterStr;
    int filterStrLen = 0;
    for(uint i = 0; i < numbOfFilters; i++){
        tempStr = book.getParam((*(fileTypeFilter.booksFileFilter + i)).param);
        tempFilterStr = (*(fileTypeFilter.booksFileFilter + i)).filterStr;
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

unsigned long long ReadFileRules::getStartIdPoint(){
    return startIdPoint;
}

void ReadFileRules::readJson(QJsonObject& o){
    if(fileType == NUMB_OF_FILE_TYPES)
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
        if(!(maxRead > 0)){
            constructingError = true;
            return;
        }
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
                if(a.at(i) == QJsonValue::Object){
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
                if(a.at(i) == QJsonValue::Object){
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
