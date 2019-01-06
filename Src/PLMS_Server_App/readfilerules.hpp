#ifndef READFILERULES_HPP
#define READFILERULES_HPP
// ------------------ Macros --------------------------------------------
// Include macros

// ----------------------------------------------------------------------

// ------------------ Includes ------------------------------------------
#include"readfilerulestypeenum.hpp"
// ----------------------------------------------------------------------

// ------------------ Predefinitions ------------------------------------
class App;
class User;
class QJsonDocument;
// ----------------------------------------------------------------------

// ------------------ Read File Rules Class -----------------------------------------

class ReadFileRules{
public:
    // Constructor
    ReadFileRules(ReadFileRulesType type);
    ReadFileRules(QJsonDocument* jsonDoc);  // Need for initialization

    // Destructor
   ~ReadFileRules();

private:
    // Parent
    App* parent = nullptr;

    // Type of rules
    ReadFileRulesType type;

    // Filter Struct
    /* _PH_ Filter* filter = nullptr;*/


public:
    //  Is Rule Finished?
    bool checkRules(User*);

    // Get Functions
    App* getParent();
    ReadFileRulesType getReadFilesRulesType();
};


#endif // READFILERULES_HPP
