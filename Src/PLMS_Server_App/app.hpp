#ifndef APP_HPP
#define APP_HPP
// ------------------ Macros --------------------------------------------
#define SERVER_MSG(msg) {qDebug() << QString(msg) + "\n";}
#define CHECK_PARAM_RETURN_V(checkStrV, paramNameText, numbOfSigns, returnV) {tempStr.clear(); tempStr = paramNameText; for(i = 0; i < numbOfSigns; i++) if(checkStrV.at(i) != tempStr.at(i)) break; if(i == numbOfSigns) return returnV;}
#define CHECK_PARAM_NO_RETURN_V(checkStrV, paramNameText, numbOfSigns) {tempStr.clear(); tempStr = paramNameText; for(i = 0; i < numbOfSigns; i++) if(checkStrV.at(i) != tempStr.at(i)) break;}
#define CHECK_PARAM_INIT QString tempStr; int i = 0;
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
    ClientsFilesMenager clientsFilesMenager;

public:
    static void readCharUtf8(QFile& file, QString& tempChar);
    ClientsFilesMenager& getClientsFilesMenager();

};

#endif // APP_HPP
