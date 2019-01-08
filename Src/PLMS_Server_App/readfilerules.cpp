#include"readfilerules.hpp"
#include<QJsonObject>


ReadFileRules::ReadFileRules(QJsonObject& jsonObject, App* parent = nullptr)
    : parent(parent)
{

}

ReadFileRules::ReadFileRules(QJsonObject& jsonObject, FileType fileType, App* parent = nullptr)
    : parent(parent), fileType(fileType)
{

}

ReadFileRules::~ReadFileRules(){

}

bool ReadFileRules::check(User &user){
    return true;
}

App* ReadFileRules::getParent(){
    return parent;
}


