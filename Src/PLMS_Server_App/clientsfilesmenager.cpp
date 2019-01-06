#include"clientsfilesmenager.hpp"
#include"app.hpp"
#include"readfilerules.hpp"
#include<QFile>
#include"user.hpp"
#include<QDebug>
#include"filereadingstatenum.hpp"

ClientsFilesMenager::ClientsFilesMenager(App* parent)
    : parent(parent)
{

}

ClientsFilesMenager::~ClientsFilesMenager(){
    clearMemory();
}

bool ClientsFilesMenager::createClientsFile(){
    QFile file(CLIENTS_FILE_NAME);
    if(file.exists())
        file.remove();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << QString("\n").toUtf8();
        file.close();
    }
    return true;
}

bool ClientsFilesMenager::createClientsFileBackUp(){
  if(QFile(CLIENTS_FILE_BACKUP_NAME).exists())
        QFile(CLIENTS_FILE_BACKUP_NAME).remove();
  if(!QFile::copy(CLIENTS_FILE_NAME, CLIENTS_FILE_BACKUP_NAME))
  {
      SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
      return false;
  }
   return true;
}

uint8_t ClientsFilesMenager::restoreClientsFile(){
    QFile clientsFile(CLIENTS_FILE_NAME);
    clientsFile.remove();
    clientsFile.setFileName(CLIENTS_FILE_BACKUP_NAME);
    if(!clientsFile.exists())
        return 1;   // No BackUp
    uint counter = 0;
    // Próba skopiowania backupu i zmiany nazwy
    while(!clientsFile.copy(CLIENTS_FILE_NAME)){
        if(counter < 100)
          counter++;
        else{
            SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
            return 2;   // Open File Error
          }
        }
    return 0;
}


void ClientsFilesMenager::clearMemory(){

}

UserParameters ClientsFilesMenager::checkUserParameters(QString &checkStr){
    QString tempStr;
    uint i = 0;
    switch(tempStr.length()){
    case 4:
    {
            // Check USER_NAME
            tempStr = USER_PARAMETERS_USER_NAME;
            for(i = 0; i < 4; i++)
                if(checkStr.at(i) != tempStr.at(i))
                    break;
            if(i == 4)  return USER_NAME;
            // Check ...

            // End Of Check for 4 Signs
    }
    break;
    case 7:
    {
        // Check USER_SURNAME
        tempStr = USER_PARAMETERS_USER_SURNAME;
        for(i = 0; i < 7; i++)
            if(checkStr.at(i) != tempStr.at(i))
                break;
        if(i == 7)  return USER_SURNAME;
        // Check ...

        // End Of Check for 7 Signs
    }
    break;
    case 8:
    {
        // Check USER_PASSWORD
        tempStr = USER_PARAMETERS_USER_PASSWORD;
        for(i = 0; i < 8; i++)
            if(checkStr.at(i) != tempStr.at(i))
                break;
        if(i == 8)  return USER_PASSWORD;
        // Check USER_ID
        tempStr = USER_PARAMETERS_USER_ID;
        for(i = 0; i < 8; i++)
            if(checkStr.at(i) != tempStr.at(i))
                break;
        if(i == 8)  return USER_ID;

        // End Of Check for 8 Signs
    }
    break;
    case 9:
    {
        // Check USER_FIRST_NAME
        tempStr = USER_PARAMETERS_USER_FIRST_NAME;
        for(i = 0; i < 9; i++)
            if(checkStr.at(i) != tempStr.at(i))
                break;
        if(i == 9)  return USER_FIRST_NAME;

        // End Of Check for 9 Signs
    }
    break;
    case 10:
    {
        // Check USER_SECOND_NAME
        tempStr = USER_PARAMETERS_USER_SECOND_NAME;
        for(i = 0; i < 10; i++)
            if(checkStr.at(i) != tempStr.at(i))
                break;
        if(i == 10)  return USER_SECOND_NAME;

        // End Of Check for 10 Signs
    }
    break;
    default:
    break;
    }
    return USER_NUMB_OF_PARAMETERS; // Parameter not Found
}

bool ClientsFilesMenager::readClientsFile(ReadFileRules& rules){
    QFile file(CLIENTS_FILE_NAME);
    if(!file.exists()){
        switch(restoreClientsFile()){
        case 1: // No BackUp
            if(!createClientsFile()) // Creating Error
                return false;
            break;
        case 2: // Open File Error
            return false;
        default:    // Restored File
            break;
        }
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        // File Reading Depends of Reading Rules
        SERVER_MSG("--- Client File Reading Start ---");
        User tempUser("Test");
        do{
            if(!readNextClient(tempUser, file))
            {
                file.close();
                SERVER_MSG("--- Client File Read Failed ---");
                return false; // READING ERROR
            }
            // Sprawdz czy użytkownik ma poprawne dane z pomocą funkcji (ale co z tym zrobić) (chyba przekazać informację o uszkodzonych danych)
        }while(!rules.checkRules(&tempUser));
        file.close();
        SERVER_MSG("--- Client File Read Success");
    }
}

bool ClientsFilesMenager::readNextClient(User &tempUser, QFile &file){
    QString tempChar;
    QString tempStr;
    FileReadingStat frs = FILE_READING_SEARCHING;
    UserParameters userParameter = USER_NUMB_OF_PARAMETERS;

    // Find next client
    bool stop = false;
    while(!stop){
        if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
            SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
            SERVER_MSG("Clients file end while reading clients parameters");
            /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
            return false;
        }
        App::readCharUtf8(file, tempChar);
        // Operation on readed character
        switch(frs){
            case FILE_READING_SEARCHING:
            {
            if(!(tempChar == '\n' || tempChar == '\r' || tempChar == ' ' || tempChar == '"')){
                frs = FILE_READING_READING_PARAM_NAME;
                tempStr.append(tempChar);
              }
            }
            break;
            case FILE_READING_READING_PARAM_NAME:
            {   // Read until equal sign or (file corrupted , internal server error, SERVER PAUSE WORK)
                if(tempChar == '\n' || tempChar == '\r' || tempChar == ' '){
                    SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                    SERVER_MSG("Clients file have corrupted parameter name");
                    /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                    return false;
                }
                if(tempChar == '='){
                    // Check parameter name
                    if((userParameter = checkUserParameters(tempStr)) == USER_NUMB_OF_PARAMETERS){
                        SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                        SERVER_MSG("Clients file have corrupted parameter name");
                        /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                    }
                    // ---------------------
                    frs = FILE_READING_READING_VALUE;
                    if(userParameter == USER_END_PARAMETER_TOKEN){
                        return true;
                    }
                    tempStr.clear();
                }else{
                    tempStr.append(tempChar);
                }
            }
            break;
            case FILE_READING_READING_VALUE:
            {
                // Check if speech marks, if not read as number (End with EndLineSign and check only digits)
                    if(tempChar == '"'){
                        do{
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Clients file end while reading clients parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
                            App::readCharUtf8(file, tempChar);
                            tempStr.append(tempChar);
                        }while(tempChar != '"');
                    }else{
                        do{
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Clients file end while reading clients parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
                            App::readCharUtf8(file, tempChar);
                            if((tempChar.at(0).toLatin1() < 48 || tempChar.at(0).toLatin1() > 57)){
                                if(tempChar != '\r'){
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Clients file number parameter have no digit sign in value");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                                }
                            }else{
                                tempStr.append(tempChar);
                            }
                        }while(tempChar != '\n');
                    }
                    tempUser.setParam(userParameter, tempStr);
                    tempStr.clear();
                    frs = FILE_READING_SEARCHING;
            }
            break;
            default:
            break;
        }
    }
}

