#include"booksfilesmenager.hpp"
#include"app.hpp"
#include"readfilerules.hpp"
#include<QFile>
#include"book.hpp"
#include<QDebug>
#include"filereadingstatenum.hpp"
#include<QJsonObject>
#include"filetypeenum.hpp"
#include"mytcpsocket.hpp"
#include<QTextStream>

BooksFilesMenager::BooksFilesMenager(App* parent)
    : parent(parent)
{}

BooksFilesMenager::~BooksFilesMenager(){
    clearMemory();
}

bool BooksFilesMenager::createBooksFile(){
    QFile file(BOOKS_FILE_NAME);
    if(file.exists())
        file.remove();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        SERVER_MSG(BOOKS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << (QString(BOOK_PARAMETERS_BOOK_ID) + QString("=0\nend=\n")).toUtf8();
        file.close();
    }
    SERVER_MSG(QString("Books File Created"));
    return true;
}

bool BooksFilesMenager::createBooksFileBackUp(){
  if(QFile(BOOKS_FILE_BACKUP_NAME).exists())
        QFile(BOOKS_FILE_BACKUP_NAME).remove();
  if(!QFile::copy(BOOKS_FILE_NAME, BOOKS_FILE_BACKUP_NAME))
  {
      SERVER_MSG(BOOKS_FILE_OPEN_ERROR_TEXT);
      return false;
  }
  SERVER_MSG(QString("Books File BackUp Created"));
   return true;
}

uint8_t BooksFilesMenager::restoreBooksFile(){
    QFile booksFile(BOOKS_FILE_NAME);
    booksFile.remove();
    booksFile.setFileName(BOOKS_FILE_BACKUP_NAME);
    if(!booksFile.exists())
        return 1;   // No BackUp
    uint counter = 0;
    // Próba skopiowania backupu i zmiany nazwy
    while(!booksFile.copy(BOOKS_FILE_NAME)){
        if(counter < 100)
          counter++;
        else{
            SERVER_MSG(BOOKS_FILE_OPEN_ERROR_TEXT);
            return 2;   // Open File Error
          }
        }
    return 0;
}


void BooksFilesMenager::clearMemory(){

}

BookParameters BooksFilesMenager::checkBookParameters(QString &checkStr){
    CHECK_PARAM_INIT;
    switch(checkStr.length()){
    case 3:
    {
        // Check END_TOKEN
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_END_PARAMETER_TOKEN, 3, BOOK_END_PARAMETER_TOKEN);
        // Check ...

        // End of Check for 3 Signs
    }
    break;
    case 4:
    {
        // Check BOOK_USER_ID
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_USER_ID, 4, BOOK_USER_ID);
        // Check ...

        // End of Check for 4 Signs
    }
    break;
    case 5:
    {
        // Check BOOK_TITLE
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_TITLE, 5, BOOK_TITLE);
        // Check ...

        // End Of Check for 5 Signs
    }
    break;
    case 6:
    {
        // Check BOOK_ID
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_ID, 6, BOOK_ID);

        // Check BOOK_STATUS
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_STATUS, 6, BOOK_STATUS);

        // Check BOOK_COMMENTS_USER_ID
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID, 6, BOOK_COMMENTS_USER_ID);
        // Check ...

        // End Of Check for 6 Signs
    }
    break;
    case 7:
    {
        // Check BOOK_EDITION
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_EDITION, 7, BOOK_EDITION);
        // Check ...

        // End Of Check for 7 Signs
    }
    break;
    case 8:
    {
        // Check BOOK_COMMENTS
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_COMMENTS, 8, BOOK_COMMENTS);

        // Check ...

        // End Of Check for 8 Signs
    }
    break;
    case 9:
    {
        // Check BOOK_PUBLISHER
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_PUBLISHER, 9, BOOK_PUBLISHER);
        // Check ...

        // End Of Check for 9 Signs
    }
    break;
    case 10:
    {
        // Check BOOK_COMMENTS_CONTENT
        CHECK_PARAM_RETURN_V(checkStr, BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT, 10, BOOK_COMMENTS_CONTENT);
        // Check ...

        // End Of Check for 10 Signs
    }
    break;
    default:
    break;
    }
    return BOOK_NUMB_OF_PARAMETERS; // Parameter not Found
}

bool BooksFilesMenager::readBooksFile(ReadFileRules& rules){
    QFile file(BOOKS_FILE_NAME);
    if(!file.exists()){
        switch(restoreBooksFile()){
        case 1: // No BackUp
            if(!createBooksFile()) // Creating Error
                return false;
            break;
        case 2: // Open File Error
            return false;
        default:    // Restored File
            break;
        }
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(BOOKS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        // File Reading Depends of Reading Rules
        SERVER_MSG("--- Book File Reading Start ---");
        Book tempBook;
        do{
            if(!readNextBook(tempBook, file))
            {
                file.close();
                SERVER_MSG("--- Book File Read Failed ---");
                return false; // READING ERROR
            }
            if(!tempBook.checkBookFromFile()){
                // Fail of book check (Depends of command type)
                file.close();
                SERVER_MSG("--- Book File Read Failed ---");
                return false;
            }
        }while(!rules.check(tempBook, actualSocket));
        file.close();
        SERVER_MSG("--- Book File Read Success");
    }
}

bool BooksFilesMenager::readNextBook(Book &tempBook, QFile &file){
    QString tempChar;
    QString tempStr;
    FileReadingStat frs = FILE_READING_SEARCHING;
    BookParameters bookParameter = BOOK_NUMB_OF_PARAMETERS;
    tempBook = Book();
    // Find next book
    bool stop = false;
    while(!stop){
        if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
            SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
            SERVER_MSG("Books file end while reading books parameters");
            /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
            return false;
        }
        App::readCharUtf8(file, tempChar);
        // Operation on readed character
        switch(frs){
            case FILE_READING_SEARCHING:
            {
            if(!(tempChar == '\n' || tempChar == '\r' || tempChar == ' ' || tempChar == '"')){
                frs = FILE_READING_READING_PARAM_NAME;
                tempStr.append(tempChar);
              }
            }
            break;
            case FILE_READING_READING_PARAM_NAME:
            {   // Read until equal sign or (file corrupted , internal server error, SERVER PAUSE WORK)
                if(tempChar == '\n' || tempChar == '\r' || tempChar == ' '){
                    SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                    SERVER_MSG("Books file have corrupted parameter name");
                    /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                    return false;
                }
                if(tempChar == '='){
                    // Check parameter name
                    if((bookParameter = checkBookParameters(tempStr)) == BOOK_NUMB_OF_PARAMETERS){
                        SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                        SERVER_MSG("Books file have corrupted parameter name");
                        /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                        return false;
                    }
                    // ---------------------
                    frs = FILE_READING_READING_VALUE;
                    if(bookParameter == BOOK_END_PARAMETER_TOKEN){
                        return true;
                    }
                    tempStr.clear();
                }else{
                    tempStr.append(tempChar);
                }
            }
            break;
            case FILE_READING_READING_VALUE:
            {
                // Check if speech marks, if not read as number (End with EndLineSign and check only digits)
                    if(tempChar == '"'){
                        App::readCharUtf8(file, tempChar);
                        while(tempChar != '"'){
                        tempStr.append(tempChar);
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Books file end while reading books parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
                            App::readCharUtf8(file, tempChar);
                        }
                    }else{
                        do{
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Books file end while reading books parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
                            if((tempChar.at(0).toLatin1() < 48 || tempChar.at(0).toLatin1() > 57)){
                                if(tempChar != '\r'){
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Books file number parameter have no digit sign in value");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                                }
                            }else{
                                tempStr.append(tempChar);
                            }
                            App::readCharUtf8(file, tempChar);
                        }while(tempChar != '\n');
                    }
                    tempBook.setParam(bookParameter, tempStr);
                    tempStr.clear();
                    frs = FILE_READING_SEARCHING;
            }
            break;            
        }
    }
}



void BooksFilesMenager::addEditRemoveBook(MyTcpSocket* newActualSocket){
    actualSocket = newActualSocket;
    writeBooksFile();
}

void BooksFilesMenager::readBooks(MyTcpSocket* newActualSocket){
    actualSocket = newActualSocket;
    QJsonObject jsonObj = actualSocket->requestData.value(READ_FILE_RULES_JSON_KEY_TEXT).toObject();
    ReadFileRules rfr(jsonObj, parent);
    if(!rfr.isConstructingError())
        readBooksFile(rfr);
    else {
        newActualSocket->setReturnErrorType(RETURN_ERROR_JSON_BOOK_NOT_SENT); // _PH_ Change to Read File Rules JSON Corrupted
    }
}

bool BooksFilesMenager::writeBooksFile(){
    // Reading Rules for Write File
    ReadFileRules readFileRules(FILE_TYPE_BOOKS_FILE, parent);
    QFile file(BOOKS_FILE_NAME);
    if(!file.exists()){
        switch(restoreBooksFile()){
        case 1: // No BackUp
            if(!createBooksFile()) // Creating Error
                return false;
            break;
        case 2: // Open File Error
            return false;
        default:    // Restored File
            break;
        }
    }
    bool bookFound = false;
    QFile newFile(TEMP_FILE_NAME);
    newFile.remove();
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(BOOKS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        SERVER_MSG("--- Temp File Writing Start ---");
        if(!newFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            SERVER_MSG(TEMP_FILE_OPEN_ERROR_TEXT);
            file.close();
            SERVER_MSG("--- Book File Closed");
            return false;
        }else{
            Book tempBook;
            Book requestBook(actualSocket->requestData.value(BOOK_JSON_KEY_TEXT).toObject());
            unsigned long long lastId = 0;
            do{
                if(!readNextBook(tempBook, file))
                {
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Book File Read Failed ---");
                    return false; // READING ERROR
                }
                if(!tempBook.checkBookFromFile()){
                    // Fail of book check (Depends of command type)
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Book File Read Failed ---");
                    return false;
                }

                if(requestBook.getBookId() == 0){
                    // Check if readed from file book have the same  number as requestBook
                    if(requestBook.getParam(BOOK_TITLE) == tempBook.getParam(BOOK_TITLE) && requestBook.getParam(BOOK_PUBLISHER) == tempBook.getParam(BOOK_PUBLISHER) && requestBook.getParam(BOOK_EDITION) == tempBook.getParam(BOOK_EDITION)){
                        SERVER_MSG("There is a book with the same title, publisher and edition.");
                        file.close();
                        newFile.close();
                        SERVER_MSG("--- Book File Read Failed ---");
                        return false;
                    }
                    if(!bookFound){
                        if(lastId + 1 < tempBook.getBookId()){
                            requestBook.setBookId(lastId + 1);
                            writeNextBook(requestBook, newFile);
                            bookFound = true;
                        }else{
                            if(tempBook.getBookId() == 0){
                                requestBook.setBookId(lastId + 1);
                                writeNextBook(requestBook, newFile);
                                bookFound = true;
                            }
                        }
                    }
                    lastId = tempBook.getBookId();
                    writeNextBook(tempBook, newFile);
                }else{
                    switch(actualSocket->getCmdType()){
                    case COMMAND_TYPE_BOOK_EDIT:
                    if(!bookFound){
                        if(requestBook.getBookId() == tempBook.getBookId()){
                            writeNextBook(requestBook, newFile);
                            bookFound = true;    // For Information Purpose (Catch Not Found Book Error)
                        }else {
                            writeNextBook(tempBook, newFile);
                        }
                    }else{
                        file.close();
                        newFile.close();
                        SERVER_MSG("--- Book File Read Failed ---");
                        return false;
                    }
                        break;
                    case COMMAND_TYPE_BOOK_REMOVE:
                    if(!bookFound){
                        if(requestBook.getBookId() == tempBook.getBookId()){
                            requestBook.setParam(BOOK_ID, QString("0"));
                            bookFound = true;// For Information Purpose (Catch Not Found Book Error)
                        }else{
                            writeNextBook(tempBook, newFile);
                        }
                    }else{
                        file.close();
                        newFile.close();
                        SERVER_MSG("--- Book File Read Failed ---");
                        return false;
                    }
                        break;
                    default:
                        break;
                    }
                }
            }while(readFileRules.check(tempBook, actualSocket));
            newFile.close();
            if(!bookFound){
                actualSocket->setReturnErrorType(RETURN_ERROR_WRONG_COMMAND);   // _PH_ Change to BOOK_NOT_FOUND
                file.close();
                SERVER_MSG("--- Book File Read Failed ---");
                return true;
            }
        }
        file.close();
        if(!file.remove()){
            actualSocket->setReturnErrorType(RETURN_ERROR_WRONG_COMMAND);   // _PH_ Change to FILE_NOT_REMOVED
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        if(!newFile.rename(BOOKS_FILE_NAME)){
            actualSocket->setReturnErrorType(RETURN_ERROR_WRONG_COMMAND);   // _PH_ Change to FILE_NOT_REMOVED
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        if(!createBooksFileBackUp()){
            actualSocket->setReturnErrorType(RETURN_ERROR_WRONG_COMMAND);   // _PH_ Change to FILE_NOT_REMOVED
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        SERVER_MSG("--- Temp File Write Success");
    }
    return true;
}

bool BooksFilesMenager::writeNextBook(Book &book, QFile &file){
    QTextStream s(&file);
    if(book.getBookId() == 0){
        s << WRITE_PARAM_TO_FILE(book, BOOK_ID); //*
        // END (DONT MODIFY)
        s << WRITE_PARAM_TO_FILE(book, BOOK_END_PARAMETER_TOKEN);
        return  true;
    }
    s << WRITE_PARAM_TO_FILE(book, BOOK_ID); //*
    s << WRITE_PARAM_TO_FILE(book, BOOK_STATUS); //*
    s << WRITE_PARAM_TO_FILE(book, BOOK_TITLE); //*
    s << WRITE_PARAM_TO_FILE(book, BOOK_PUBLISHER); //*
    s << WRITE_PARAM_TO_FILE(book, BOOK_EDITION); //*
    if(!book.getParam(BOOK_USER_ID).isEmpty())
        s << WRITE_PARAM_TO_FILE(book, BOOK_USER_ID);
    s << book.prepareCommentsPack();

    // END (DONT MODIFY)
    s << WRITE_PARAM_TO_FILE(book, BOOK_END_PARAMETER_TOKEN);

    return true;
}

void BooksFilesMenager::reallocFastBooks(BookFastAccess* newArr, unsigned int size){
    SET_PTR_DOA(bookFastAccess, newArr);
    numbOfBookFastAccess = size;
}

void BooksFilesMenager::insertFastBook(unsigned int index, unsigned long long bookId, unsigned long long filePos){
    if(index < numbOfBookFastAccess){
        (*(bookFastAccess + index)).id = bookId;
        (*(bookFastAccess + index)).filePosition = filePos;
    }
    else {
        BookFastAccess* tempPtr = new BookFastAccess[index + 1];
        for(unsigned int i = 0; i < numbOfBookFastAccess; i++)
            (*(tempPtr + i)) = (*(bookFastAccess + i));
        (*(tempPtr + index)).id = bookId;
        (*(tempPtr + index)).filePosition = filePos;
        SET_PTR_DOA(bookFastAccess, tempPtr);
        numbOfBookFastAccess = index + 1;
    }
}

bool BooksFilesMenager::init(){
    ReadFileRules rfr(FILE_TYPE_BOOKS_FILE, parent);
    allocBookFastAccess = true;
    reallocFastBooks(nullptr, 0);
    return readBooksFile(rfr);
}
