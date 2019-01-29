#ifndef APP_HPP
#define APP_HPP
// ------------------ Macros --------------------------------------------
#define SERVER_MSG(msg) {qDebug() << QString(msg).toUtf8();}
#define CHECK_PARAM_RETURN_V(checkStrV, paramNameText, numbOfSigns, returnV) {tempStr.clear(); tempStr = paramNameText; for(i = 0; i < numbOfSigns; i++) if(checkStrV.at(i) != tempStr.at(i)) break; if(i == numbOfSigns) return returnV;}
#define CHECK_PARAM_NO_RETURN_V(checkStrV, paramNameText, numbOfSigns) {tempStr.clear(); tempStr = paramNameText; for(i = 0; i < numbOfSigns; i++) if(checkStrV.at(i) != tempStr.at(i)) break;}
#define CHECK_PARAM_INIT QString tempStr; int i = 0;
// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QCoreApplication>
#include"httpserver.hpp"
#include"clientsfilesmenager.hpp"
#include"booksfilesmenager.hpp"
#include<QTimer>

// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class User;
class QFile;
// ----------------------------------------------------------------------

// ------------------ App Class -----------------------------------------

class App : public QCoreApplication{
    Q_OBJECT
public:
    // Constructor
    App(int argc, char** argv);

    // Destructor
    ~App();

private:
    // Elements
    ClientsFilesMenager clientsFilesMenager;
    BooksFilesMenager booksFilesMenager;
    HttpServer httpServer;
    QTimer activityCheckTimer;


public:
    static void readCharUtf8(QFile& file, QString& tempChar);
    ClientsFilesMenager& getClientsFilesMenager();
    BooksFilesMenager& getBooksFilesMenager();

    static unsigned long long strLenForFile(QString&);

    void checkActivityTimer();

    bool eventFilter(QObject*, QEvent*);

};

#endif // APP_HPP
