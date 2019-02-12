#include"booklog.hpp"
#include<QJsonObject>
#include<QJsonArray>
#include"bookcomment.hpp"
#include"app.hpp"
BookLog::BookLog(){}

BookLog::BookLog(QJsonObject jsonObj)
{
 readJson(jsonObj);
}

BookLog::~BookLog(){

}

void BookLog::setParam(BookLogParameters bookLogParam, QString value){
    switch (bookLogParam) {
    case BOOK_LOG_USER_PESEL:
        pesel = value;
        break;
    case BOOK_LOG_ACTION:
        action = static_cast<BookLogAction>(value.toUInt());
        break;
    case BOOK_LOG_USER_FIRST_NAME:
        firstName = value;
        break;
    case BOOK_LOG_USER_SURNAME:
        surname = value;
        break;
    case BOOK_LOG_USER_PERMISSIONS:
        permissions = static_cast<UserPermissions>(value.toUInt());
        break;
    case BOOK_LOG_ACTION_DATE_TIME:
        dateTime = QDateTime::fromString(value, DATE_TIME_FORMAT);
        break;
    case BOOK_LOG_BOOK_ID:
        bookId = value.toULongLong();
        break;
    case BOOK_LOG_COMMENT_CONTENT:
        commentContent = value;
        break;
    case BOOK_LOG_USER_ID_COMMENT:
        userIDComment = value.toULongLong();
        break;
    case BOOK_LOG_DATE_TIME_OPTIONAL:
        dateTimeOptional = QDateTime::fromString(value, OPTIONAL_DATE_TIME_FORMAT);
        break;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}


bool BookLog::checkBookLogFromFile(){
    if(endOfBookLogFile)
        return true;
    return checkBookLogParameters();
}

QString BookLog::getParam(BookLogParameters bookParam){
    switch(bookParam){
    case BOOK_LOG_ACTION:
        return QString::number(action);
    case BOOK_LOG_USER_PESEL:
        return pesel;
    case BOOK_LOG_USER_SURNAME:
        return surname;
    case BOOK_LOG_USER_FIRST_NAME:
        return firstName;
    case BOOK_LOG_USER_PERMISSIONS:
        return  QString::number(permissions);
    case BOOK_LOG_COMMENT_CONTENT:
        return commentContent;
    case BOOK_LOG_USER_ID_COMMENT:
        return  QString::number(userIDComment);
    case BOOK_LOG_ACTION_DATE_TIME:
        return dateTime.toString(DATE_TIME_FORMAT);
    case BOOK_LOG_DATE_TIME_OPTIONAL:
        return dateTimeOptional.toString(OPTIONAL_DATE_TIME_FORMAT);
    case BOOK_LOG_BOOK_ID:
        return QString::number(bookId);
    default:
        return QString();
    }
}

QString BookLog::getStrForFile(BookLogParameters bookParam){
    switch(bookParam){
    case BOOK_LOG_ACTION:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION) + QString("=") + QString::number(action) + QString("\n");
    case BOOK_LOG_USER_PESEL:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PESEL) + QString("=\"") + pesel + QString("\"\n");
    case BOOK_LOG_USER_SURNAME:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_SURNAME) + QString("=\"") + surname + QString("\"\n");
    case BOOK_LOG_USER_FIRST_NAME:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_FIRST_NAME) + QString("=\"") + firstName + QString("\"\n");
    case BOOK_LOG_USER_PERMISSIONS:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PERMISSIONS) + QString("=") + QString::number(permissions) + QString("\n");
    case BOOK_LOG_END_PARAMETER_TOKEN:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_END_TOKEN) + QString("=\n");
    case BOOK_LOG_BOOK_ID:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_BOOK_ID) + QString("=") + QString::number(bookId) + QString("\n");
    case BOOK_LOG_COMMENT_CONTENT:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_COMMENT_CONTENT) + QString("=\"") + commentContent + QString("\"\n");
    case BOOK_LOG_USER_ID_COMMENT:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_ID_COMMENT) + QString("=") + QString::number(userIDComment) + QString("\n");
    case BOOK_LOG_ACTION_DATE_TIME:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION_DATE_TIME) + QString("=\"") + dateTime.toString(DATE_TIME_FORMAT) + QString("\"\n");
    case BOOK_LOG_DATE_TIME_OPTIONAL:
        return QString(BOOK_LOG_PARAMETERS_BOOK_LOG_DATE_TIME_OPTIONAL) + QString("=\"") + dateTimeOptional.toString(OPTIONAL_DATE_TIME_FORMAT) + QString("\"\n");
    default:
        return QString();
    }
}

bool BookLog::checkBookLogParameters(){
    // BookLog Pesel
    if(pesel.isEmpty())
        return false;
    // BookLog First Name
    if(firstName.isEmpty())
        return false;
    // BookLog Surname
    if(surname.isEmpty())
        return false;
    // BookLog Permissions
    if(permissions == NUMB_OF_USER_PERMISSIONS)
        return false;
    // BookLog Action
    if(action == NUMB_OF_BOOK_LOG_ACTIONS)
        return false;
    if(dateTime.isNull())
        return false;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    return  true;
}

bool BookLog::readJson(QJsonObject& o){
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION) != QJsonValue::Undefined)
        action = static_cast<BookLogAction>(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION).toString().toUInt());
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PESEL) != QJsonValue::Undefined)
        pesel = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PESEL).toString();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_SURNAME) != QJsonValue::Undefined)
        surname = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_SURNAME).toString();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_FIRST_NAME) != QJsonValue::Undefined)
        firstName = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_FIRST_NAME).toString();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PERMISSIONS) != QJsonValue::Undefined)
        action = static_cast<BookLogAction>(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PERMISSIONS).toString().toUInt());
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_BOOK_ID) != QJsonValue::Undefined)
        bookId = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_BOOK_ID).toString().toULongLong();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_COMMENT_CONTENT) != QJsonValue::Undefined)
        commentContent = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_COMMENT_CONTENT).toString();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_ID_COMMENT) != QJsonValue::Undefined)
        userIDComment = o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_ID_COMMENT).toString().toULongLong();
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION_DATE_TIME) != QJsonValue::Undefined)
        dateTime = QDateTime::fromString(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION_DATE_TIME).toString(), DATE_TIME_FORMAT);
    if(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_DATE_TIME_OPTIONAL) != QJsonValue::Undefined)
        dateTimeOptional = QDateTime::fromString(o.value(BOOK_LOG_PARAMETERS_BOOK_LOG_DATE_TIME_OPTIONAL).toString(), OPTIONAL_DATE_TIME_FORMAT);
    return true;
}

unsigned long long BookLog::getFileDataStrLength(){
    QString str = WRITE_PARAM_TO_FILE((*this), BOOK_LOG_ACTION) +
             WRITE_PARAM_TO_FILE((*this), BOOK_LOG_USER_PESEL) +
            WRITE_PARAM_TO_FILE((*this), BOOK_LOG_USER_SURNAME) +
            WRITE_PARAM_TO_FILE((*this), BOOK_LOG_USER_FIRST_NAME) +
            WRITE_PARAM_TO_FILE((*this), BOOK_LOG_USER_PERMISSIONS) +
            WRITE_PARAM_TO_FILE((*this), BOOK_LOG_ACTION_DATE_TIME) +
            ((bookId != 0)? WRITE_PARAM_TO_FILE((*this), BOOK_LOG_BOOK_ID) : QString()) +
            ((dateTimeOptional.isNull())? QString() : WRITE_PARAM_TO_FILE((*this), BOOK_LOG_DATE_TIME_OPTIONAL)) +
            ((userIDComment != 0)? WRITE_PARAM_TO_FILE((*this), BOOK_LOG_USER_ID_COMMENT) + WRITE_PARAM_TO_FILE((*this), BOOK_LOG_COMMENT_CONTENT) : QString()) +
            WRITE_PARAM_TO_FILE((*this), BOOK_LOG_END_PARAMETER_TOKEN);     // _PH_ Add additional data if add new one
    return App::strLenForFile(str);
}

void BookLog::writeJson(QJsonObject& o){    
    if(!pesel.isEmpty())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PESEL, getParam(BOOK_LOG_USER_PESEL));
    if(action != NUMB_OF_BOOK_LOG_ACTIONS)
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION, getParam(BOOK_LOG_ACTION));
    if(!firstName.isEmpty())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_FIRST_NAME, firstName);
    if(!surname.isEmpty())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_SURNAME, surname);
    if(permissions != NUMB_OF_USER_PERMISSIONS)
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PERMISSIONS, getParam(BOOK_LOG_USER_PERMISSIONS));
    if(!dateTime.isNull())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION_DATE_TIME, getParam(BOOK_LOG_ACTION_DATE_TIME));
    if(bookId != 0)
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_BOOK_ID, getParam(BOOK_LOG_BOOK_ID));
    if(!dateTimeOptional.isNull())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_DATE_TIME_OPTIONAL, getParam(BOOK_LOG_DATE_TIME_OPTIONAL));
    if(userIDComment != 0)
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_USER_ID_COMMENT, getParam(BOOK_LOG_USER_ID_COMMENT));
    if(!commentContent.isEmpty())
        o.insert(BOOK_LOG_PARAMETERS_BOOK_LOG_COMMENT_CONTENT, getParam(BOOK_LOG_COMMENT_CONTENT));
}

unsigned long long BookLog::getBookId(){
    return bookId;
}

QDateTime& BookLog::getDateTimeOptional(){
    return dateTimeOptional;
}

unsigned long long BookLog::getUserId(){
    return userIDComment;
}

BookLogAction BookLog::getAction(){
    return action;
}

UserPermissions BookLog::getUserPermissions(){
    return permissions;
}
