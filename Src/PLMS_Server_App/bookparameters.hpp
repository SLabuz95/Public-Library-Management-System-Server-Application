#ifndef BOOKPARAMETERS_HPP
#define BOOKPARAMETERS_HPP

// Macros Name Parameters
#define BOOK_PARAMETERS_BOOK_ID ("bookId")
#define BOOK_PARAMETERS_BOOK_STATUS ("status")
#define BOOK_PARAMETERS_BOOK_TITLE ("title")
#define BOOK_PARAMETERS_BOOK_PUBLISHER ("publisher")
#define BOOK_PARAMETERS_BOOK_EDITION ("edition")
#define BOOK_PARAMETERS_BOOK_COMMENTS ("comments")
#define BOOK_PARAMETERS_BOOK_USER_ID ("user")
#define BOOK_PARAMETERS_BOOK_COMMENTS_CONTENT ("comContent")
#define BOOK_PARAMETERS_BOOK_COMMENTS_USER_ID ("comUID")
#define BOOK_PARAMETERS_BOOK_END_PARAMETER_TOKEN ("end")

enum BookParameters{
    BOOK_ID,
    BOOK_STATUS,
    BOOK_TITLE,
    BOOK_PUBLISHER,
    BOOK_EDITION,
    BOOK_USER_ID,
    BOOK_COMMENTS,  // Its not file parameter
    BOOK_COMMENT_CONTENT,
    BOOK_COMMENT_USER_ID,
    BOOK_END_PARAMETER_TOKEN,   // Use to catch end of book read
    BOOK_NUMB_OF_PARAMETERS     // Use to catch errors
};

#endif // BOOKPARAMETERS_HPP
