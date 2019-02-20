#ifndef BOOKLOGSFILESMENAGER_HPP
#define BOOKLOGSFILESMENAGER_HPP


// ------------------ Macros --------------------------------------------
#define BOOK_LOGS_FILE_NAME (QString("bookLogs"))
#define BOOK_LOGS_FILE_BACKUP_NAME (QString("bookLogsBU"))
#define TEMP_FILE_NAME (QString("tempBL"))
#define BOOK_LOGS_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"bookLogs\" !!! -------------"))
#define TEMP_FILE_OPEN_ERROR_TEXT (QString("-----------!!! Błąd otwarcia pliku \"tempBL\" !!! -------------"))

// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<stdint.h>
#include"booklogparameters.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class BookLog;
class QFile;
class ReadFileRules;
class QString;
class MyTcpSocket;
// ----------------------------------------------------------------------

// ------------------ BooksFilesMenager Class -----------------------------------------
class BookLogsFilesMenager{

public:
    // Constructor
    BookLogsFilesMenager(App*);

    // Destructor
    ~BookLogsFilesMenager();

private:

    // Parent
    App* parent = nullptr;  // DONT DELETE

    // Actual TcpSocket
    MyTcpSocket* actualSocket = nullptr;    // DONT DELETE

    bool createBookLogsFile();
    bool createBookLogsFileBackUp();
    uint8_t restoreBookLogsFile();
    void clearMemory();
    bool readNextBookLog(BookLog&, QFile&);
    bool writeNextBookLog(BookLog&, QFile&);
    BookLogParameters checkBookLogParameters(QString&);

public:
    void addBookLog(MyTcpSocket* newActualSocket, BookLog&);
    void readBookLogs(MyTcpSocket* newActualSocket);

private:
    bool readBookLogsFile(ReadFileRules& rules);
    bool writeBookLogsFile(BookLog&);

};

#endif // BOOKLOGSFILESMENAGER_HPP
