#include"booklogsfilesmenager.hpp"
#include"app.hpp"
#include"readfilerules.hpp"
#include<QFile>
#include"booklog.hpp"
#include<QDebug>
#include"filereadingstatenum.hpp"
#include<QJsonObject>
#include<QJsonArray>
#include"filetypeenum.hpp"
#include"mytcpsocket.hpp"
#include<QTextStream>
#include<QTextCodec>

BookLogsFilesMenager::BookLogsFilesMenager(App* parent)
    : parent(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

BookLogsFilesMenager::~BookLogsFilesMenager(){
    clearMemory();
}

bool BookLogsFilesMenager::createBookLogsFile(){
    QFile file(BOOK_LOGS_FILE_NAME);
    if(file.exists())
        file.remove();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        SERVER_MSG(BOOK_LOGS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << (QString("end=\n")).toUtf8();
        file.close();
    }
    SERVER_MSG(QString("Book Logs File Created"));
    return true;
}

bool BookLogsFilesMenager::createBookLogsFileBackUp(){
  if(QFile(BOOK_LOGS_FILE_BACKUP_NAME).exists())
        QFile(BOOK_LOGS_FILE_BACKUP_NAME).remove();
  if(!QFile::copy(BOOK_LOGS_FILE_NAME, BOOK_LOGS_FILE_BACKUP_NAME))
  {
      SERVER_MSG(BOOK_LOGS_FILE_OPEN_ERROR_TEXT);
      return false;
  }
  SERVER_MSG(QString("Book Logs File BackUp Created"));
   return true;
}

uint8_t BookLogsFilesMenager::restoreBookLogsFile(){
    QFile bookLogsFile(BOOK_LOGS_FILE_NAME);
    bookLogsFile.remove();
    bookLogsFile.setFileName(BOOK_LOGS_FILE_BACKUP_NAME);
    if(!bookLogsFile.exists())
        return 1;   // No BackUp
    uint counter = 0;
    // Próba skopiowania backupu i zmiany nazwy
    while(!bookLogsFile.copy(BOOK_LOGS_FILE_NAME)){
        if(counter < 100)
          counter++;
        else{
            SERVER_MSG(BOOK_LOGS_FILE_OPEN_ERROR_TEXT);
            return 2;   // Open File Error
          }
        }
    return 0;
}


void BookLogsFilesMenager::clearMemory(){

}

BookLogParameters BookLogsFilesMenager::checkBookLogParameters(QString &checkStr){
    CHECK_PARAM_INIT;
    switch(checkStr.length()){
    case 3:
    {
        // Check END_TOKEN
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_END_TOKEN, 3, BOOK_LOG_END_PARAMETER_TOKEN);
        // Check ...

        // End of Check for 3 Signs
    }
    break;
    case 5:
    {
        // Check PESEL
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PESEL, 5, BOOK_LOG_USER_PESEL);
        // Check ...

        // End of Check for 5 Signs
    }
    break;
    case 6:
    {
        // Check BOOK_ID
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_BOOK_ID, 6, BOOK_LOG_BOOK_ID);

        // Check BOOK_LOG_ACTION
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION, 6, BOOK_LOG_ACTION);

        // Check ...

        // End Of Check for 6 Signs
    }
    break;
    case 7:
    {
        // Check BOOK_LOG_USER_SURNAME
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_USER_SURNAME, 7, BOOK_LOG_USER_SURNAME);
        // Check ...

        // End Of Check for 7 Signs
    }
    break;
    case 8:
    {
        // Check BOOK_LOG_DATE_TIME_OPTIONAL
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_DATE_TIME_OPTIONAL, 8, BOOK_LOG_DATE_TIME_OPTIONAL);
        // Check ...

        // End Of Check for 8 Signs
    }
    break;
    case 9:
    {
        // Check BOOK_LOG_FIRST_NAME
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_USER_FIRST_NAME, 9, BOOK_LOG_USER_FIRST_NAME);
        // Check BOOK_LOG_USER_ID_COM
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_USER_ID_COMMENT, 9, BOOK_LOG_USER_ID_COMMENT);

        // Check ...

        // End Of Check for 9 Signs
    }
    break;
    case 10:
    {
        // Check BOOK_LOG_COMMENT_CONTENT
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_COMMENT_CONTENT, 10, BOOK_LOG_COMMENT_CONTENT);

        // Check ...

        // End Of Check for 10 Signs
    }
    break;
    case 11:
    {
        // Check BOOK_LOG_PERMISSIONS
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_USER_PERMISSIONS, 11, BOOK_LOG_USER_PERMISSIONS);
        // Check ...

        // End Of Check for 11 Signs
    }
        break;
    case 14:
    {
        // Check BOOK_LOG_ACTION_DATE_TIME
        CHECK_PARAM_RETURN_V(checkStr, BOOK_LOG_PARAMETERS_BOOK_LOG_ACTION_DATE_TIME, 14, BOOK_LOG_ACTION_DATE_TIME);
        // Check ...

        // End Of Check for 14 Signs
    }
    break;
    default:
    break;
    }    
    return BOOK_LOG_NUMB_OF_PARAMETERS; // Parameter not Found    
}

bool BookLogsFilesMenager::readBookLogsFile(ReadFileRules& rules){
    //filePos = getFilePos(rules.getSkipNumber());// Reset File Position Pointer
    QFile file(BOOK_LOGS_FILE_NAME);
    if(!file.exists()){
        switch(restoreBookLogsFile()){
        case 1: // No BackUp
            if(!createBookLogsFile()) // Creating Error
                return false;
            break;
        case 2: // Open File Error
            return false;
        default:    // Restored File
            break;
        }
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(BOOK_LOGS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        // File Reading Depends of Reading Rules
        SERVER_MSG("--- Book Logs File Reading Start ---");        
        BookLog tempBookLog;
        do{
            if(!readNextBookLog(tempBookLog, file))
            {
                file.close();
                SERVER_MSG("--- Book Logs File Read Failed ---");
                return false; // READING ERROR
            }
            if(!tempBookLog.checkBookLogFromFile()){
                // Fail of book check (Depends of command type)
                file.close();
                SERVER_MSG("--- Book File Read Failed ---");
                return false;
            }
        }while(rules.check(tempBookLog, actualSocket));
        file.close();
        SERVER_MSG("--- Book File Read Success");
    }
}

bool BookLogsFilesMenager::readNextBookLog(BookLog &tempBookLog, QFile &file){
    QString tempChar;
    QString tempStr;
    bool paramReaded = false;
    FileReadingStat frs = FILE_READING_SEARCHING;
    BookLogParameters bookLogParameter = BOOK_LOG_NUMB_OF_PARAMETERS;
    tempBookLog = BookLog();
    // Find next book
    bool stop = false;
    while(!stop){
        if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
            SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
            SERVER_MSG("Book Logs file end while reading books parameters");
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
                    if((bookLogParameter = checkBookLogParameters(tempStr)) == BOOK_LOG_NUMB_OF_PARAMETERS){
                        SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                        SERVER_MSG("Books file have corrupted parameter name");
                        /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                        return false;
                    }
                    // ---------------------
                    frs = FILE_READING_READING_VALUE;
                    if(bookLogParameter == BOOK_LOG_END_PARAMETER_TOKEN){
                        if(!paramReaded)
                            tempBookLog.endOfBookLogFile = true;
                        return true;
                    }else{
                        paramReaded = true;
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
                    tempBookLog.setParam(bookLogParameter, tempStr);
                    tempStr.clear();
                    frs = FILE_READING_SEARCHING;
            }
            break;
        }
    }
}



void BookLogsFilesMenager::addBookLog(MyTcpSocket* newActualSocket, BookLog& bookLog){
    actualSocket = newActualSocket;
    writeBookLogsFile(bookLog);
}

void BookLogsFilesMenager::readBookLogs(MyTcpSocket *newActualSocket){
    actualSocket = newActualSocket;
    QJsonObject jsonObj = actualSocket->requestData.value(READ_FILE_RULES_JSON_KEY_TEXT).toObject();
    ReadFileRules rfr(jsonObj, parent);
    if(!rfr.isConstructingError())
        readBookLogsFile(rfr);
    else {
        newActualSocket->setReturnErrorType(RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED); // _PH_ Change to Read File Rules JSON Corrupted
    }
}

bool BookLogsFilesMenager::writeBookLogsFile(BookLog& requestBookLog){
    // Reading Rules for Write File
    ReadFileRules readFileRules(FILE_TYPE_BOOK_LOG_FILE, parent);
    QFile file(BOOK_LOGS_FILE_NAME);
    if(!file.exists()){
        switch(restoreBookLogsFile()){
        case 1: // No BackUp
            if(!createBookLogsFile()) // Creating Error
                return false;
            break;
        case 2: // Open File Error
            return false;
        default:    // Restored File
            break;
        }
    }
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
            BookLog tempBookLog;
            writeNextBookLog(requestBookLog, newFile);
            do{
                if(!readNextBookLog(tempBookLog, file))
                {
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Book Logs File Read Failed ---");
                    return false; // READING ERROR
                }
                if(!tempBookLog.checkBookLogFromFile()){
                    // Fail of book logs check (Depends of command type)
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Book File Read Failed ---");
                    return false;
                }
                writeNextBookLog(tempBookLog, newFile);
            }while(readFileRules.check(tempBookLog, actualSocket));
            newFile.close();            
        }
        file.close();
        if(!file.remove()){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        if(!newFile.rename(BOOK_LOGS_FILE_NAME)){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        if(!createBookLogsFileBackUp()){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Book File Read Failed ---");
            return true;
        }
        SERVER_MSG("--- Temp File Write Success");
    }
    return true;
}

bool BookLogsFilesMenager::writeNextBookLog(BookLog &bookLog, QFile &file){
    QTextStream s(&file);
    if(bookLog.endOfBookLogFile){
        // END (DONT MODIFY)
        s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_END_PARAMETER_TOKEN);
        return  true;
    }
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_USER_PESEL); //*
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_USER_PERMISSIONS); //*
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_ACTION); //*
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_USER_FIRST_NAME); //*
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_USER_SURNAME); //*
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_ACTION_DATE_TIME);
    if(bookLog.getBookId() != 0)
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_BOOK_ID);
    if(!bookLog.getDateTimeOptional().isNull())
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_DATE_TIME_OPTIONAL);
    if(bookLog.getUserId() != 0){
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_USER_ID_COMMENT);
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_COMMENT_CONTENT);
    }
    // END (DONT MODIFY)
    s << WRITE_PARAM_TO_FILE(bookLog, BOOK_LOG_END_PARAMETER_TOKEN);

    return true;
}

