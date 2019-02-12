#include"clientsfilesmenager.hpp"
#include"app.hpp"
#include"readfilerules.hpp"
#include<QFile>
#include"user.hpp"
#include<QDebug>
#include"filereadingstatenum.hpp"
#include<QJsonObject>
#include"filetypeenum.hpp"
#include"mytcpsocket.hpp"
#include<QTextStream>
#include<QJsonArray>
#include<QTextCodec>

ClientsFilesMenager::ClientsFilesMenager(App* parent)
    : parent(parent)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    if(!init()){
        SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
        SERVER_MSG("Clients file initialization error");
        /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
    }
    allocUserFastAccess = false;
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
        User admin;
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        admin.setUserId(1);
        admin.setParam(USER_NAME, "admin");
        admin.setParam(USER_PASSWORD, "123");
        admin.setParam(USER_SURNAME, "Admin");
        admin.setParam(USER_PESEL, "00000000000");
        admin.setParam(USER_FIRST_NAME, QString("Domyślny").toUtf8());
        admin.setParam(USER_PERMISSIONS, QString::number(USER_PERMISSIONS_ADMIN));
        writeNextClient(admin, file);
        stream << (QString(USER_PARAMETERS_USER_ID) + QString("=0\nend=\n")).toUtf8();
        file.close();
    }
    SERVER_MSG(QString("Clients File Created"));
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
  SERVER_MSG(QString("Clients File BackUp Created"));
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
    SET_PTR_DOA(userFastAccess, nullptr);
    SET_PTR_DOA(loggedUsers, nullptr);
}

UserParameters ClientsFilesMenager::checkUserParameters(QString &checkStr){
    CHECK_PARAM_INIT;
    switch(checkStr.length()){
    case 3:
    {
        // Check END_TOKEN
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_END_PARAMETER_TOKEN, 3, USER_END_PARAMETER_TOKEN);
        // Check ...

        // End of Check for 3 Signs
    }
        break;
    case 4:
    {
        // Check USER_NAME
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_NAME, 4,USER_NAME);
        // Check ...

        // End Of Check for 4 Signs
    }
    break;
    case 5:
    {
        // Check USER_PESEL
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_PESEL, 5, USER_PESEL);
        // Check ...

        // End Of Check for 5 Signs
    }
        break;
    case 7:
    {
        // Check USER_SURNAME
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_SURNAME, 7, USER_SURNAME);
        // Check ...

        // End Of Check for 7 Signs
    }
    break;
    case 8:
    {
        // Check USER_PASSWORD
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_PASSWORD, 8, USER_PASSWORD);
        // Check USER_ID
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_ID, 8, USER_ID);
        // Check USER_PERMISSIONS
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_PERMISSIONS, 8, USER_PERMISSIONS);
        // Check ...

        // End Of Check for 8 Signs
    }
    break;
    case 9:
    {
        // Check USER_FIRST_NAME
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_FIRST_NAME, 9, USER_FIRST_NAME);
        // Check ...

        // End Of Check for 9 Signs
    }
    break;
    case 10:
    {
        // Check USER_SECOND_NAME
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_SECOND_NAME, 10, USER_SECOND_NAME);
        // Check USER_BOOK_ID
        CHECK_PARAM_RETURN_V(checkStr, USER_PARAMETERS_USER_BOOK_ID, 10, USER_BOOK_ID);
        // Check ...

        // End Of Check for 10 Signs
    }
    break;
    default:
    break;
    }
    return USER_NUMB_OF_PARAMETERS; // Parameter not Found
}

bool ClientsFilesMenager::readClientsFile(ReadFileRules& rules){
    filePos = getFilePos(rules.getStartIdPoint());// Reset File Position Pointer
    unsigned long long idCounter = 0;
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
        file.seek(filePos);
        User tempUser;
        do{
            if(!readNextClient(tempUser, file))
            {
                file.close();
                SERVER_MSG("--- Client File Read Failed ---");
                return false; // READING ERROR
            }
            if(!tempUser.checkUserFromFile()){
                // Fail of user check (Depends of command type)
                file.close();
                SERVER_MSG("--- Client File Read Failed ---");
                return false;
            }
            idCounter++;
            if(allocUserFastAccess && idCounter % USER_FAST_ACCESS_STEP == 1 && tempUser.getUserId() != 0)
                insertFastClient(static_cast<unsigned int>(idCounter / USER_FAST_ACCESS_STEP), tempUser.getUserId(), filePos);
            filePos += tempUser.getFileDataStrLength();
        }while(rules.check(tempUser, actualSocket));
        file.close();
        SERVER_MSG("--- Client File Read Success");
    }
}

bool ClientsFilesMenager::readNextClient(User &tempUser, QFile &file){
    QString tempChar;
    QString tempStr;
    FileReadingStat frs = FILE_READING_SEARCHING;
    UserParameters userParameter = USER_NUMB_OF_PARAMETERS;
    tempUser = User();
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
                        return false;
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
                        App::readCharUtf8(file, tempChar);
                        while(tempChar != '"'){
                        tempStr.append(tempChar);
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Clients file end while reading clients parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
                            App::readCharUtf8(file, tempChar);                            
                        }
                    }else{
                        do{
                            if(file.atEnd()){ // CANT BE AT END IN THIS FUNCTION (FILE IS CORRUPTED, SERVER INTERNAL ERROR, SERVER PAUSE WORK)
                                SERVER_MSG("___CRITICAL INTERNAL SERVER ERROR___");
                                SERVER_MSG("Clients file end while reading clients parameters");
                                /* _PH_ Set internal server error in parent for pause server. Remember to send errors for all active threads*/
                                return false;
                            }
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
                            App::readCharUtf8(file, tempChar);
                        }while(tempChar != '\n');
                    }
                    tempUser.setParam(userParameter, tempStr);
                    tempStr.clear();
                    frs = FILE_READING_SEARCHING;
            }
            break;
         }
    }
}

void ClientsFilesMenager::addEditRemoveClient(MyTcpSocket* newActualSocket){
    actualSocket = newActualSocket;
    fileOperation = true;
    reallocFastClients(nullptr, 0);
    if(!writeClientsFile()){
        actualSocket->setReturnErrorType(RETURN_ERROR_FILE_ERROR);
    }
    fileOperation = false;
}

void ClientsFilesMenager::addEditRemoveClient(MyTcpSocket* newActualSocket, User& user){
    actualSocket = newActualSocket;
    fileOperation = true;
    reallocFastClients(nullptr, 0);
    if(!writeClientsFile(user)){
        actualSocket->setReturnErrorType(RETURN_ERROR_FILE_ERROR);
    }
    fileOperation = false;
}

void ClientsFilesMenager::readClients(MyTcpSocket* newActualSocket){
    actualSocket = newActualSocket;
    QJsonObject jsonObj = actualSocket->requestData.value(READ_FILE_RULES_JSON_KEY_TEXT).toObject();
    ReadFileRules rfr(jsonObj, parent);
    if(!rfr.isConstructingError()){
        fileOperation = true;
        readClientsFile(rfr);
        fileOperation = false;
    }else {
        newActualSocket->setReturnErrorType(RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED);
    }
}

void ClientsFilesMenager::readClients(MyTcpSocket* newActualSocket, ReadFileRules& rfr){
    actualSocket = newActualSocket;
    if(!rfr.isConstructingError()){
        fileOperation = true;
        readClientsFile(rfr);
        fileOperation = false;
    }else{
        newActualSocket->setReturnErrorType(RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED);
    }
}

void ClientsFilesMenager::loginClient(MyTcpSocket *newActualSocket){
    actualSocket = newActualSocket;
    User user(actualSocket->requestData.value(USER_JSON_KEY_TEXT).toArray().at(0).toObject());
    if(user.getParam(USER_NAME).isEmpty() || user.getParam(USER_PASSWORD).isEmpty())
    {
        actualSocket->setReturnErrorType(RETURN_ERROR_USER_JSON_CORRUPTED);
    }else{
        QJsonArray filterArr;
        QJsonObject filterObj;        
        filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_NAME));
        filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, user.getParam(USER_NAME));
        filterArr.append(filterObj);
        filterObj = QJsonObject();
        filterObj.insert(READ_FILE_RULES_FILTER_PARAM_TEXT, QString::number(USER_PASSWORD));
        filterObj.insert(READ_FILE_RULES_FILTER_VALUE_TEXT, user.getParam(USER_PASSWORD));
        filterArr.append(filterObj);
        filterObj = QJsonObject();
        filterObj.insert(READ_FILE_RULES_FILE_TYPE_TEXT, QString::number(FILE_TYPE_CLIENTS_FILE));
        filterObj.insert(READ_FILE_RULES_MAX_READ_TEXT, QString("1"));
        filterObj.insert(READ_FILE_RULES_FULL_FILTER, QString::number(1));
        filterObj.insert(READ_FILE_RULES_FILTER_TEXT, filterArr);
        ReadFileRules rfr(filterObj, parent);
        if(!rfr.isConstructingError()){
            fileOperation = true;
            readClientsFile(rfr);
            fileOperation = false;
        }else {
            newActualSocket->setReturnErrorType(RETURN_ERROR_READ_FILES_RULES_JSON_CORRUPTED);
        }
    }
}

void ClientsFilesMenager::logoutClient(MyTcpSocket *newActualSocket){
    actualSocket = newActualSocket;
    QJsonArray jA = actualSocket->requestData.value(USER_PARAMETERS_USER_ID).toArray();
    uint jALen = jA.count();
    fileOperation = true;
    for(uint i = 0; i < jALen; i++){
        if(jA.at(i).toString().toULongLong() != 0){
            removeFastLoggedClient(jA.at(i).toString().toULongLong());
        }
    }
    fileOperation = false;
}

void ClientsFilesMenager::extendActivity(MyTcpSocket *newActualSocket){
    actualSocket = newActualSocket;
    QJsonArray jsonA = actualSocket->requestData.value(USER_PARAMETERS_USER_ID).toArray();
    QJsonArray retJsonA;
    uint aLen = jsonA.count();
    for(uint j = 0; j < aLen; j++){
        unsigned long long tempId = jsonA.at(j).toString().toULongLong();
        for(uint i = 0;  i < numbOfLoggedUsers; i++){
            if((*(loggedUsers + i)).id == tempId){
                retJsonA.append(QString::number(tempId));
                (*(loggedUsers + i)).activity = MAX_ACTIVITY_VALUE;
                break;
            }
        }
    }
    actualSocket->returnData.insert(USER_PARAMETERS_USER_ID, retJsonA);
}

bool ClientsFilesMenager::writeClientsFile(){
    if(!createClientsFileBackUp()){
        actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
        SERVER_MSG("--- Client File Read Failed ---");
        return true;
    }
    // Reading Rules for Write File
    ReadFileRules readFileRules(FILE_TYPE_CLIENTS_FILE, parent);
    filePos = 0;    // Reset File Position Pointer
    unsigned long long idCounter = 0;
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
    bool userFound = false;
    QFile newFile(TEMP_FILE_NAME);
    newFile.remove();
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        SERVER_MSG("--- Temp File Writing Start ---");
        if(!newFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            SERVER_MSG(TEMP_FILE_OPEN_ERROR_TEXT);
            file.close();
            SERVER_MSG("--- Client File Closed");
            return false;
        }else{
            User tempUser;
            User requestUser(actualSocket->requestData.value(USER_JSON_KEY_TEXT).toArray().at(0).toObject());
            bool addClient = (requestUser.getUserId() == 0);
            unsigned long long lastId = 0;
            do{
                if(!readNextClient(tempUser, file))
                {
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Client File Read Failed ---");
                    return false; // READING ERROR
                }
                if(!tempUser.checkUserFromFile()){
                    // Fail of user check (Depends of command type)
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Client File Read Failed ---");
                    return false;
                }
                // Fast Access Reallocation --------------
                // UserFastAccess Reallocation
                idCounter++;
                if(idCounter % USER_FAST_ACCESS_STEP == 1 && tempUser.getUserId() != 0)
                    insertFastClient(static_cast<unsigned int>(idCounter / USER_FAST_ACCESS_STEP), tempUser.getUserId(), filePos);

                // LoggedUser Reallocation
                for(uint i = 0 ; i < numbOfLoggedUsers; i++){
                    if(tempUser.getUserId() != 0 && (*(loggedUsers + i)).id == tempUser.getUserId()){
                        insertFastLoggedClient(tempUser.getUserId(), filePos);
                    }
                }
                filePos += tempUser.getFileDataStrLength();
                // ------------------------------------------

                if(addClient){
                    // Check if readed from file user have the same PESEL number as requestUser
                    if(requestUser.getParam(USER_PESEL) == tempUser.getParam(USER_PESEL)){
                        SERVER_MSG("There is a user with the same PESEL name");
                        file.close();
                        newFile.close();
                        actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                        SERVER_MSG("--- Client File Read Failed ---");
                        return true;
                    }
                    // Check if readed from file user have the same user name as requestUser
                    if(requestUser.getParam(USER_NAME) == tempUser.getParam(USER_NAME)){
                        SERVER_MSG("There is a user with the same user name");
                        file.close();
                        newFile.close();
                        actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                        SERVER_MSG("--- Client File Read Failed ---");
                        return true;
                    }
                    if(!userFound){
                        if(lastId + 1 < tempUser.getUserId()){
                            requestUser.setUserId(lastId + 1);
                            writeNextClient(requestUser, newFile);
                            actualSocket->processReadedUserFromFile(requestUser);
                            userFound = true;
                        }else{
                            if(tempUser.getUserId() == 0){
                                requestUser.setUserId(lastId + 1);
                                writeNextClient(requestUser, newFile);
                                actualSocket->processReadedUserFromFile(requestUser);
                                userFound = true;
                            }
                        }
                    }
                    lastId = tempUser.getUserId();
                    writeNextClient(tempUser, newFile);
                }else{
                    switch(actualSocket->getCmdType()){
                    case COMMAND_TYPE_CLIENT_EDIT:
                        SERVER_MSG(requestUser.getParam(USER_PESEL));
                        SERVER_MSG(tempUser.getParam(USER_PESEL));
                        if(tempUser.getUserId() != 0 && requestUser.getUserId() != tempUser.getUserId() && requestUser.getParam(USER_PESEL) == tempUser.getParam(USER_PESEL)){
                            // There is a user with the same pesel number
                            SERVER_MSG("There is a user with the same PESEL name");
                            file.close();
                            newFile.close();
                            actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                            SERVER_MSG("--- Client File Read Failed ---");
                            return true;
                        }
                        // Check if readed from file user have the same user name as requestUser
                        if(tempUser.getUserId() != 0 && requestUser.getUserId() != tempUser.getUserId() && requestUser.getParam(USER_NAME) == tempUser.getParam(USER_NAME)){
                            SERVER_MSG("There is a user with the same user name");
                            file.close();
                            newFile.close();
                            actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                            SERVER_MSG("--- Client File Read Failed ---");
                            return true;
                        }
                        if(requestUser.getUserId() == tempUser.getUserId()){
                            if(userFound){
                                file.close();
                                newFile.close();
                                actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                                SERVER_MSG("--- Client File Read Failed ---");
                                return true;
                            }
                            tempUser.merge(requestUser);
                            writeNextClient(tempUser, newFile);
                            userFound = true;    // For Information Purpose (Catch Not Found User Error)
                        }else {
                            writeNextClient(tempUser, newFile);
                        }                    
                        break;
                    case COMMAND_TYPE_CLIENT_REMOVE:
                    if(tempUser.getUserId() != 0 && requestUser.getUserId() == tempUser.getUserId() && requestUser.getUserId() != 0){
                            actualSocket->getBookLog()->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_REMOVE_USER));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_PESEL, tempUser.getParam(USER_PESEL));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_SURNAME, tempUser.getParam(USER_SURNAME));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_FIRST_NAME, tempUser.getParam(USER_FIRST_NAME));
                            requestUser.setParam(USER_ID, QString("0"));
                            userFound = true;// For Information Purpose (Catch Not Found User Error)
                     }else{
                            writeNextClient(tempUser, newFile);
                     }
                        break;
                    default:
                        break;
                    }
                }

            }while(readFileRules.check(tempUser, actualSocket));
            newFile.close();
            if(!userFound){
                actualSocket->setReturnErrorType(RETURN_ERROR_USER_NOT_FOUND);
                file.close();
                SERVER_MSG("--- Client File Read Failed ---");
                return true;
            }
        }
        file.close();
        if(!file.remove()){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Client File Read Failed ---");
            return true;
        }
        if(!newFile.rename(CLIENTS_FILE_NAME)){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Client File Read Failed ---");
            return true;
        }
        SERVER_MSG("--- Temp File Write Success");
    }
    return true;
}

bool ClientsFilesMenager::writeClientsFile(User& requestUser){
    if(!createClientsFileBackUp()){
        actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
        SERVER_MSG("--- Client File Read Failed ---");
        return true;
    }
    // Reading Rules for Write File
    ReadFileRules readFileRules(FILE_TYPE_CLIENTS_FILE, parent);
    filePos = 0;    // Reset File Position Pointer
    unsigned long long idCounter = 0;
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
    bool userFound = false;
    QFile newFile(TEMP_FILE_NAME);
    newFile.remove();
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){  // File Not Open Error
        SERVER_MSG(CLIENTS_FILE_OPEN_ERROR_TEXT);
        return false;
    }else{
        SERVER_MSG("--- Temp File Writing Start ---");
        if(!newFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            SERVER_MSG(TEMP_FILE_OPEN_ERROR_TEXT);
            file.close();
            SERVER_MSG("--- Client File Closed");
            return false;
        }else{
            User tempUser;
            bool addClient = (requestUser.getUserId() == 0);
            unsigned long long lastId = 0;
            do{
                if(!readNextClient(tempUser, file))
                {
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Client File Read Failed ---");
                    return false; // READING ERROR
                }
                if(!tempUser.checkUserFromFile()){
                    // Fail of user check (Depends of command type)
                    file.close();
                    newFile.close();
                    SERVER_MSG("--- Client File Read Failed ---");
                    return false;
                }
                if(tempUser.getUserId() == 0){
                    writeNextClient(tempUser, newFile);
                    break;
                }
                // Fast Access Reallocation --------------
                // UserFastAccess Reallocation
                idCounter++;
                if(idCounter % USER_FAST_ACCESS_STEP == 1 && tempUser.getUserId() != 0)
                    insertFastClient(static_cast<unsigned int>(idCounter / USER_FAST_ACCESS_STEP), tempUser.getUserId(), filePos);

                // LoggedUser Reallocation
                for(uint i = 0 ; i < numbOfLoggedUsers; i++){
                    if((*(loggedUsers + i)).id == tempUser.getUserId()){
                        insertFastLoggedClient(tempUser.getUserId(), filePos);
                    }
                }
                filePos += tempUser.getFileDataStrLength();
                // ------------------------------------------

                if(addClient){
                    // Check if readed from file user have the same PESEL number as requestUser
                    if(requestUser.getParam(USER_PESEL) == tempUser.getParam(USER_PESEL)){
                        SERVER_MSG("There is a user with the same PESEL name");
                        file.close();
                        newFile.close();
                        actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                        SERVER_MSG("--- Client File Read Failed ---");
                        return true;
                    }
                    // Check if readed from file user have the same user name as requestUser
                    if(requestUser.getParam(USER_NAME) == tempUser.getParam(USER_NAME)){
                        SERVER_MSG("There is a user with the same user name");
                        file.close();
                        newFile.close();
                        actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                        SERVER_MSG("--- Client File Read Failed ---");
                        return true;
                    }
                    if(!userFound){
                        if(lastId + 1 < tempUser.getUserId()){
                            requestUser.setUserId(lastId + 1);
                            writeNextClient(requestUser, newFile);
                            actualSocket->processReadedUserFromFile(requestUser);
                            userFound = true;
                        }else{
                            if(tempUser.getUserId() == 0){
                                requestUser.setUserId(lastId + 1);
                                writeNextClient(requestUser, newFile);
                                actualSocket->processReadedUserFromFile(requestUser);
                                userFound = true;
                            }
                        }
                    }
                    lastId = tempUser.getUserId();
                    writeNextClient(tempUser, newFile);
                }else{
                    switch(actualSocket->getCmdType()){
                    case COMMAND_TYPE_BOOK_RETURN:
                    case COMMAND_TYPE_BOOK_CHECKOUT:
                    case COMMAND_TYPE_BOOK_RESERVE:
                    case COMMAND_TYPE_CLIENT_EDIT:
                        SERVER_MSG(requestUser.getParam(USER_PESEL));
                        SERVER_MSG(tempUser.getParam(USER_PESEL));
                        if(tempUser.getUserId() != 0 && requestUser.getUserId() != tempUser.getUserId() && requestUser.getParam(USER_PESEL) == tempUser.getParam(USER_PESEL)){
                            // There is a user with the same pesel number
                            SERVER_MSG("There is a user with the same PESEL name");
                            file.close();
                            newFile.close();
                            actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                            SERVER_MSG("--- Client File Read Failed ---");
                            return true;
                        }
                        // Check if readed from file user have the same user name as requestUser
                        if(tempUser.getUserId() != 0 && requestUser.getUserId() != tempUser.getUserId() && requestUser.getParam(USER_NAME) == tempUser.getParam(USER_NAME)){
                            SERVER_MSG("There is a user with the same user name");
                            file.close();
                            newFile.close();
                            actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                            SERVER_MSG("--- Client File Read Failed ---");
                            return true;
                        }
                        if(requestUser.getUserId() == tempUser.getUserId()){
                            if(userFound){
                                file.close();
                                newFile.close();
                                actualSocket->setReturnErrorType(RETURN_ERROR_USER_FOUND);
                                SERVER_MSG("--- Client File Read Failed ---");
                                return true;
                            }
                            if(actualSocket->getCmdType() == COMMAND_TYPE_BOOK_RETURN || actualSocket->getCmdType() == COMMAND_TYPE_BOOK_RESERVE || actualSocket->getCmdType() == COMMAND_TYPE_BOOK_CHECKOUT){
                                actualSocket->getBookLog()->setParam(BOOK_LOG_USER_PESEL, tempUser.getParam(USER_PESEL));
                                actualSocket->getBookLog()->setParam(BOOK_LOG_USER_SURNAME, tempUser.getParam(USER_SURNAME));
                                actualSocket->getBookLog()->setParam(BOOK_LOG_USER_FIRST_NAME, tempUser.getParam(USER_FIRST_NAME));
                                actualSocket->getBookLog()->setParam(BOOK_LOG_USER_PERMISSIONS, tempUser.getParam(USER_PERMISSIONS));
                            }
                            tempUser.merge(requestUser);
                            writeNextClient(tempUser, newFile);
                            userFound = true;    // For Information Purpose (Catch Not Found User Error)
                        }else {
                            writeNextClient(tempUser, newFile);
                        }
                        break;
                    case COMMAND_TYPE_CLIENT_REMOVE:
                    if(requestUser.getUserId() == tempUser.getUserId() && requestUser.getUserId() != 0){
                            actualSocket->getBookLog()->setParam(BOOK_LOG_ACTION, QString::number(BOOK_LOG_ACTION_REMOVE_USER));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_PESEL, tempUser.getParam(USER_PESEL));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_SURNAME, tempUser.getParam(USER_SURNAME));
                            actualSocket->getBookLog()->setParam(BOOK_LOG_USER_FIRST_NAME, tempUser.getParam(USER_FIRST_NAME));
                            requestUser.setParam(USER_ID, QString("0"));
                            userFound = true;// For Information Purpose (Catch Not Found User Error)
                     }else{
                            writeNextClient(tempUser, newFile);
                     }
                        break;
                    default:
                        break;
                    }
                }
            }while(readFileRules.check(tempUser, actualSocket));
            newFile.close();
            if(!userFound){
                actualSocket->setReturnErrorType(RETURN_ERROR_USER_NOT_FOUND);
                file.close();
                SERVER_MSG("--- Client File Read Failed ---");
                return true;
            }
        }
        file.close();
        if(!file.remove()){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Client File Read Failed ---");
            return true;
        }
        if(!newFile.rename(CLIENTS_FILE_NAME)){
            actualSocket->setReturnErrorType(RETURN_ERROR_FILE_NOT_REMOVED);
            SERVER_MSG("--- Client File Read Failed ---");
            return true;
        }
        SERVER_MSG("--- Temp File Write Success");
    }
    return true;
}

bool ClientsFilesMenager::writeNextClient(User &user, QFile &file){
    QTextStream s(&file);
    if(user.getUserId() == 0){
        s << WRITE_PARAM_TO_FILE(user, USER_ID); //*
        // END (DONT MODIFY)
        s << WRITE_PARAM_TO_FILE(user, USER_END_PARAMETER_TOKEN);
        return  true;
    }
    s << WRITE_PARAM_TO_FILE(user, USER_ID); //*
    s << WRITE_PARAM_TO_FILE(user, USER_NAME); //*
    s << WRITE_PARAM_TO_FILE(user, USER_PASSWORD); //*
    s << WRITE_PARAM_TO_FILE(user, USER_PESEL); //*
    s << WRITE_PARAM_TO_FILE(user, USER_FIRST_NAME); //*
    if(!user.getParam(USER_SECOND_NAME).isEmpty())
        s << WRITE_PARAM_TO_FILE(user, USER_SECOND_NAME);
    s << WRITE_PARAM_TO_FILE(user, USER_SURNAME);
    s << WRITE_PARAM_TO_FILE(user, USER_PERMISSIONS);
    s << WRITE_PARAM_TO_FILE(user, USER_BOOK_ID);
    // END (DONT MODIFY)
    s << WRITE_PARAM_TO_FILE(user, USER_END_PARAMETER_TOKEN);

    return true;
}

void ClientsFilesMenager::reallocFastClients(UserFastAccess* newArr, unsigned int size){
    SET_PTR_DOA(userFastAccess, newArr);
    numbOfUserFastAccess = size;
}

void ClientsFilesMenager::insertFastClient(unsigned int index, unsigned long long userId, unsigned long long filePos){
    if(index < numbOfUserFastAccess){
        (*(userFastAccess + index)).id = userId;
        (*(userFastAccess + index)).filePosition = filePos;
    }else {
        UserFastAccess* tempPtr = new UserFastAccess[index + 1];
        for(unsigned int i = 0; i < numbOfUserFastAccess; i++)
            (*(tempPtr + i)) = (*(userFastAccess + i));
        (*(tempPtr + index)).id = userId;
        (*(tempPtr + index)).filePosition = filePos;
        SET_PTR_DOA(userFastAccess, tempPtr);
        numbOfUserFastAccess = index + 1;
    }
}

bool ClientsFilesMenager::init(){
    ReadFileRules rfr(FILE_TYPE_CLIENTS_FILE, parent);
    allocUserFastAccess = true;
    fileOperation = true;
    actualSocket = new MyTcpSocket(new QTcpSocket(), parent);   // Fake Session To Avoid Memory Destruction
    reallocFastClients(nullptr, 0);
    bool ret = readClientsFile(rfr);
    SET_PTR_DO(actualSocket, nullptr);
    fileOperation = false;
    return ret;
}

void ClientsFilesMenager::insertFastLoggedClient(unsigned long long userId, unsigned long long filePos){
    for(uint i = 0;  i < numbOfLoggedUsers; i++){
        if((*(loggedUsers + i)).id == userId){
            (*(loggedUsers + i)).filePosition = filePos;
            return;
        }
    }
    UserLoggedFastAccess* tempPtr = new UserLoggedFastAccess[numbOfLoggedUsers + 1];
    for(unsigned int i = 0; i < numbOfLoggedUsers; i++)
        (*(tempPtr + i)) = (*(loggedUsers + i));
    (*(tempPtr + numbOfLoggedUsers)).id = userId;
    (*(tempPtr + numbOfLoggedUsers)).filePosition = filePos;
    SET_PTR_DOA(loggedUsers, tempPtr);
    numbOfLoggedUsers++;
    qDebug() << numbOfLoggedUsers;
}

void ClientsFilesMenager::removeFastLoggedClient(unsigned long long userId){
    ushort remove = 0;
    if(numbOfLoggedUsers == 0)
        return;
    if(numbOfLoggedUsers > 1){
        UserLoggedFastAccess* temp = new UserLoggedFastAccess[numbOfLoggedUsers - 1];        
        for(unsigned int i = 0; i < numbOfLoggedUsers; i++){
            if((*(loggedUsers + i)).id != userId)
                (*(temp + i - remove)) = (*(loggedUsers + i));
            else
                remove++;
        }
        SET_PTR_DOA(loggedUsers, temp);
    }else{
        if((*loggedUsers).id == userId ){
        SET_PTR_DOA(loggedUsers, nullptr);
        remove++;
        }
    }
    numbOfLoggedUsers -= remove;
    qDebug() << numbOfLoggedUsers;
}

void ClientsFilesMenager::checkOrReduceActivity(){
    for(uint i = 0;  i < numbOfLoggedUsers; i++){
        if((*(loggedUsers + i)).activity > 0){
                (*(loggedUsers + i)).activity--;
            }else{
                removeFastLoggedClient((*(loggedUsers + i)).id);
                i--;
            }
    }
}

unsigned long long ClientsFilesMenager::getFilePos(unsigned long long id){
    if(id == 0)
        return 0;
    // Check logged users
    for(uint i = 0; i < numbOfLoggedUsers; i++)
        if((*(loggedUsers + i)).id == id)
            return (*(loggedUsers + i)).filePosition;

    // Check fast table
    for(uint i = 0; i < numbOfUserFastAccess; i++)
        if((*(userFastAccess + i)).id > id && i > 0)
            return (*(userFastAccess + i - 1)).filePosition;
    return (*(userFastAccess + numbOfUserFastAccess - 1)).filePosition;
}

unsigned long long ClientsFilesMenager::getActualFilePos(){
    return filePos;
}

bool ClientsFilesMenager::isFileOperation(){
    return fileOperation;
}

