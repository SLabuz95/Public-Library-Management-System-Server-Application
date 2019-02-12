#include"readfilerules.hpp"
#include<QJsonObject>
#include"user.hpp"
#include"mytcpsocket.hpp"
#include<QJsonArray>
#include"book.hpp"
#include"booklog.hpp"


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
        case FILE_TYPE_BOOK_LOG_FILE:
            delete []fileTypeFilter.bookLogsFileFilter;
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
        case FILE_TYPE_BOOK_LOG_FILE:
            delete []fileTypeFilter.bookLogsFileFilter;
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
    case FILE_TYPE_BOOK_LOG_FILE:
        delete []fileTypeFilter.bookLogsFileFilter;
        break;
    default:
        break;
    }
}

bool ReadFileRules::check(User &user, MyTcpSocket* actualSocket){
    if(user.getUserId() == 0)
        return false;
    if(maxDecrementing){
        if(startIdPoint <= user.getUserId()){
            if(maxRead > 0){
                if(numbOfFilters != 0){
                    if(checkFilters(user)){
                        if(skipNumb > 0)
                            skipNumb--;
                        else{
                        maxRead--;
                        actualSocket->processReadedUserFromFile(user);
                        }
                    }
                }else{
                    if(skipNumb > 0)
                        skipNumb--;
                    else{
                    maxRead--;
                    actualSocket->processReadedUserFromFile(user);
                    }
                }
            }else{
                if(skipNumb > 0 || nextPossibleReadId)
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
                        if(skipNumb > 0)
                            skipNumb--;
                        else{
                        maxRead--;
                        actualSocket->processReadedBookFromFile(book);
                        }
                    }
                }else{
                    if(skipNumb > 0)
                        skipNumb--;
                    else{
                    maxRead--;
                    actualSocket->processReadedBookFromFile(book);
                    }
                }
            }else{
                if(skipNumb > 0 || nextPossibleReadId)
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

bool ReadFileRules::check(BookLog &bookLog, MyTcpSocket* actualSocket){
    if(bookLog.endOfBookLogFile)
        return false;
    if(maxDecrementing){
         if(maxRead > 0){
                if(numbOfFilters != 0){
                    if(checkFilters(bookLog)){
                        if(skipNumb > 0)
                            skipNumb--;
                        else{
                        maxRead--;
                        actualSocket->processReadedBookLogFromFile(bookLog);
                        }
                    }
                }else{
                    if(skipNumb > 0)
                        skipNumb--;
                    else{
                    maxRead--;
                    actualSocket->processReadedBookLogFromFile(bookLog);
                    }
                }
            }else{
                if(skipNumb > 0 || nextPossibleReadId)
                    return false;
                else{
                    if(numbOfFilters != 0){
                        if(checkFilters(bookLog)){
                            nextPossibleReadId = true;
                            actualSocket->addNextPossibleReadId(1);
                            return false;
                        }
                    }else{
                        nextPossibleReadId = true;
                        actualSocket->addNextPossibleReadId(1);
                        return false;
                    }
                }
            }
    }
    return true;
}


bool ReadFileRules::checkFilters(User &user){
    QString tempStr;
    QString tempFilterStr;
    UserParameters userParam;
    bool filterOk[USER_END_PARAMETER_TOKEN] = {0,0,0,0,0,0,0,0,0};
    bool nextParam = false;
    int filterStrLen = 0;
    for(uint i = 0; i < numbOfFilters; i++){
        userParam = (*(fileTypeFilter.clientsFileFilter + i)).param;
        tempStr = user.getParam(userParam);
        tempFilterStr = (*(fileTypeFilter.clientsFileFilter + i)).filterStr;
        filterStrLen = tempFilterStr.length();
        nextParam = false;
        for(uint j = i + 1; j < numbOfFilters; j++){
            if(userParam == (*(fileTypeFilter.clientsFileFilter + j)).param){
                nextParam = true;
                break;
            }
        }
        if(fullFilter || userParam < USER_NAME){
            if(tempStr != tempFilterStr){
                if(!nextParam && !filterOk[userParam])
                    return false;
            }else{
                filterOk[userParam] = true;
            }
        }else{
            if(filterStrLen > tempStr.length()){
                if(!nextParam && !filterOk[userParam])
                    return false;
            }else{
                uint j = 0;
                for(; j < filterStrLen; j++)
                    if(tempStr.at(j) != tempFilterStr.at(j))
                        break;
                if(j < filterStrLen){
                    if(!nextParam && !filterOk[userParam])
                        return false;
                }else{
                    filterOk[userParam] = true;
                }
            }
        }
    }
    return true;
}

bool ReadFileRules::checkFilters(Book &book){
    QString tempStr;
    QString tempFilterStr;
    BookParameters bookParam;
    bool filterOk[BOOK_END_PARAMETER_TOKEN] = {0,0,0,0,0,0,0,0,0,0,0,0};
    bool nextParam = false;
    int filterStrLen = 0;
    for(uint i = 0; i < numbOfFilters; i++){
        bookParam = (*(fileTypeFilter.booksFileFilter + i)).param;
        tempStr = book.getParam(bookParam);
        tempFilterStr = (*(fileTypeFilter.booksFileFilter + i)).filterStr;
        filterStrLen = tempFilterStr.length();
        nextParam = false;
        for(uint j = i + 1; j < numbOfFilters; j++){
            if(bookParam == (*(fileTypeFilter.booksFileFilter + j)).param){
                nextParam = true;
                break;
            }
        }
        if(fullFilter || (bookParam < BOOK_TITLE && bookParam != BOOK_ID)){
            if(tempStr != tempFilterStr){
                if(!nextParam && !filterOk[bookParam])
                    return false;
            }else{
                filterOk[bookParam] = true;
            }
        }else{
            if(filterStrLen > tempStr.length()){
                if(!nextParam && !filterOk[bookParam])
                    return false;
            }else{
                uint j = 0;
                for(; j < filterStrLen; j++)
                    if(tempStr.at(j) != tempFilterStr.at(j))
                        break;
                if(j < filterStrLen){
                    if(!nextParam && !filterOk[bookParam])
                        return false;
                }else{
                    filterOk[bookParam] = true;
                }
            }
        }
    }
    return true;
}

bool ReadFileRules::checkFilters(BookLog &bookLog){
    QString tempStr;
    QString tempFilterStr;
    BookLogParameters bookLogParam;
    bool filterOk[BOOK_LOG_END_PARAMETER_TOKEN] = {0,0,0,0,0,0,0,0,0,0};
    bool nextParam = false;
    int filterStrLen = 0;
    for(uint i = 0; i < numbOfFilters; i++){
        bookLogParam = (*(fileTypeFilter.bookLogsFileFilter + i)).param;
        tempStr = bookLog.getParam(bookLogParam);
        tempFilterStr = (*(fileTypeFilter.bookLogsFileFilter + i)).filterStr;
        filterStrLen = tempFilterStr.length();
        nextParam = false;
        for(uint j = i + 1; j < numbOfFilters; j++){
            if(bookLogParam == (*(fileTypeFilter.bookLogsFileFilter + j)).param){
                nextParam = true;
                break;
            }
        }
        if(fullFilter || bookLogParam == BOOK_LOG_ACTION || bookLogParam == BOOK_LOG_USER_PERMISSIONS){
            if(tempStr != tempFilterStr){
                if(!nextParam && !filterOk[bookLogParam])
                    return false;
            }else{
                filterOk[bookLogParam] = true;
            }
        }else{
            if(filterStrLen > tempStr.length()){
                if(!nextParam && !filterOk[bookLogParam])
                    return false;
            }else{
                uint j = 0;
                for(; j < filterStrLen; j++)
                    if(tempStr.at(j) != tempFilterStr.at(j))
                        break;
                if(j < filterStrLen){
                    if(!nextParam && !filterOk[bookLogParam])
                        return false;
                }else{
                    filterOk[bookLogParam] = true;
                }
            }
        }
    }
    return true;
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
    if(o.value(READ_FILE_RULES_SKIP_NUMB) != QJsonValue::Undefined)
        skipNumb = o.value(READ_FILE_RULES_SKIP_NUMB).toString().toULongLong();
    if(o.value(READ_FILE_RULES_FULL_FILTER) != QJsonValue::Undefined)
        fullFilter = o.value(READ_FILE_RULES_FULL_FILTER).toString().toUInt();
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
        numbOfFilters = a.count();
        if(numbOfFilters == 0)
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
