#include"httpserver.hpp"
#include<QObject>
#include"requestdecodestat.hpp"
#include"messagetype.hpp"
#include"mytcpsocket.hpp"
#include<QJsonDocument>
#include<QJsonObject>

HttpServer::HttpServer(App* parent)
    : parent(parent), server(this)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(myConnection()));
    if(!server.listen(QHostAddress::Any, 8080)) qDebug() << "Bład startu serwera";
    else    qDebug() << "Serwer is waiting\n";
    qDebug() << "Address: " + server.serverAddress().toString() + " Port: " + QString::number(server.serverPort()) + " \n";
}

HttpServer::~HttpServer(){
    socket->close();
}

void HttpServer::myConnection(){

    socket = new MyTcpSocket(server.nextPendingConnection());
    socket->waitForReadyRead(100);
    do
    {
    socket->decodeRequest(QString(socket->readAll()));
    socket->waitForReadyRead(100);
    }while(socket->bytesAvailable());
    // _PH_ ACTUAL SOCKET WORK AT SERVER THREAT
    socket->process();
}

