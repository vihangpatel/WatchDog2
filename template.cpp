#include "templates.h"
#include <QDebug>
QString TEMPLATE_SUFFIX = "templates";
QString TEMPLATE_FILE_EXTENSION = ".handlebars";
QString TEMPLATE_COMPILE_COMMAND = "individual_compilation.bat";
QString TEMPLATE_FOLDER_COMPILE_COMMAND = "compile_handlebars_in_folder.bat";

Templates::Templates(QString basePath) {
    qfsw_tmplt = new QFileSystemWatcher();
    tmpltDir = new QDir(basePath);
    process = new QProcess();    
}

void Templates::connectSignals(){
    this->connect(qfsw_tmplt,SIGNAL(directoryChanged(QString)),SLOT(on_dir_changed(QString)));
    this->connect(qfsw_tmplt,SIGNAL(fileChanged(QString)),SLOT(on_file_changed(QString)));
}

void Templates::scanChanges()
{
    emit filesChanged(getFileInfoList());
}

void Templates::changeBasePath(QString strPath){
    fileCount = 0;
    str_basePath = strPath + "/" + TEMPLATE_SUFFIX;
    tmpltDir->setPath(str_basePath);
    deRegisterWatcher();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

bool Templates::registerWatcher(){

    QString fullPath = str_basePath;
    if(!tmpltDir->exists()){
        qWarning() << "Folder doesnt exists.";
        return false;
    }

    if(qfsw_tmplt != NULL){
        delete qfsw_tmplt;
        qfsw_tmplt = new QFileSystemWatcher();
        connectSignals();
    }

    QFileInfoList lstFiles = getFileInfoList();
    fileCount = lstFiles.length();
    for(int i = 0 ; i < fileCount ; i++ ){
        qfsw_tmplt->addPath(lstFiles.at(i).filePath());
    }

    qfsw_tmplt->addPath(str_basePath);
    // qDebug() << "Files added : " << qfsw_tmplt->files().length();
    return true;
}

QFileInfoList Templates::getFileInfoList(){
    QStringList filters;
    filters << "*.handlebars";
    tmpltDir->setNameFilters(filters);
    return tmpltDir->entryInfoList();
}

void Templates::on_file_changed(QString strFilePath){
    // qDebug() << "File changed : " << strFilePath;
    QFile file(strFilePath);
    if(!file.exists()){
        // qDebug()  << "File do not exits";
         emit filesChanged(getFileInfoList());
        return;
    }
    QStringList args ;
    QString compilePath = getIndividualTemplateCompilePath();
    QString fileName = strFilePath.replace(TEMPLATE_FILE_EXTENSION,"");
    fileName = fileName.replace("/","\\");
    compilePath.replace("/","\\");
    args << fileName;
    process->startDetached(compilePath,args);
}

QString Templates::getIndividualTemplateCompilePath()
{
    return str_basePath + "/" + TEMPLATE_COMPILE_COMMAND;
}

void Templates::on_dir_changed(QString strDirPath){
    // qDebug() << "Dir changed : " << strDirPath;
    deregisterFiles();
    registerWatcher();
    emit filesChanged(getFileInfoList());
}

void Templates::compileAllHandleBars()
{
    QString compilePath = getAllTemplateCompilePath();
    compilePath.replace("/","\\");
    process->startDetached(compilePath);
}

QString Templates::getAllTemplateCompilePath()
{
    return str_basePath + "/" + TEMPLATE_FOLDER_COMPILE_COMMAND;
}

void Templates::deRegisterWatcher(){
    deregisterDirs();
    deregisterFiles();
}

void Templates::deregisterFiles(){
    qfsw_tmplt->removePaths(qfsw_tmplt->files());
}

void Templates :: deregisterDirs(){
    qfsw_tmplt->removePaths(qfsw_tmplt->directories());
}

Templates::~Templates()
{
    return;
    delete qfsw_tmplt;
    delete process;
    delete tmpltDir;
}
