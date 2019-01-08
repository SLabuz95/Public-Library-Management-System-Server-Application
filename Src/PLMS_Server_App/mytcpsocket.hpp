#ifndef MYTCPSOCKET_HPP
#define MYTCPSOCKET_HPP

#include<QTcpSocket>
#include"messagetype.hpp"
#include"requestdecodestat.hpp"
#include"returnerrortype.hpp"
#include"tcpsocketstat.hpp"
#include"commandtype.hpp"
#include<QJsonObject>
#include<QJsonParseError>

#define RETURN_MESSAGE(json) ((QString("HTTP/1.1 200 OK\r\nContent-type: application/json\r\nContent-length: ") + QString::number(json.toJson().length()) + QString("\r\n\r\n") + json.toJson()).toUtf8())

class MyTcpSocket{
public:
    // Constructor
    MyTcpSocket(QTcpSocket* tcpSocket);

    // Destructor
    ~MyTcpSocket();

    QTcpSocket* tcpSocket = nullptr;

    RequestDecodeStat decodeStat = GET_POST_READ;

    TcpSocketStat tcpSocketStat = TCP_SOCKET_DECODING;

    ReturnErrorType returnErrorType = RETURN_ERROR_NO_ERROR;

    QJsonParseError jsonParseError;

    MessageType msgType;

    CommandType cmdType;

    QJsonObject requestData;

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
    void process();
    bool checkCommand(QString&);

};

#endif // MYTCPSOCKET_HPP
