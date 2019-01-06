#include"readfilerules.hpp"
#include<QJsonDocument>

ReadFileRules::ReadFileRules(ReadFileRulesType type){

}

ReadFileRules::ReadFileRules(QJsonDocument* jsonDoc){

}

ReadFileRules::~ReadFileRules(){

}

bool ReadFileRules::checkRules(User *user){
    return true;
}

App* ReadFileRules::getParent(){
    return parent;
}

ReadFileRulesType ReadFileRules::getReadFilesRulesType(){
    return type;
}

