#include"app.hpp"
#include<QDebug>
#include<QFile>
#include<QByteArray>
#include<QTextCodec>

App::App(int argc, char** argv)
    : QCoreApplication (argc, argv), httpServer(this)/*, clientsFilesMenager(this)*/
{
    qDebug() << "Inicjalizacja serwera...\n";
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
}

App::~App(){
    qDebug() << "Kończenie pracy serwera ...";
}

void App::readCharUtf8(QFile &file, QString &tempChar){
    QByteArray byteArr = QByteArray();
    char tempC;
    file.getChar(&tempC);
    byteArr.append(tempC);
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
   tempChar.clear();
   tempChar = QString(byteArr);
}
