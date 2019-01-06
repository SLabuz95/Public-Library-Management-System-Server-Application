#ifndef APP_HPP
#define APP_HPP
// ------------------ Macros --------------------------------------------
#define SERVER_MSG(msg) {qDebug() << QString(msg) + "\n";}
// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QCoreApplication>
#include"httpserver.hpp"
#include"clientsfilesmenager.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class User;
class QFile;
// ----------------------------------------------------------------------

// ------------------ App Class -----------------------------------------

class App : public QCoreApplication{
public:
    // Constructor
    App(int argc, char** argv);

    // Destructor
    ~App();

private:
    // Elements

    HttpServer httpServer;
    //ClientsFilesMenager clientsFilesMenager;

public:
    static void readCharUtf8(QFile& file, QString& tempChar);

};

#endif // APP_HPP
