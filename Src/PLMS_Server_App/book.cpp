#include"book.hpp"
#include<QJsonObject>
#include<QJsonArray>
#include"bookcomment.hpp"
#include"app.hpp"
Book::Book(){}

Book::Book(QJsonObject jsonObj)
{
 readJson(jsonObj);
}

Book::~Book(){
    SET_PTR_DOA(bookComments, nullptr);
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
    case BOOK_COMMENT_CONTENT:
        // Add UserID to last comment
        if(numbOfBookComments > 0)
            (*(bookComments + numbOfBookComments - 1)).content = value;
        break;
    case BOOK_COMMENT_USER_ID:
        addComment(value.toUInt());
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

bool Book::readCommentsFromJson(QJsonArray &jA){
    uint tempNumbOfBookComments = static_cast<uint>(jA.size());
    BookComment* tempBookComments = new BookComment[tempNumbOfBookComments];
    uint i = 0;
    for(i = 0; i < tempNumbOfBookComments; i++){
        QJsonValue jAValue = jA.at(i);
        if(jAValue == QJsonValue::Object){
            if(jAValue.toObject().value(BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID) != QJsonValue::Undefined){
                (*(tempBookComments + i)).userId = jAValue.toObject().value(BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID).toString().toULongLong();
                if((*(tempBookComments + i)).userId == 0)
                    break;
            }else{
                break;
            }
            if(jAValue.toObject().value(BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT) != QJsonValue::Undefined){
                (*(tempBookComments + i)).content = jAValue.toObject().value(BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT).toString();
                if((*(tempBookComments + i)).content.isEmpty())
                    break;
            }else{
                break;
            }
        }else{
            break;
        }
    }
    if(i < numbOfBookComments){
        SET_PTR_DOA(tempBookComments, nullptr);
        return false;
    }else{
        SET_PTR_DOA(bookComments, tempBookComments);
        numbOfBookComments = tempNumbOfBookComments;
        return true;
    }
}


bool Book::readJson(QJsonObject& o){
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
        if(!readCommentsFromJson(jA))
            return false;
    }
    return true;
}

void Book::addComment(unsigned long long userId){
    BookComment* temp = new BookComment[numbOfBookComments + 1];
    for(uint i = 0; i < numbOfBookComments; i++)
        (*(temp + i)) = (*(bookComments + i));
    (*(temp + numbOfBookComments)).userId = userId;
    SET_PTR_DOA(bookComments, temp);
    numbOfBookComments++;
}

void Book::addComment(unsigned long long userId, QString str){
    addComment(userId);
    (*(bookComments + numbOfBookComments - 1)).content = str;
}

void Book::removeComment(unsigned long long userId){
    ushort remove = 0;
    if(numbOfBookComments > 1){
        BookComment* temp = new BookComment[numbOfBookComments - 1];
        ushort remove = 0;
        for(uint i = 0; i < numbOfBookComments; i++){
            if((*(bookComments + i)).userId != userId)
                (*(temp + i - remove)) = (*(bookComments + i));
            else
                remove++;
        }
        SET_PTR_DOA(bookComments, temp);
    }else{
        if((*bookComments).userId == userId){
            SET_PTR_DOA(bookComments, nullptr);
            remove++;
        }
    }
    numbOfBookComments -= remove;
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

void Book::writeJson(QJsonObject& o){
    if(bookId != 0)
        o.insert(BOOK_PARAMETERS_BOOK_ID, getParam(BOOK_ID));
    if(bookStatus != NUMB_OF_BOOK_STATUS)
        o.insert(BOOK_PARAMETERS_BOOK_STATUS, getParam(BOOK_STATUS));
    if(!bookTitle.isEmpty())
        o.insert(BOOK_PARAMETERS_BOOK_TITLE, bookTitle);
    if(!bookPublisher.isEmpty())
        o.insert(BOOK_PARAMETERS_BOOK_PUBLISHER, bookPublisher);
    if(!bookEdition.isEmpty())
        o.insert(BOOK_PARAMETERS_BOOK_EDITION, bookEdition);
    if(userId != 0)
        o.insert(BOOK_PARAMETERS_BOOK_USER_ID, getParam(BOOK_USER_ID));
    if(checkBookComments()){
        QJsonArray jA;
        for(uint i = 0; i < numbOfBookComments; i++){
            QJsonObject jAO;
            jAO.insert(BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID, QString::number((*(bookComments + i)).userId));
            jAO.insert(BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT, (*(bookComments + i)).content);
            jA.append(jAO);
        }
        o.insert(BOOK_PARAMETERS_BOOK_COMMENTS, jA);
    }
}

QString Book::prepareCommentsPack(){
    QString str;
    for(uint i = 0; i < numbOfBookComments; i++){
        str.append(QString(BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID) + "=" + QString::number((*(bookComments + i)).userId) + QString("\n"));
        str.append(QString(BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT) + "=\"" + (*(bookComments + i)).content + QString("\"\n"));
    }
    return str;
}

bool Book::checkBookComments(){
    for(uint i = 0; i < numbOfBookComments; i++){
        if((*(bookComments + i)).userId == 0 || (*(bookComments + i)).content.isEmpty())
            return false;
    }
    return true;
}

void Book::merge(Book &book){
    if(!book.getParam(BOOK_TITLE).isEmpty())
        setParam(BOOK_TITLE, book.getParam(BOOK_TITLE));
    if(bookStatus != NUMB_OF_BOOK_STATUS)
        setParam(BOOK_STATUS, book.getParam(BOOK_STATUS));
    if(!book.getParam(BOOK_PUBLISHER).isEmpty())
        setParam(BOOK_PUBLISHER, book.getParam(BOOK_PUBLISHER));
    if(!book.getParam(BOOK_EDITION).isEmpty())
        setParam(BOOK_EDITION, book.getParam(BOOK_EDITION));
    if(book.getParam(BOOK_USER_ID).toULongLong() == userId)
        userId = 0;
    else
        userId = book.getParam(BOOK_USER_ID).toULongLong();
    mergeBookComments(book);
}

void Book::mergeBookComments(Book &book){
    ushort userNoBI = book.getNumbOfBookComments();
    for(uint i = 0; i < userNoBI; i++){
        uint j = 0;
        for(j = 0; j < numbOfBookComments; j++)
            if((*(book.getBookComments() + i)).userId == (*(bookComments + j)).userId){
                removeComment((*(bookComments + j)).userId);
                j--;
                break;
            }
        if(j == numbOfBookComments)
        addComment((*(book.getBookComments() + i)).userId, (*(book.getBookComments() + i)).content);
    }
}

