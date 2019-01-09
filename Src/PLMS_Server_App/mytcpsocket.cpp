#include"mytcpsocket.hpp"
#include<QJsonObject>
#include<QJsonDocument>
#include"app.hpp"
#include"user.hpp"
MyTcpSocket::MyTcpSocket(QTcpSocket* tcpSocket, App* app)
    : tcpSocket(tcpSocket), msgType(NUMB_OF_MESSAGE_TYPES), app(app)
{

}

MyTcpSocket::~MyTcpSocket(){
    tcpSocket->disconnectFromHost();
    if(tcpSocket->state() == QAbstractSocket::UnconnectedState || tcpSocket->waitForDisconnected(3000))
        qDebug() << "Socket disconnected";
    tcpSocket->close();

    tcpSocket->deleteLater();
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
    // Fall-Throught
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
        // Fall-Throught
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
            qDebug() << tempStr;
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
    jsonObj.insert(RETURN_ERROR_JSON_VARIABLE_TEXT, QJsonValue::fromVariant(returnErrorType));
    if(returnErrorType == RETURN_ERROR_NO_ERROR){
        switch(cmdType){
        // _PH_ ADD RETURN MESSAGE JSON TO WRITE (MODIFY jsonObj VARIABLE)
        default:
            break;
        }
    }else{
        switch(returnErrorType){
        case RETURN_ERROR_JSON_PARSE_ERROR:
            jsonObj.insert(RETURN_ERROR_JSON_PARSE_ERROR_VARIABLE_TEXT, QJsonValue::fromVariant(jsonParseError.error));
            break;
        default:
            break;
        }
    }
    jsonDoc.setObject(jsonObj);
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
    if(decodeStat != DECODING_FINISH)
        returnErrorType = RETURN_ERROR_NOT_COMPLETE_HTTP_MSG;
    tcpSocketStat = TCP_SOCKET_PROCESSING;
    if(returnErrorType != RETURN_ERROR_NO_ERROR){
        sendReturnMessage();
        return;
    }
    // Process
    switch(cmdType){
    // _PH_ ADD COMMAND PROCESSING CODE IF NEED
    case COMMAND_TYPE_CLIENT_REGISTER:
        if(requestData.value(USER_JSON_KEY_TEXT) == QJsonValue::Undefined){
            returnErrorType = RETURN_ERROR_JSON_USER_NOT_SENT;
            break;
        }
        app->getClientsFilesMenager().addClient(User(requestData.value(USER_JSON_KEY_TEXT).toObject()), this);

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
        // Check ...

        break;
    case 15:
        // Check CLIENT_REGISTER
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_REGISTER_TEXT, 15);
        if(i == 15){
            cmdType = COMMAND_TYPE_CLIENT_REGISTER;
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
        // Check ...

        break;
    case 13:
        // Check CLIENT_REMOVE
        CHECK_PARAM_NO_RETURN_V(cmd, COMMAND_TYPE_CLIENT_REMOVE_TEXT, 13);
        if(i == 13){
            cmdType = COMMAND_TYPE_CLIENT_REMOVE;
            return true;
        }
        // Check ...

        break;
    default:
        break;
    }
    return false;
}

void MyTcpSocket::processReadedUserFromFile(User &user){
    switch(cmdType){
    case COMMAND_TYPE_CLIENT_REGISTER:
        break;
    default:
        break;
    }
}

CommandType MyTcpSocket::getCmdType(){
    return cmdType;
}
