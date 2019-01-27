#include"book.hpp"
#include<QJsonObject>
#include<QJsonArray>
#include"bookcomment.hpp"
#include"app.hpp"
Book::Book(){

}

Book::Book(QJsonObject jsonObj)
{
 readJson(jsonObj);
}

Book::~Book(){

}

unsigned long long Book::getBookId(){
    return bookId;
}

BookStatus Book::getBookStatus(){
    return bookStatus;
}

BookComment* Book::getBookComments(){
    return bookComments;
}

uint Book::getNumbOfBookComments(){
    return numbOfBookComments;
}

void Book::setParam(BookParameters bookParam, QString value){
    switch (bookParam) {
    case BOOK_ID:
        bookId = value.toULongLong();
        break;
    case BOOK_STATUS:
        bookStatus = static_cast<BookStatus>(value.toUInt());
        break;
    case BOOK_TITLE:
        bookTitle = value;
        break;
    case BOOK_PUBLISHER:
        bookPublisher = value;
        break;
    case BOOK_EDITION:
        bookEdition = value;
        break;
    case BOOK_USER_ID:
        userId = value.toULongLong();
        break;
    case BOOK_COMMENTS_CONTENT:
        // Add New Comment
        addComment(value);
        break;
    case BOOK_COMMENTS_USER_ID:
        // Add UserID to last comment
        if(numbOfBookComments > 0)
            (*(bookComments + numbOfBookComments - 1)).userId = value.toUInt();
        break;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    default:
        break;
    }
}

void Book::setBookId(unsigned long long newBookId){
    bookId = newBookId;
}

bool Book::checkBookFromFile(){
    if(bookId == 0)
        return true;
    return checkBookParameters();
}

QString Book::getParam(BookParameters bookParam){
    switch(bookParam){
    case BOOK_ID:
        return QString::number(bookId);
    case BOOK_STATUS:
        return QString::number(bookStatus);
    case BOOK_TITLE:
        return bookTitle;
    case BOOK_PUBLISHER:
        return bookPublisher;
    case BOOK_EDITION:
        return bookEdition;
    case BOOK_USER_ID:
        if(userId == 0)
            return QString();
        else {
            return QString::number(userId);
        }
    case BOOK_COMMENTS:
        return prepareCommentsPack();   // _PH_ TO CHECK
    default:
        return QString();
    }
}

QString Book::getStrForFile(BookParameters bookParam){
    switch(bookParam){
    case BOOK_ID:
        return QString(BOOK_PARAMETERS_BOOK_ID) + QString("=") + QString::number(bookId) + QString("\n");
    case BOOK_STATUS:
        return QString(BOOK_PARAMETERS_BOOK_STATUS) + QString("=") + QString::number(bookStatus) + QString("\n");
    case BOOK_TITLE:
        return QString(BOOK_PARAMETERS_BOOK_TITLE) + QString("=\"") + bookTitle + QString("\"\n");
    case BOOK_PUBLISHER:
        return QString(BOOK_PARAMETERS_BOOK_PUBLISHER) + QString("=\"") + bookPublisher + QString("\"\n");
    case BOOK_EDITION:
        return QString(BOOK_PARAMETERS_BOOK_EDITION) + QString("=\"") + bookEdition + QString("\"\n");
    case BOOK_USER_ID:
        return QString(BOOK_PARAMETERS_BOOK_USER_ID) + QString("=") + QString::number(userId) + QString("\n");
    case BOOK_COMMENTS:
        return prepareCommentsPack();
    case BOOK_END_PARAMETER_TOKEN:
        return QString(BOOK_PARAMETERS_BOOK_END_PARAMETER_TOKEN) + QString("=\n");
    default:
        return QString();
    }
}

bool Book::checkBookParameters(){
    // Book Status
    if(bookStatus == NUMB_OF_BOOK_STATUS)
        return false;
    // Book Title
    if(bookTitle.isEmpty())
        return false;
    // Book Publisher
    if(bookPublisher.isEmpty())
        return false;
    // Book Edition
    if(bookEdition.isEmpty())
        return false;
    // Book Comments
    if(!checkBookComments())
        return false;
    // _PH_ DONT DELETE THIS PH (Check All Params)
    return  true;
}


QJsonObject& Book::readJson(QJsonObject& o){
    if(o.value(BOOK_PARAMETERS_BOOK_ID) != QJsonValue::Undefined)
        bookId = o.value(BOOK_PARAMETERS_BOOK_ID).toString().toULongLong();
    if(o.value(BOOK_PARAMETERS_BOOK_STATUS) != QJsonValue::Undefined)
        bookStatus = static_cast<BookStatus>(o.value(BOOK_PARAMETERS_BOOK_STATUS).toInt());
    if(o.value(BOOK_PARAMETERS_BOOK_TITLE) != QJsonValue::Undefined)
        bookTitle = o.value(BOOK_PARAMETERS_BOOK_TITLE).toString();
    if(o.value(BOOK_PARAMETERS_BOOK_PUBLISHER) != QJsonValue::Undefined)
        bookPublisher = o.value(BOOK_PARAMETERS_BOOK_PUBLISHER).toString();
    if(o.value(BOOK_PARAMETERS_BOOK_EDITION) != QJsonValue::Undefined)
        bookEdition = o.value(BOOK_PARAMETERS_BOOK_EDITION).toString();
    if(o.value(BOOK_PARAMETERS_BOOK_USER_ID) != QJsonValue::Undefined)
        userId = o.value(BOOK_PARAMETERS_BOOK_USER_ID).toString().toULongLong();
    if(o.value(BOOK_PARAMETERS_BOOK_COMMENTS) != QJsonValue::Undefined){
        QJsonArray jA = o.value(BOOK_PARAMETERS_BOOK_COMMENTS).toArray();
        SET_PTR_DOA(bookComments, readCommentsFromJson(jA));
    }
    return o;
}

unsigned long long Book::getFileDataStrLength(){
    QString str = WRITE_PARAM_TO_FILE((*this), BOOK_ID) +
             WRITE_PARAM_TO_FILE((*this), BOOK_STATUS) +
            WRITE_PARAM_TO_FILE((*this), BOOK_TITLE) +
            WRITE_PARAM_TO_FILE((*this), BOOK_PUBLISHER) +
            WRITE_PARAM_TO_FILE((*this), BOOK_EDITION) +
            ((!getParam(BOOK_USER_ID).isEmpty())? WRITE_PARAM_TO_FILE((*this), BOOK_USER_ID) : "") +
            prepareCommentsPack() +
            WRITE_PARAM_TO_FILE((*this), BOOK_END_PARAMETER_TOKEN);     // _PH_ Add additional data if add new one
    return App::strLenForFile(str);
}
