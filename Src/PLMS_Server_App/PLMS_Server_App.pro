QT -= gui
QT += network
CONFIG += c++11 console
CONFIG -= app_bundle
TARGET = "Aplikacja biblioteki publicznej"
VERSION_PE_HEADER = 1.0
SOURCES += \
    main.cpp \
    app.cpp \
    httpserver.cpp \
    clientsfilesmenager.cpp \
    user.cpp \
    readfilerules.cpp \
    mytcpsocket.cpp

HEADERS += \
    app.hpp \
    httpserver.hpp \
    clientsfilesmenager.hpp \
    readfilerules.hpp \
    user.hpp \
    filereadingstatenum.hpp \
    userparametersenum.hpp \
    requestdecodestat.hpp \
    messagetype.hpp \
    mytcpsocket.hpp \
    returnerrortype.hpp \
    tcpsocketstat.hpp \
    commandtype.hpp \
    filetypeenum.hpp \
    bookparameters.hpp
