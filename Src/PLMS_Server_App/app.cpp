#include"app.hpp"
#include<QDebug>

App::App(int argc, char** argv)
    : QCoreApplication (argc, argv)
{
    qDebug() << "Inicjalizacja serwera...";
}

App::~App(){
    qDebug() << "Kończenie pracy serwera ...";
}

