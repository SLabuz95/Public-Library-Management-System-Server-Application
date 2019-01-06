#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

// ------------------ Macros --------------------------------------------

// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QTcpServer>
#include<QNetworkInterface>
#include<QTcpServer>


// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class User;
class MyTcpSocket;
class App;
// ----------------------------------------------------------------------

// ------------------ HTTP Server Class -----------------------------------------

class HttpServer : public QObject{
    Q_OBJECT
public:
    HttpServer(App*);

    ~HttpServer();

    MyTcpSocket* socket = nullptr;

    App* parent = nullptr;
    uint c = 0;
public slots:
    void myConnection();
private:
    qint64 bytesAvailable() const;
    QTcpServer server;

};

#endif // HTTPSERVER_HPP
