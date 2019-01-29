#include"user.hpp"
#include<QJsonObject>
#include<QJsonArray>
#include"app.hpp"

User::User(){}

User::User(QJsonObject jsonObj)
{
 readJson(jsonObj);
}

User::~User(){
    SET_PTR_DOA(bookId, nullptr);
}

QString User::getUserName(){
    return userName;
}

unsigned long long User::getUserId(){
    return userId;
}

UserPermissions User::getUserPermissions(){
    return userPermissions;
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
    case USER_PERMISSIONS:
        userPermissions = static_cast<UserPermissions>(value.toUInt());
        break;
    case USER_BOOK_ID:
        addBookId(value.toULongLong());
        break;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}

void User::setUserId(unsigned long long newUserId){
    userId = newUserId;
}

bool User::checkUserFromFile(){
    if(userId == 0)
        return true;
    return checkUserParameters();
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
    case USER_PERMISSIONS:
        return QString::number(userPermissions);
    default:
        return QString();
    }
}

QString User::getStrForFile(UserParameters userParam){
    switch(userParam){
    case USER_ID:
        return QString(USER_PARAMETERS_USER_ID) + QString("=") + QString::number(userId) + QString("\n");
    case USER_NAME:
        return QString(USER_PARAMETERS_USER_NAME) + QString("=\"") + userName + QString("\"\n");
    case USER_PESEL:
        return QString(USER_PARAMETERS_USER_PESEL) + QString("=\"") + userPesel + QString("\"\n");
    case USER_SURNAME:
        return QString(USER_PARAMETERS_USER_SURNAME) + QString("=\"") + userSurname + QString("\"\n");
    case USER_PASSWORD:
        return QString(USER_PARAMETERS_USER_PASSWORD) + QString("=\"") + userPassword + QString("\"\n");
    case USER_FIRST_NAME:
        return QString(USER_PARAMETERS_USER_FIRST_NAME) + QString("=\"") + userFirstName + QString("\"\n");
    case USER_SECOND_NAME:
        return QString(USER_PARAMETERS_USER_SECOND_NAME) + QString("=\"") + userSecondName + QString("\"\n");
    case USER_END_PARAMETER_TOKEN:
        return QString(USER_PARAMETERS_USER_END_PARAMETER_TOKEN) + QString("=\n");
    case USER_BOOK_ID:
    {
        QString ret;
        for(ushort i = 0; i < numbOfBookId; i++)
            ret.append(QString(USER_PARAMETERS_USER_BOOK_ID) + QString("=") + QString::number(*(bookId + i)) + QString("\n"));
        return ret;
    }
    case USER_PERMISSIONS:
        return QString(USER_PARAMETERS_USER_PERMISSIONS) + QString("=") + QString::number(userPermissions) + QString("\n");
    default:
        return QString();
    }
}

bool User::checkUserParameters(){
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
    // User Pesel
    if(userPesel.isEmpty())
        return false;
    // User Permissions
    if(userPermissions == NUMB_OF_USER_PERMISSIONS)
        return false;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    return  true;
}


void User::readJson(QJsonObject& o){
    if(o.value(USER_PARAMETERS_USER_ID) != QJsonValue::Undefined)
        userId = o.value(USER_PARAMETERS_USER_ID).toString().toULongLong();
    if(o.value(USER_PARAMETERS_USER_NAME) != QJsonValue::Undefined)
        userName = o.value(USER_PARAMETERS_USER_NAME).toString();
    if(o.value(USER_PARAMETERS_USER_PASSWORD) != QJsonValue::Undefined)
        userPassword = o.value(USER_PARAMETERS_USER_PASSWORD).toString();
    if(o.value(USER_PARAMETERS_USER_PESEL) != QJsonValue::Undefined)
        userPesel = o.value(USER_PARAMETERS_USER_PESEL).toString();
    if(o.value(USER_PARAMETERS_USER_FIRST_NAME) != QJsonValue::Undefined)
        userFirstName = o.value(USER_PARAMETERS_USER_FIRST_NAME).toString();
    if(o.value(USER_PARAMETERS_USER_SECOND_NAME) != QJsonValue::Undefined)
        userSecondName = o.value(USER_PARAMETERS_USER_SECOND_NAME).toString();
    if(o.value(USER_PARAMETERS_USER_SURNAME) != QJsonValue::Undefined)
        userSurname = o.value(USER_PARAMETERS_USER_SURNAME).toString();
    if(o.value(USER_PARAMETERS_USER_BOOK_ID) == QJsonValue::Array){
        QJsonArray jA = o.value(USER_PARAMETERS_USER_BOOK_ID).toArray();
        numbOfBookId = jA.count();
        bookId = new unsigned long long[numbOfBookId];
        for(uint i = 0; i < numbOfBookId; i++)
            (*(bookId + i)) = jA.at(i).toString().toULongLong();
    }
    if(o.value(USER_PARAMETERS_USER_PERMISSIONS) != QJsonValue::Undefined)
        userPermissions = static_cast<UserPermissions>(o.value(USER_PARAMETERS_USER_PERMISSIONS).toInt());
}

unsigned long long User::getFileDataStrLength(){
    QString str = WRITE_PARAM_TO_FILE((*this), USER_ID) +
             WRITE_PARAM_TO_FILE((*this), USER_NAME) +
            WRITE_PARAM_TO_FILE((*this), USER_PASSWORD) +
            WRITE_PARAM_TO_FILE((*this), USER_PESEL) +
            WRITE_PARAM_TO_FILE((*this), USER_FIRST_NAME) +
            ((!getParam(USER_SECOND_NAME).isEmpty())? WRITE_PARAM_TO_FILE((*this), USER_SECOND_NAME) : "") +
            WRITE_PARAM_TO_FILE((*this), USER_SURNAME) +
            WRITE_PARAM_TO_FILE((*this), USER_BOOK_ID) +
            WRITE_PARAM_TO_FILE((*this), USER_PERMISSIONS) +
            WRITE_PARAM_TO_FILE((*this), USER_END_PARAMETER_TOKEN);     // _PH_ Add additional data if add new one
    return App::strLenForFile(str);    
}

void User::addBookId(unsigned long long newBook){
    unsigned long long* temp = new unsigned long long[numbOfBookId+1];
    for(ushort i = 0; i < numbOfBookId; i++)
        (*(temp + i)) = (*(bookId + i));
    (*(temp + numbOfBookId)) = newBook;
    SET_PTR_DOA(bookId, temp);
    numbOfBookId++;
}

void User::removeBookId(unsigned long long removeBook){
    ushort remove = 0;
    if(numbOfBookId > 1){
        unsigned long long* temp = new unsigned long long[numbOfBookId - 1];
        for(ushort i = 0; i < numbOfBookId; i++){
            if((*(bookId + i)) != removeBook)
                (*(temp + i - remove)) = (*(bookId + i));
            else
                remove++;
        }
        SET_PTR_DOA(bookId, temp);
    }else{
        if(*bookId == removeBook){
            SET_PTR_DOA(bookId, nullptr);
            remove++;
        }
    }
    numbOfBookId -= remove;
}

void User::merge(User &user){
    if(!user.getParam(USER_NAME).isEmpty())
        setParam(USER_NAME, user.getParam(USER_NAME));
    if(!user.getParam(USER_PASSWORD).isEmpty())
        setParam(USER_PASSWORD, user.getParam(USER_PASSWORD));
    if(!user.getParam(USER_FIRST_NAME).isEmpty())
        setParam(USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
    if(!user.getParam(USER_SECOND_NAME).isEmpty()){
        if(user.getParam(USER_SECOND_NAME) == "\"D\"")
            setParam(USER_SECOND_NAME, QString());
        else
            setParam(USER_SECOND_NAME, user.getParam(USER_SECOND_NAME));
    }
    if(!user.getParam(USER_SURNAME).isEmpty())
        setParam(USER_SURNAME, user.getParam(USER_SURNAME));
    if(!user.getParam(USER_PESEL).isEmpty())
        setParam(USER_PESEL, user.getParam(USER_PESEL));
    if(!user.getParam(USER_PERMISSIONS).isEmpty())
        setParam(USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
    mergeBookId(user);
}

void User::mergeBookId(User &user){
    ushort userNoBI = user.getNumbOfBookId();
    for(uint i = 0; i < userNoBI; i++){
        uint j = 0;
        for(j = 0; j < numbOfBookId; j++)
            if((*(user.getBookId() + i)) == (*(bookId + j))){
                removeBookId((*(bookId + j)));
                j--;
                break;
            }
        if(j == numbOfBookId)
        addBookId((*(user.getBookId() + i)));
    }
}

unsigned long long* User::getBookId(){
    return bookId;
}

ushort User::getNumbOfBookId(){
    return numbOfBookId;
}

void User::writeJson(QJsonObject &o){
    if(userId != 0)
        o.insert(USER_PARAMETERS_USER_ID, QString::number(userId));
    if(!userName.isEmpty())
        o.insert(USER_PARAMETERS_USER_NAME, userName);
    if(!userPesel.isEmpty())
        o.insert(USER_PARAMETERS_USER_PESEL, userPesel);
    if(!userSurname.isEmpty())
        o.insert(USER_PARAMETERS_USER_SURNAME, userSurname);
    if(!userPassword.isEmpty())
        o.insert(USER_PARAMETERS_USER_PASSWORD, userPassword);
    if(!userFirstName.isEmpty())
        o.insert(USER_PARAMETERS_USER_FIRST_NAME, userFirstName);
    if(!userSecondName.isEmpty())
        o.insert(USER_PARAMETERS_USER_SECOND_NAME, userSecondName);
    QJsonArray jA;
    for(ushort i = 0; i < numbOfBookId; i++)
        jA.append(QString::number((*(bookId + i))));
    o.insert(USER_PARAMETERS_USER_BOOK_ID, jA);
    if(userPermissions != NUMB_OF_USER_PERMISSIONS)
        o.insert(USER_PARAMETERS_USER_PERMISSIONS, userPermissions);
}
