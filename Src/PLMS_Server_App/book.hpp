#ifndef BOOK_HPP
#define BOOK_HPP

// ------------------ Macros --------------------------------------------
#define BOOK_JSON_KEY_TEXT ("book")
#define WRITE_PARAM_TO_FILE(bookO, paramN) (bookO.getStrForFile(paramN).toUtf8())
#define BOOK_DATE_TIME_FORMAT ("dd.MM.yyyy")
// Memory Pointer Macros ------------------------------------------------------------------------------
#define SET_PTR_DO(ptr, newPtr) {if(ptr) {delete ptr; ptr = nullptr;} ptr = newPtr;}
#define SET_PTR_DOA(ptrA, newPtrA) {if(ptrA) {delete []ptrA; ptrA = nullptr;}; ptrA = newPtrA;}
#define SET_PTR_NDO(ptr, newPtr) {ptr = newPtr;}
// Include macros
//#include "mainmacros.hpp"

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include<QString>
#include"bookparameters.hpp"
#include"bookstatus.hpp"
#include"booktype.hpp"
#include<QDateTime>

// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class QJsonObject;
class QJsonArray;
struct BookComment;
// ----------------------------------------------------------------------

// ------------------ Book Class ------------------------------------

class Book{
public:
    // Constructor
    Book();
    Book(QJsonObject);

    // Destructor
    ~Book();

private:
    // Book Data (InAccessible)
    unsigned long long bookId = 0;   // if 0, unidentified  *
    // Book Status
    BookStatus bookStatus = NUMB_OF_BOOK_STATUS;   //*
    // Book Title
    QString bookTitle;   //*
    // Book Publisher
    QString bookPublisher;  //*
    // Book Edition
    QString bookEdition; //*
    // Book Type
    BookType bookType = NUMB_OF_BOOK_TYPES;
    // Book Author
    QString author;
    // Book User ID
    unsigned long long userId = 0; //(Optional check)
    // Book Comments
    BookComment* bookComments = nullptr;    //*
    // Book Numb Of Comments
    uint numbOfBookComments = 0;

    QDateTime date;

    bool jsonReadCommentsError = false;

public:

    // Check book have all required data (function for file)
    bool checkBookFromFile();

    // Get Functions
    unsigned long long getBookId();
    BookStatus getBookStatus();
    BookComment* getBookComments();
    BookComment getBookCommentById(unsigned long long);
    uint getNumbOfBookComments();
    QDateTime getDate();
    BookType getBookType();
    // Set Book Id
    void setBookId(unsigned long long newBookId);

    // Set Param by Name with QString and Enumerator
    void setParam(BookParameters bookParam, QString value);

    // Get string Value  of Param by Name
    QString getParam(BookParameters bookParam);

    // Prepare string of comments for file
    QString prepareCommentsPack();

    // Get String for File
    QString getStrForFile(BookParameters bookParam);

    // Check Book parameters (dont check bookId)
    bool checkBookParameters();

    // Check Book Comments parameters
    bool checkBookComments();

    // Read Comments from Json
    bool readCommentsFromJson(QJsonArray&);

    bool readJson(QJsonObject&);
    void writeJson(QJsonObject&);

    void addComment(unsigned long long userId);
    void addComment(unsigned long long userId, QString content);
    void removeComment(unsigned long long userId);

    unsigned long long getFileDataStrLength();

    void merge(Book&);
    void mergeBookComments(Book&);
};

#endif // BOOK_HPP
