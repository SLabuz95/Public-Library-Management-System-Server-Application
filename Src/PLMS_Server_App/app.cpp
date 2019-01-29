#include"app.hpp"
#include<QDebug>
#include<QFile>
#include<QByteArray>
#include<QTextCodec>

App::App(int argc, char** argv)
    : QCoreApplication (argc, argv), httpServer(this), clientsFilesMenager(this), booksFilesMenager(this)
{
    qDebug() << "Inicjalizacja serwera...\n";
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    activityCheckTimer.setInterval(2000);
    activityCheckTimer.installEventFilter(this);
    activityCheckTimer.start();
    qDebug() << "Serwer zainicjalizowany";
}

App::~App(){
    qDebug() << "Kończenie pracy serwera ...";
}

void App::readCharUtf8(QFile &file, QString &tempChar){
    QByteArray byteArr = QByteArray();
    char tempC;
    file.getChar(&tempC);
    byteArr.append(tempC);
    if(tempC < 0){
        if(tempC > -96)
        {
           file.getChar(&tempC);
           byteArr.append(tempC);
        }else{
             if(tempC > -112){
                 for(uint i = 0; i < 2; i++){
                    file.getChar(&tempC);
                    byteArr.append(tempC);
                 }
             }else{
                 if(tempC > -120){
                     for(uint i = 0; i < 3; i++){
                         file.getChar(&tempC);
                         byteArr.append(tempC);
                      }
                  }else{
                       if(tempC > -124){
                         for(uint i = 0; i < 4; i++){
                             file.getChar(&tempC);
                             byteArr.append(tempC);
                          }
                        }else{
                              for(uint i = 0; i < 5; i++){
                                   file.getChar(&tempC);
                                   byteArr.append(tempC);
                               }
                        }
                  }
             }
         }
    }
   tempChar.clear();
   tempChar = QString(byteArr);
}

ClientsFilesMenager& App::getClientsFilesMenager(){
    return clientsFilesMenager;
}

BooksFilesMenager& App::getBooksFilesMenager(){
    return booksFilesMenager;
}

unsigned long long App::strLenForFile(QString &str){
    unsigned long long counter = 0;
    QByteArray data = str.toUtf8();
    ushort i = 1;
    char tempChar = '\0';
    QTextStream textStr(data);
    while(!textStr.atEnd())
    {
        textStr.device()->getChar(&tempChar);
        if(tempChar > 0){
            if(tempChar == '\n')
                counter += 2;
            else
                counter++;
        }else{
        if(tempChar > -96){
            textStr.device()->getChar(&tempChar);
            i++;
        }else{
            if(tempChar > -112)
            {
                for( ; i < 4; i++)
                {
                    textStr.device()->getChar(&tempChar);
                }
            }else{
                if(tempChar > -120){
                    for( ; i < 5; i++){
                        textStr.device()->getChar(&tempChar);
                     }
                }else{
                    if(tempChar > -124){
                        for( ; i < 6; i++){
                            textStr.device()->getChar(&tempChar);
                        }
                    }else{
                        for( ; i < 7; i++){
                            textStr.device()->getChar(&tempChar);
                         }
                    }
                }
            }
        }
        counter += i;
        i = 1;
   }
 }
    return counter;
}

void App::checkActivityTimer(){
    if(!clientsFilesMenager.isFileOperation()){
        clientsFilesMenager.checkOrReduceActivity();
    }
}

bool App::eventFilter(QObject *obj, QEvent *ev){
    switch (ev->type()) {
    case QEvent::Timer:
        if(obj == &activityCheckTimer){
            activityCheckTimer.stop();
            checkActivityTimer();
            activityCheckTimer.start();
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(obj, ev);
}
