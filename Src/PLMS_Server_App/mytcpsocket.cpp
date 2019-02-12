#include"mytcpsocket.hpp"
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonArray>
#include"app.hpp"
#include"user.hpp"
#include"readfilerules.hpp"
#include"book.hpp"
#include"booklog.hpp"
#include"bookcomment.hpp"

MyTcpSocket::MyTcpSocket(QTcpSocket* tcpSocket, App* app)
    : tcpSocket(tcpSocket), msgType(NUMB_OF_MESSAGE_TYPES), app(app)
{}

MyTcpSocket::~MyTcpSocket(){
    tcpSocket->disconnectFromHost();
    if(tcpSocket->state() == QAbstractSocket::UnconnectedState || tcpSocket->waitForDisconnected(3000))
        qDebug() << "Socket disconnected";
    tcpSocket->close();
    tcpSocket->deleteLater();
    clearMemory();
}

bool MyTcpSocket::waitForReadyRead(int milis){
    return tcpSocket->waitForReadyRead(milis);
}

QByteArray MyTcpSocket::readAll(){
    return tcpSocket->readAll();
}

void MyTcpSocket::close(){
    tcpSocket->close();
}

qint64 MyTcpSocket::bytesAvailable() const{
    return tcpSocket->bytesAvailable();
}

void MyTcpSocket::disconnectFromHost(){
    tcpSocket->disconnectFromHost();
}

bool MyTcpSocket::waitForDisconnected(int milis){
    return tcpSocket->waitForDisconnected(milis);
}

void MyTcpSocket::setMessageType(MessageType set){
    msgType = set;
}

void MyTcpSocket::decodeRequest(QString msg){
    if(returnErrorType == RETURN_ERROR_NO_ERROR){
    int len = msg.length();
    if(len == 0)
        return;
    int i = 0;
    QString tempStr;
    switch (decodeStat) {
    case GET_POST_READ:
    {
        // Read First Word (GET or POST)
        while(i < len && msg.at(i) != ' '){
            tempStr.append(msg.at(i++));
        }
        if(i == len)
            break;
        // Check its POST or GET
        QString checkStr;
        switch(tempStr.length()){
        case 3: // GET
        {
            checkStr = MESSAGE_TYPE_GET_TEXT;
            uint j = 0;
            for(; j < 3; j++)
                if(checkStr.at(j) != tempStr.at(j))
                    break;
            if(j == 3)
                msgType = GET;
         }
            break;
        case 4: // POST
        {
            checkStr = MESSAGE_TYPE_POST_TEXT;
            uint j = 0;
            for(; j < 4; j++)
                if(checkStr.at(j) != tempStr.at(j))
                    break;
            if(j == 4)
                msgType = POST;
         }
            break;
        default:
            break;
        }
        if(msgType == NUMB_OF_MESSAGE_TYPES){
            returnErrorType = RETURN_ERROR_WRONG_MESSAGE_TYPE;
            break;
        }
        decodeStat = COMMAND_READ;
    }
    [[clang::fallthrough]];
    case COMMAND_READ:
    {
        tempStr.clear();
        // Find /
        while(i < len && msg.at(i++) != '/');
        if(i == len)
            break;
        // Read Command
        while(i < len && msg.at(i) != ' '){
            tempStr.append(msg.at(i++));
        }
        if(i == len)
            break;
        // Check Command
        if(!checkCommand(tempStr)){
            returnErrorType = RETURN_ERROR_WRONG_COMMAND;
            break;
        }
        if(msgType == POST)
            decodeStat = CONTENT_TYPE_READ;
        else
        {
            decodeStat = DECODING_FINISH;
            break;
        }
    }
    [[clang::fallthrough]];
    case CONTENT_TYPE_READ:
    {
        tempStr.clear();
        // Find Content-Type
        QString checkStr = REQUEST_DECODE_CONTENT_TYPE_TEXT;
        int stopValue = checkStr.length() - 1;
        int tempI = -1;
        while(i < len && tempI < stopValue){
            if(msg.at(i++) == checkStr.at(tempI + 1)){
               tempI++;
             }else{
               tempI = -1;
            }
          }
        if(i == len)
            break;
        if(tempI != stopValue)
        {
            returnErrorType = RETURN_ERROR_NO_CONTENT_TYPE;
            break;
        }
        // Find Space Sign
        while(i < len && msg.at(i++) != ' ');
        if(i == len)
            break;
        // Read Content-Type value (Json type)
        checkStr = REQUEST_DECODE_CONTENT_TYPE_VALUE_JSON_TEXT;
        stopValue = checkStr.length();
        for(tempI = 0; i + tempI < len && tempI < stopValue; tempI++){
            if(msg.at(i+tempI) != checkStr.at(tempI))
                break;
        }
        if(i == len)
            break;
        if(tempI != stopValue){
            returnErrorType = RETURN_ERROR_CONTENT_TYPE_VALUE_NOT_JSON;
            break;
        }
        decodeStat = DATA_READ;
    }
    [[clang::fallthrough]];
    case DATA_READ:
        {
            tempStr.clear();
            uint curlyBracketCounter = 0;
            // Find First Curly Bracket
            while(i < len && msg.at(i++) != '{');
            if(i == len)
                break;
            curlyBracketCounter++;
            tempStr.append('{');
            // Find Last Curly Bracket
            while(i < len && curlyBracketCounter > 0){
                if(msg.at(i) == '}')
                    curlyBracketCounter--;
                if(msg.at(i) == '{')
                    curlyBracketCounter++;
                tempStr.append(msg.at(i++));
            }
            if(i == len)
                break;            
            requestData = (QJsonDocument::fromJson((tempStr + "\n").toUtf8(), &jsonParseError)).object();
            if(jsonParseError.error != QJsonParseError::NoError){
                returnErrorType = RETURN_ERROR_JSON_PARSE_ERROR;
            }
            decodeStat = DECODING_FINISH;
        }
        break;
    default:
        break;
        }
    }
}

void MyTcpSocket::sendReturnMessage(){
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    if(returnErrorType == RETURN_ERROR_NO_ERROR){ // _PH_ Modify to add additional information (EDIT ONLY returnData VARIABLE)
        // Make Log
        if(bookLogs){
            app->getBookLogsFilesMenager().addBookLog(this, *bookLogs);
        }
        // DONT MODIFY________________________
        returnData.insert(RETURN_ERROR_JSON_VARIABLE_TEXT, QString::number(returnErrorType));
        // ___________________________________
        // ADDITIONAL INFORMATION____________________

        // __________________________________________

        jsonDoc.setObject(returnData);  // ALWAYS AT END
    }else{
        jsonObj.insert(RETURN_ERROR_JSON_VARIABLE_TEXT, QString::number(returnErrorType));
        switch(returnErrorType){    // _PH_ Modify to add additional error information (EDIT ONLY jsonObj VARIABLE)
        case RETURN_ERROR_JSON_PARSE_ERROR:
            jsonObj.insert(RETURN_ERROR_JSON_PARSE_ERROR_VARIABLE_TEXT, QString::number(jsonParseError.error));
            break;
        default:
            break;
        }
        jsonDoc.setObject(jsonObj); // ALWAYS AT END
    }    
    tcpSocket->write(RETURN_MESSAGE(jsonDoc));
    tcpSocket->waitForBytesWritten(3000);
    delete this;
}

void MyTcpSocket::setReturnErrorType(ReturnErrorType set){
    returnErrorType = set;
}

void MyTcpSocket::process(){
    // Process Function Initialization
    // _PH_ FUNCTION WORK AT SERVER THREAT (REIMPLEMENT WHEN IMPLEMENT MULTI_THREAT APP)
    if(returnErrorType == RETURN_ERROR_NO_ERROR && decodeStat != DECODING_FINISH)
        returnErrorType = RETURN_ERROR_NOT_COMPLETE_HTTP_MSG;
    tcpSocketStat = TCP_SOCKET_PROCESSING;
    if(returnErrorType != RETURN_ERROR_NO_ERROR){
        sendReturnMessage();
        return;
    }
    // Process
    switch(cmdType){
    // _PH_ ADD COMMAND PROCESSING CODE IF NEED
    case COMMAND_TYPE_CLIENT_ADD:
    [[clang::fallthrough]]; // Fallthrough
    case COMMAND_TYPE_CLIENT_REGISTER:
    {
        QJsonArray jA;
        if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            jA = requestData.value(USER_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                break;
            }
            {
                User user(jA.at(0).toObject());
                if(user.getUserId() != 0 || !user.checkUserParameters()){
                    returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                    break;
                }
             }
             returnUsers_Books = new QJsonArray();
             app->getClientsFilesMenager().addEditRemoveClient(this);
             returnData.insert(USER_JSON_KEY_TEXT, *returnUsers_Books);
             SET_PTR_DO(returnUsers_Books, nullptr);
        }
    }
        break;
    case COMMAND_TYPE_CLIENT_REMOVE:
        bookLogs = new BookLog[1]; // FallThrough
    case COMMAND_TYPE_CLIENT_EDIT:
    {
        QJsonArray jA;
        if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            jA = requestData.value(USER_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                break;
            }
            {
                User user(jA.at(0).toObject());
                if(user.getUserId() == 0){
                    returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                    break;
                }
             }
            app->getClientsFilesMenager().addEditRemoveClient(this);
        }
    }
        break;
    case COMMAND_TYPE_CLIENT_READ:
    {
        if(requestData.value(READ_FILE_RULES_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
            break;
        }
        returnUsers_Books = new QJsonArray();
        app->getClientsFilesMenager().readClients(this);
        returnData.insert(USER_JSON_KEY_TEXT, *returnUsers_Books);
        SET_PTR_DO(returnUsers_Books, nullptr);
    }
        break;
    case COMMAND_TYPE_CLIENT_LOGIN:
    {
        QJsonArray jA;
        if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            jA = requestData.value(USER_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                break;
            }
            returnUsers_Books = new QJsonArray();
            app->getClientsFilesMenager().loginClient(this);
            returnData.insert(USER_JSON_KEY_TEXT, *returnUsers_Books);
            SET_PTR_DO(returnUsers_Books, nullptr);
        }
    }
        break;
    case COMMAND_TYPE_CLIENT_LOGOUT:
    {
        if(requestData.value(USER_PARAMETERS_USER_ID) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            uint len = requestData.value(USER_PARAMETERS_USER_ID).toArray().count();
            if(len > 0){
                app->getClientsFilesMenager().logoutClient(this);
                bookLogs = new BookLog[len];
                bookLogsIter = bookLogs;
                numbOfBookLogs = len;
                QJsonArray filterArr;
                QJsonObject filterObj;
                for(uint i = 0; i < len ;i++){
                    filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_ID));
                    filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, requestData.value(USER_PARAMETERS_USER_ID).toArray().at(i).toString());
                    filterArr.append(filterObj);
                    filterObj = QJsonObject();
                }
                filterObj.insert(READ_FILE_RULES_FILE_TYPE_TEXT, QString::number(FILE_TYPE_CLIENTS_FILE));
                filterObj.insert(READ_FILE_RULES_MAX_READ_TEXT, QString::number(len));
                filterObj.insert(READ_FILE_RULES_FULL_FILTER, QString::number(1));
                filterObj.insert(READ_FILE_RULES_FILTER_TEXT, filterArr);
                ReadFileRules rfr(filterObj, app);
                app->getClientsFilesMenager().readClients(this, rfr);
                for(uint i = 0; i < len; i++)
                    app->getBookLogsFilesMenager().addBookLog(this, (*(bookLogs + i)));
                SET_PTR_DOA(bookLogs, nullptr);
                numbOfBookLogs = 0;
            }

        }
    }
        break;
    case COMMAND_TYPE_CLIENT_ACTIVITY:
    {
        if(requestData.value(USER_PARAMETERS_USER_ID) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            if(requestData.value(USER_PARAMETERS_USER_ID).toArray().count() > 0)
                app->getClientsFilesMenager().extendActivity(this);
        }
    }
        break;
    case COMMAND_TYPE_BOOK_ADD:
    {        
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
                returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
                break;
            }else{
                bookLogs = new BookLog[1];
                jA = requestData.value(USER_JSON_KEY_TEXT).toArray();
                if(jA.count() != 1){
                    returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                }
                {
                    User user(jA.at(0).toObject());
                    if(user.getUserId() == 0 || !user.checkUserParameters()){
                        returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                    }
                    bookLogs->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
                    bookLogs->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
                    bookLogs->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
                    bookLogs->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
                }
                jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
                if(jA.count() != 1){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
                {
                    Book book(jA.at(0).toObject());
                    if(book.getBookId() != 0 || !book.checkBookParameters()){
                        returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                    }
                }
                returnUsers_Books = new QJsonArray();
                app->getBooksFilesMenager().addEditRemoveBook(this);
                returnData.insert(BOOK_JSON_KEY_TEXT, *returnUsers_Books);
                SET_PTR_DO(returnUsers_Books, nullptr);
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_REMOVE:
    {

        QJsonArray jA;
        if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }else{
            bookLogs = new BookLog[1];
            jA = requestData.value(USER_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
            }
            {
                User user(jA.at(0).toObject());
                if(user.getUserId() != 0 || !user.checkUserParameters()){
                    returnErrorType = RETURN_ERROR_USER_JSON_CORRUPTED;
                }
                bookLogs->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
                bookLogs->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
                bookLogs->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
                bookLogs->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
            }
            QJsonArray jA;
            if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
                returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
                break;
            }else{
                jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
                if(jA.count() != 1){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                    break;
                }

                    Book book(jA.at(0).toObject());
                    if(book.getBookId() == 0){
                        returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                        break;
                    }

                app->getBooksFilesMenager().addEditRemoveBook(this);
                if(returnErrorType != RETURN_ERROR_NO_ERROR){
                    break;
                }
                User user;
                user.setParam(USER_ID, bookLogs->getParam(BOOK_LOG_USER_ID_COMMENT));
                user.setParam(USER_BOOK_ID, book.getParam(BOOK_ID));
                bookLogs->setParam(BOOK_LOG_USER_ID_COMMENT, QString("0"));
                app->getClientsFilesMenager().addEditRemoveClient(this, user);
                if(returnErrorType != RETURN_ERROR_NO_ERROR){
                    switch(app->getClientsFilesMenager().restoreClientsFile()){
                    case 1: // No BackUp
                        if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                            break;
                        break;
                    case 2: // Open File Error
                        break;
                    default:    // Restored File
                        break;
                    }
                    switch(app->getBooksFilesMenager().restoreBooksFile()){
                    case 1: // No BackUp
                        if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                            break;
                        break;
                    case 2: // Open File Error
                        break;
                    default:    // Restored File
                        break;
                    }
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_EDIT:
    {
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                break;
            }

                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                    break;
                }

            app->getBooksFilesMenager().addEditRemoveBook(this);              
        }
    }
        break;
    case COMMAND_TYPE_BOOK_READ:
    {
        if(requestData.value(READ_FILE_RULES_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED;
            break;
        }
        returnUsers_Books = new QJsonArray();
        app->getBooksFilesMenager().readBooks(this);
        returnData.insert(BOOK_JSON_KEY_TEXT, *returnUsers_Books);
        SET_PTR_DO(returnUsers_Books, nullptr);
    }
        break;
    case COMMAND_TYPE_BOOK_RETURN:
    {
        bookLogs = new BookLog[1];
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
            }

                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
            bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_RETURN_BOOK));
            bookLogs->setParam(BOOK_LOG_BOOK_ID, book.getParam(BOOK_ID));
            app->getBooksFilesMenager().addEditRemoveBook(this);

            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                break;
            }
            User user;
            user.setParam(USER_ID, book.getParam(BOOK_USER_ID));
            user.setParam(USER_BOOK_ID, book.getParam(BOOK_ID));
            app->getClientsFilesMenager().addEditRemoveClient(this, user);
            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                switch(app->getClientsFilesMenager().restoreClientsFile()){
                case 1: // No BackUp
                    if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
                switch(app->getBooksFilesMenager().restoreBooksFile()){
                case 1: // No BackUp
                    if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_RESERVE:
    {
        bookLogs = new BookLog[1];
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
            }
                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
                book.setParam(BOOK_STATUS, QString::number(BOOK_STATUS_RESERVED));
            bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_RESERVE_BOOK));
            bookLogs->setParam(BOOK_LOG_BOOK_ID, book.getParam(BOOK_ID));
            app->getBooksFilesMenager().addEditRemoveBook(this);

            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                break;
            }
            User user;
            user.setParam(USER_ID, book.getParam(BOOK_USER_ID));
            user.setParam(USER_BOOK_ID, book.getParam(BOOK_ID));
            app->getClientsFilesMenager().addEditRemoveClient(this, user);
            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                switch(app->getClientsFilesMenager().restoreClientsFile()){
                case 1: // No BackUp
                    if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
                switch(app->getBooksFilesMenager().restoreBooksFile()){
                case 1: // No BackUp
                    if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_CHECKOUT:
    {
        bookLogs = new BookLog[1];
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
            }
                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
                book.setParam(BOOK_STATUS, QString::number(BOOK_STATUS_CHECKED_OUT));
                bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_CHECK_OUT_BOOK));
            bookLogs->setParam(BOOK_LOG_BOOK_ID, book.getParam(BOOK_ID));
            app->getBooksFilesMenager().addEditRemoveBook(this);

            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                break;
            }
            if(book.getParam(BOOK_USER_ID).toULongLong() != 0){
                User user;
                user.setParam(USER_ID, book.getParam(BOOK_USER_ID));
                user.setParam(USER_BOOK_ID, book.getParam(BOOK_ID));
                app->getClientsFilesMenager().addEditRemoveClient(this, user);
            }else{
                QJsonArray filterArr;
                QJsonObject filterObj;
                filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_ID));
                filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, bookLogs->getParam(BOOK_LOG_USER_ID_COMMENT));
                filterArr.append(filterObj);
                bookLogs->setParam(BOOK_LOG_USER_ID_COMMENT, QString("0"));
                filterObj = QJsonObject();
                filterObj.insert(READ_FILE_RULES_FILE_TYPE_TEXT, QString::number(FILE_TYPE_CLIENTS_FILE));
                filterObj.insert(READ_FILE_RULES_MAX_READ_TEXT, QString::number(1));
                filterObj.insert(READ_FILE_RULES_FULL_FILTER, QString::number(1));
                filterObj.insert(READ_FILE_RULES_FILTER_TEXT, filterArr);
                ReadFileRules rfr(filterObj, app);
                app->getClientsFilesMenager().readClients(this, rfr);
            }
            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                switch(app->getClientsFilesMenager().restoreClientsFile()){
                case 1: // No BackUp
                    if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
                switch(app->getBooksFilesMenager().restoreBooksFile()){
                case 1: // No BackUp
                    if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_COMMENT_REMOVE:
    {
        bookLogs = new BookLog[1];
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
            }
                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
            bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_REMOVE_COMMENT));
            bookLogs->setParam(BOOK_LOG_BOOK_ID, book.getParam(BOOK_ID));
            app->getBooksFilesMenager().addEditRemoveBook(this);

            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                break;
            }
            QJsonArray filterArr;
            QJsonObject filterObj;
            filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_ID));
            filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, bookLogs->getParam(BOOK_LOG_USER_ID_COMMENT));
            filterArr.append(filterObj);
            filterObj = QJsonObject();
            filterObj.insert(READ_FILE_RULES_FILE_TYPE_TEXT, QString::number(FILE_TYPE_CLIENTS_FILE));
            filterObj.insert(READ_FILE_RULES_MAX_READ_TEXT, QString::number(1));
            filterObj.insert(READ_FILE_RULES_FULL_FILTER, QString::number(1));
            filterObj.insert(READ_FILE_RULES_FILTER_TEXT, filterArr);
            ReadFileRules rfr(filterObj, app);
            app->getClientsFilesMenager().readClients(this, rfr);
            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                switch(app->getClientsFilesMenager().restoreClientsFile()){
                case 1: // No BackUp
                    if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
                switch(app->getBooksFilesMenager().restoreBooksFile()){
                case 1: // No BackUp
                    if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_COMMENT_ADD_EDIT:
    {
        bookLogs = new BookLog[1];
        QJsonArray jA;
        if(requestData.value(BOOK_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_BOOK_NOT_SENT;
            break;
        }else{
            jA = requestData.value(BOOK_JSON_KEY_TEXT).toArray();
            if(jA.count() != 1){
                returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
            }

                Book book(jA.at(0).toObject());
                if(book.getBookId() == 0){
                    returnErrorType = RETURN_ERROR_BOOK_JSON_CORRUPTED;
                }
            bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_ADD_EDIT_COMMENT));
            bookLogs->setParam(BOOK_LOG_BOOK_ID, book.getParam(BOOK_ID));
            app->getBooksFilesMenager().addEditRemoveBook(this);

            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                break;
            }
            QJsonArray filterArr;
            QJsonObject filterObj;
            filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_ID));
            filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, QString::number((*book.getBookComments()).userId));
            filterArr.append(filterObj);
            filterObj = QJsonObject();
            filterObj.insert(READ_FILE_RULES_FILE_TYPE_TEXT, QString::number(FILE_TYPE_CLIENTS_FILE));
            filterObj.insert(READ_FILE_RULES_MAX_READ_TEXT, QString::number(1));
            filterObj.insert(READ_FILE_RULES_FULL_FILTER, QString::number(1));
            filterObj.insert(READ_FILE_RULES_FILTER_TEXT, filterArr);
            ReadFileRules rfr(filterObj, app);
            app->getClientsFilesMenager().readClients(this, rfr);
            if(returnErrorType != RETURN_ERROR_NO_ERROR){
                switch(app->getClientsFilesMenager().restoreClientsFile()){
                case 1: // No BackUp
                    if(!app->getClientsFilesMenager().createClientsFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
                switch(app->getBooksFilesMenager().restoreBooksFile()){
                case 1: // No BackUp
                    if(!app->getBooksFilesMenager().createBooksFile()) // Creating Error
                        break;
                    break;
                case 2: // Open File Error
                    break;
                default:    // Restored File
                    break;
                }
            }
        }
    }
        break;
    case COMMAND_TYPE_BOOK_LOG_READ:
    {
        if(requestData.value(READ_FILE_RULES_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED;
            break;
        }
        returnUsers_Books = new QJsonArray();
        app->getBookLogsFilesMenager().readBookLogs(this);
        returnData.insert(BOOK_JSON_KEY_TEXT, *returnUsers_Books);
        SET_PTR_DO(returnUsers_Books, nullptr);
    }
        break;
    default:
        break;
    }
    tcpSocketStat = TCP_SOCKET_FINISHED; // _PH_ Need for work with threats
    sendReturnMessage();
}

bool MyTcpSocket::checkCommand(QString &cmd){
    CHECK_PARAM_INIT;
    switch(cmd.length()){
    case 12:
        // Check CLIENT_LOGIN
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_LOGIN_TEXT, 12);
        if(i == 12){
            cmdType = COMMAND_TYPE_CLIENT_LOGIN;
            return true;
        }
        // Check BOOK_RESERVE
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_RESERVE_TEXT, 12);
        if(i == 12){
            cmdType = COMMAND_TYPE_BOOK_RESERVE;
            return true;
        }
        // Check BOOK_LOG_READ
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_LOG_READ_TEXT, 12);
        if(i == 12){
            cmdType = COMMAND_TYPE_BOOK_LOG_READ;
            return true;
        }
        // Check ...

        break;
    case 15:
        // Check CLIENT_REGISTER
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_REGISTER_TEXT, 15);
        if(i == 15){
            cmdType = COMMAND_TYPE_CLIENT_REGISTER;
            return true;
        }
        // Check CLIENT_ACTIVITY
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_ACTIVITY_TEXT, 15);
        if(i == 15){
            cmdType = COMMAND_TYPE_CLIENT_ACTIVITY;
            return true;
        }
        // Check BOOK_COMADDEDIT
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_COMMENT_ADD_EDIT_TEXT, 15);
        if(i == 15){
            cmdType = COMMAND_TYPE_BOOK_COMMENT_ADD_EDIT;
            return true;
        }
        // Check ...

        break;
    case 14:
        // Check BOOK_COMREMOVE
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_COMMENT_REMOVE_TEXT, 14);
        if(i == 14){
            cmdType = COMMAND_TYPE_BOOK_COMMENT_REMOVE;
            return true;
        }
        // Check ...
    break;
    case 10:
        // Check CLIENT_ADD
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_ADD_TEXT, 10);
        if(i == 10){
            cmdType = COMMAND_TYPE_CLIENT_ADD;
            return true;
        }
        // Check ...

        break;
    case 11:
        // Check CLIENT_EDIT
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_EDIT_TEXT, 11);
        if(i == 11){
            cmdType = COMMAND_TYPE_CLIENT_EDIT;
            return  true;
        }
        // Check CLIENT_READ
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_READ_TEXT, 11);
        if(i == 11){
            cmdType = COMMAND_TYPE_CLIENT_READ;
            return  true;
        }
        // Check CLIENT_REMOVE
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_REMOVE_TEXT, 11);
        if(i == 11){
            cmdType = COMMAND_TYPE_BOOK_REMOVE;
            return  true;
        }
        // Check BOOK_RETURN
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_RETURN_TEXT, 11);
        if(i == 11){
            cmdType = COMMAND_TYPE_BOOK_RETURN;
            return  true;
        }
        // Check ...

        break;
    case 13:
        // Check CLIENT_REMOVE
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_REMOVE_TEXT, 13);
        if(i == 13){
            cmdType = COMMAND_TYPE_CLIENT_REMOVE;
            return true;
        }
        // Check CLIENT_LOGOUT
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_LOGOUT_TEXT, 13);
        if(i == 13){
            cmdType = COMMAND_TYPE_CLIENT_LOGOUT;
            return true;
        }
        // Check BOOK_CHECK_OUT
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_CHECK_OUT_TEXT, 13);
        if(i == 13){
            cmdType = COMMAND_TYPE_BOOK_CHECKOUT;
            return true;
        }
        // Check ...

        break;
    case 8:
        // Check BOOK_ADD
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_ADD_TEXT, 8);
        if(i == 8){
            cmdType = COMMAND_TYPE_BOOK_ADD;
            return true;
        }
        // Check ...

        break;
    case 9:
        // Check BOOK_EDIT
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_EDIT_TEXT, 9);
        if(i == 9){
            cmdType = COMMAND_TYPE_BOOK_EDIT;
            return true;
        }
        // Check BOOK_READ
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_BOOK_READ_TEXT, 9);
        if(i == 9){
            cmdType = COMMAND_TYPE_BOOK_READ;
            return true;
        }
        // Check ...

        break;
    default:
        break;
    }
    return false;
}

BookLog* MyTcpSocket::getBookLog(){
    return bookLogs;
}

void MyTcpSocket::processReadedUserFromFile(User &user){
    switch(cmdType){
    case COMMAND_TYPE_BOOK_COMMENT_ADD_EDIT:
    {
        bookLogs->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
        bookLogs->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
        bookLogs->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
        bookLogs->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
    }
        return;
    case COMMAND_TYPE_BOOK_CHECKOUT:
    {
        bookLogs->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
        bookLogs->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
        bookLogs->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
        bookLogs->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
    }
        return;
    case COMMAND_TYPE_CLIENT_REGISTER:
    {
        app->getClientsFilesMenager().insertFastLoggedClient(user.getUserId(), app->getClientsFilesMenager().getActualFilePos());
        bookLogs = new BookLog[1];
        bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_REGISTER_USER));
    }
      break;
    case COMMAND_TYPE_CLIENT_LOGIN:
    {
        bookLogs = new BookLog[1];
        app->getClientsFilesMenager().insertFastLoggedClient(user.getUserId(), app->getClientsFilesMenager().getActualFilePos());
        bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_LOGIN));
    }
        break;
    case COMMAND_TYPE_CLIENT_ADD:
    {
        bookLogs = new BookLog[1];
        bookLogs->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_ADD_USER));
        break;
    }
    case COMMAND_TYPE_CLIENT_READ:
    {
        QJsonObject tempUserJsonObject;
        user.writeJson(tempUserJsonObject);
        returnUsers_Books->append(tempUserJsonObject);
        return;
    }
    case COMMAND_TYPE_CLIENT_LOGOUT:
    {
        bookLogsIter->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_LOGOUT));
        bookLogsIter->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
        bookLogsIter->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
        bookLogsIter->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
        bookLogsIter->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
        bookLogsIter++;
        return;
    }
    default:
        return;
    }
    bookLogs->setParam(BOOK_LOG_USER_PESEL, user.getParam(USER_PESEL));
    bookLogs->setParam(BOOK_LOG_USER_SURNAME, user.getParam(USER_SURNAME));
    bookLogs->setParam(BOOK_LOG_USER_FIRST_NAME, user.getParam(USER_FIRST_NAME));
    bookLogs->setParam(BOOK_LOG_USER_PERMISSIONS, user.getParam(USER_PERMISSIONS));
    QJsonObject tempUserJsonObject;
    user.writeJson(tempUserJsonObject);
    returnUsers_Books->append(tempUserJsonObject);
}

void MyTcpSocket::processReadedBookFromFile(Book &book){
    switch(cmdType){
    case COMMAND_TYPE_BOOK_ADD:
    case COMMAND_TYPE_BOOK_READ:
    {
        QJsonObject tempBookJsonObject;
        book.writeJson(tempBookJsonObject);
        returnUsers_Books->append(tempBookJsonObject);
    }
        break;
    default:
        break;
    }
}

void MyTcpSocket::processReadedBookLogFromFile(BookLog &bookLog){
    switch(cmdType){
    case COMMAND_TYPE_BOOK_LOG_READ:
    {
        QJsonObject tempBookJsonObject;
        bookLog.writeJson(tempBookJsonObject);
        returnUsers_Books->append(tempBookJsonObject);
    }
        break;
    default:
        break;
    }
}

CommandType MyTcpSocket::getCmdType(){
    return cmdType;
}

void MyTcpSocket::addNextPossibleReadId(unsigned long long id){
    returnData.insert(READ_FILE_RULES_NEXT_POSSIBLE_READ_ID, QString::number(id));
}

void MyTcpSocket::clearMemory(){
    SET_PTR_DO(returnUsers_Books, nullptr);
    SET_PTR_DOA(bookLogs, nullptr);
    SET_PTR_NDO(bookLogsIter, nullptr);
    numbOfBookLogs = 0;
}
