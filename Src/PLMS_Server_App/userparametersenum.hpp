#ifndef USERPARAMETERSENUM_HPP
#define USERPARAMETERSENUM_HPP

// Macros Name Parameters
#define USER_PARAMETERS_USER_ID ("clientId")
#define USER_PARAMETERS_USER_NAME ("name")
#define USER_PARAMETERS_USER_PASSWORD ("password")
#define USER_PARAMETERS_USER_FIRST_NAME ("firstName")
#define USER_PARAMETERS_USER_SECOND_NAME ("secondName")
#define USER_PARAMETERS_USER_SURNAME ("surname")
#define USER_PARAMETERS_USER_PESEL ("pesel")
#define USER_PARAMETERS_USER_BOOK_ID ("userBookId")
#define USER_PARAMETERS_USER_PERMISSIONS ("userPerm")
#define USER_PARAMETERS_USER_END_PARAMETER_TOKEN ("end")

enum UserParameters{
    USER_ID,
    USER_NAME,
    USER_PASSWORD,
    USER_FIRST_NAME,
    USER_SECOND_NAME,
    USER_SURNAME,
    USER_PESEL,
    USER_BOOK_ID,
    USER_PERMISSIONS,
    USER_END_PARAMETER_TOKEN,   // Use to catch end of client read
    USER_NUMB_OF_PARAMETERS     // Use to catch errors
};

#endif // USERPARAMETERSENUM_HPP
