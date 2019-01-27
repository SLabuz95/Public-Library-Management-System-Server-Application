#ifndef MYTCPSOCKET_HPP
#define MYTCPSOCKET_HPP

// Memory Pointer Macros ------------------------------------------------------------------------------
#define SET_PTR_DO(ptr, newPtr) {if(ptr) delete ptr; ptr = newPtr;}
#define SET_PTR_DOA(ptrArray, newPtrArray) {if(ptrArray) delete []ptrArray; ptrArray = newPtrArray;}
#define SET_PTR_NDO(ptr, newPtr) {ptr = newPtr;}

#include<QTcpSocket>
#include"messagetype.hpp"
#include"requestdecodestat.hpp"
#include"returnerrortype.hpp"
#include"tcpsocketstat.hpp"
#include"commandtype.hpp"
#include<QJsonObject>
#include<QJsonParseError>

#define RETURN_MESSAGE(json) ((QString("HTTP/1.1 200 OK\r\nContent-type: application/json\r\nContent-length: ") + QString::number(json.toJson().length()) + QString("\r\n\r\n") + json.toJson()).toUtf8())

class App;
class User;
class Book;

class MyTcpSocket{
public:
    // Constructor
    MyTcpSocket(QTcpSocket* tcpSocket, App* app);

    // Destructor
    ~MyTcpSocket();

    App* app = nullptr;

    QTcpSocket* tcpSocket = nullptr;

    RequestDecodeStat decodeStat = GET_POST_READ;

    TcpSocketStat tcpSocketStat = TCP_SOCKET_DECODING;

    ReturnErrorType returnErrorType = RETURN_ERROR_NO_ERROR;

    QJsonParseError jsonParseError;

    MessageType msgType;

    CommandType cmdType;

    QJsonObject requestData;

    QJsonObject returnData;

    QJsonArray* returnUsers_Books = nullptr;    // Return data ONLY

    bool waitForReadyRead(int milis);
    QByteArray readAll();

    void close();
    qint64 bytesAvailable() const;
    void disconnectFromHost();
    bool waitForDisconnected(int milis);
    void setMessageType(MessageType set);
    void setReturnErrorType(ReturnErrorType set);
    void decodeRequest(QString msg);
    void sendReturnMessage();
    void setReadEnd(bool set);
    CommandType getCmdType();
    void process();
    bool checkCommand(QString&);
    void processReadedUserFromFile(User&);
    void processReadedBookFromFile(Book&);

};

#endif // MYTCPSOCKET_HPP
