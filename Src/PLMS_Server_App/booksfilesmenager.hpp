#ifndef BOOKSFILESMENAGER_HPP
#define BOOKSFILESMENAGER_HPP

// ------------------ Macros --------------------------------------------
#define BOOKS_FILE_NAME (QString("books"))
#define BOOKS_FILE_BACKUP_NAME (QString("booksBU"))
#define TEMP_FILE_NAME (QString("tempB"))
#define BOOKS_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"books\" !!! -------------"))
#define TEMP_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"tempB\" !!! -------------"))

// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<stdint.h>
#include"bookparameters.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class Book;
class QFile;
class ReadFileRules;
class QString;
class MyTcpSocket;
// ----------------------------------------------------------------------

// ------------------ BooksFilesMenager Class -----------------------------------------

class BooksFilesMenager{

public:
    // Constructor
    BooksFilesMenager(App*);

    // Destructor
    ~BooksFilesMenager();

private:

    // Parent
    App* parent = nullptr;  // DONT DELETE

    // Actual TcpSocket
    MyTcpSocket* actualSocket = nullptr;    // DONT DELETE

    bool createBooksFile();
    bool createBooksFileBackUp();
    uint8_t restoreBooksFile();
    void clearMemory();
    bool readNextBook(Book&, QFile&);
    bool writeNextBook(Book&, QFile&);
    BookParameters checkBookParameters(QString&);

public:
    void addEditRemoveBook(MyTcpSocket* newActualSocket);
    void readBooks(MyTcpSocket* newActualSocket);

private:
    bool readBooksFile(ReadFileRules& rules);
    bool writeBooksFile();

};

#endif // BOOKSFILESMENAGER_HPP
