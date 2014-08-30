#include "newinteractivityform.h"
#include "ui_newinteractivityform.h"
#include <QDebug>
#include "locacc.h"
#include <QTableWidgetItem>

QString TEMPLATE_FOLDER = "templates";
QString JS_FOLDER = "js";
QString JS_VIEW_FOLDER = "views";
QString JS_MODEL_FOLDER = "models";
QString CSS_FOLDER = "css";
QString DATA_FOLDER = "data";
QString  LANG_FOLDER = "lang";
QString MEDIA_FOLDER = "media";
QString IMAGES_FOLDER = "images";
QString EN_FOLDER = "en";
QString MODULE_PLACE_HOLDER = "%@module#%";
QString VIEW_PLACE_HOLDER = "%@view/model#%";
QString CLASS_PLACE_HOLDER = "%@class#%";

NewInterActivityForm::NewInterActivityForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewInterActivityForm)
{
    ui->setupUi(this);
}

NewInterActivityForm::~NewInterActivityForm()
{
    delete ui;
}

void NewInterActivityForm::changeBasePath(QString strBasePath){
    str_basePath = strBasePath;
    baseDir.setPath(str_basePath);
}

void NewInterActivityForm::on_addTabBtn_clicked()
{
    int lastRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(lastRow);
    ui->tableWidget->setItem(lastRow,0,new QTableWidgetItem("Tab Name"));
    ui->tableWidget->setItem(lastRow,1,new QTableWidgetItem("template name"));
    ui->tableWidget->setItem(lastRow,2,new QTableWidgetItem("class name"));
    ui->tableWidget->setItem(lastRow,3,new QTableWidgetItem("ele id with #"));
}

void NewInterActivityForm::on_removeTabBtn_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    ui->tableWidget->removeRow(currentRow);
}


void NewInterActivityForm::on_saveBtn_clicked()
{
    if(!validate()){
        QMessageBox::critical(this,"Error","Not all data is supplied",QMessageBox::Cancel);
        return;
    }
    if(!baseDir.exists()){
        QMessageBox::critical(this,"Error","Base directory doesn't exist.",QMessageBox::Cancel);
        return;
    }
    if(!createDirStructure())
    {
        return ;
    }

    emit newInterActivityCreated(currentFolderPath());
    writeConfigJson();
    emit newJSONPrepared(mainJson);
    emit newInterActivityCreated(currentFolderPath());
    this->close();
}

bool NewInterActivityForm::createDirStructure(){
    QString currentPath = currentFolderPath();
    QDir tempDir(currentPath);
    if(tempDir.exists()){
        QMessageBox::critical(this,"Same Folder Name exists","You can not use same folder Name"
                              ,QMessageBox::Cancel);
        return false;
    }
    baseDir.mkpath(currentPath);
    baseDir.mkpath(currentPath + "/" + DATA_FOLDER);
    baseDir.mkpath(currentPath + "/" + JS_FOLDER);
    baseDir.mkpath(currentPath + "/" + JS_FOLDER + "/" + JS_VIEW_FOLDER);
    baseDir.mkpath(currentPath + "/" + JS_FOLDER + "/" + JS_MODEL_FOLDER);
    baseDir.mkpath(currentPath + "/" + CSS_FOLDER);
    baseDir.mkpath(currentPath + "/" + MEDIA_FOLDER);
    baseDir.mkpath(currentPath + "/" + MEDIA_FOLDER + "/" + IMAGES_FOLDER);
    baseDir.mkpath(currentPath +  "/" + LANG_FOLDER);
    baseDir.mkpath(currentPath + "/" +  LANG_FOLDER + "/" + EN_FOLDER + "/" + DATA_FOLDER);
    baseDir.mkpath(currentPath + "/" + TEMPLATE_FOLDER);
    createFiles();
    return true;
}

void NewInterActivityForm::createFiles()
{
    createHandleBars();
    createCSS();
    createJSs();
    createLocAccFile();
}

void NewInterActivityForm::createLocAccFile()
{
    LOCACC *locAcc = new LOCACC(currentFolderPath());
    locAcc->changeBasePath(currentFolderPath());
    QStringList data;
    data << "tab-contents" << "tab-contents";
    QTreeWidgetItem *tabContentsScr = locAcc->addScreen(data);
    QList<QStringList> tabData = getTemplateTableData();
    for(int i = 0 ; i < tabData.length() ; i++)
    {
        // Maintain input parameter order as specified in the addElement method of the locAcc Class.
        data.clear();
        data << QString("player-tab-" + QString::number(i));
        data << QString("player-tab-" + QString::number(i));
        data << QString("text");
        data << QString("");
        data << QString("1002");
        QTreeWidgetItem *tabItem = locAcc->addElement(data,tabContentsScr);

        data.clear();
        data <<  QString("0");          // msgID
        data << tabData.at(i).at(0); // loc
        data << tabData.at(i).at(0); // acc compulsory
        QTreeWidgetItem *msgItem = locAcc->addMessage(data,true,tabItem);
        delete msgItem;
        delete tabItem;
    }
    // Add Title Screen
    data.clear();
    data << "title-screen" << "title-screen";
    QTreeWidgetItem *titleScreen = locAcc->addScreen(data);
    data.clear();
    data << "heading" << "heading" << "text" << "" << "1002";
    QTreeWidgetItem *headingEle = locAcc->addElement(data,titleScreen);
    data.clear();
    data << "0" << "UpdateHeader" << "UpdateHeader";
    delete locAcc->addMessage(data,true,headingEle);

    // Add overview screen
    data.clear();
    data << ui->overViewScreenId->text() << ui->overViewScreenId->text();
    QTreeWidgetItem *overViewScreen = locAcc->addScreen(data);
    data.clear();
    data << "overview-header" << "overview-header" << "text" << "" << "";
    QTreeWidgetItem *overViewHeader = locAcc->addElement(data,overViewScreen);
    data.clear();
    data << "0" << "Add Header of the overview tab." << "";
    delete  locAcc->addMessage(data,true,overViewHeader);

    data.clear();
    data << "overview-text" << "overview-text" << "text" << "" << "";
    QTreeWidgetItem *overViewtext = locAcc->addElement(data,overViewScreen);
    data.clear();
    data << "0" << "Add Header TEXT of the overview tab." << "";
    delete locAcc->addMessage(data,true,overViewtext);

    locAcc->writeFile();
    delete locAcc;
    delete overViewHeader;
    delete overViewtext;
    delete overViewScreen;
    delete headingEle;
    delete titleScreen;
    delete tabContentsScr;
}

void NewInterActivityForm::createCSS()
{
    QString filePath = currentFolderPath() + "/" + CSS_FOLDER + "/" + ui->idPrefixText->text() +  ".css";
    QFile cssFile(filePath);
    if(cssFile.exists())
    {
        return ;
    }
    cssFile.open(QIODevice::ReadWrite | QIODevice::Text);
    cssFile.close();
}

void NewInterActivityForm::createJSs()
{
    QString filePath = currentFolderPath() + "/" + JS_FOLDER + "/" + "initialize.js";
    QFile jsInitializeFile(filePath);
    if(jsInitializeFile.exists())
    {
        return ;
    }
    // Create Initialize File
    jsInitializeFile.open(QIODevice::ReadWrite | QIODevice::Text);
    jsInitializeFile.close();

    // Create view File
    QList<QStringList> tableEntries = getTemplateTableData();
    for(int i = 0 ; i < tableEntries.length() ; i++ )
    {
        createJSFile(tableEntries.at(i));
    }

    // Create model File
    QString modelFilePath = currentFolderPath() + "/" + JS_FOLDER + "/" + JS_MODEL_FOLDER + "/" + ui->idPrefixText->text() + ".js";
    QFile modelFile(modelFilePath);
    modelFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QString sampleFileData = "";
    QFile sampleFile("pristineJsSample.txt");
    if(sampleFile.exists())
    {
        // Replace placeholder by the actual class name.
        sampleFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray byteArrayData= sampleFile.readAll();
        QString stringData(byteArrayData);
        stringData.replace(VIEW_PLACE_HOLDER,"Models");
        stringData.replace(CLASS_PLACE_HOLDER,ui->modelClassText->text());
        stringData.replace(MODULE_PLACE_HOLDER,ui->moduleText->text());
        sampleFileData = stringData;
        sampleFile.close();
    }
    QTextStream stream(&modelFile);
    stream <<  sampleFileData;
    modelFile.close();
}

bool NewInterActivityForm::createJSFile(QStringList tableEntry)
{
    QString viewFilePath = currentFolderPath() + "/" + JS_FOLDER + "/" + JS_VIEW_FOLDER + "/" + tableEntry.at(1) + ".js";
    QFile newJsFile(viewFilePath);
    if(newJsFile.exists())
    {
        return false;
    }
    QString sampleFileData = "";
    QFile sampleFile("pristineJsSample.txt");
    if(sampleFile.exists())
    {
        // Replace placeholder by the actual class name.
        sampleFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray byteArrayData= sampleFile.readAll();
        QString stringData(byteArrayData);
        stringData.replace(VIEW_PLACE_HOLDER,"Views");
        stringData.replace(CLASS_PLACE_HOLDER,tableEntry.at(2));
        stringData.replace(MODULE_PLACE_HOLDER,ui->moduleText->text());
        sampleFileData = stringData;
        sampleFile.close();
    }
    newJsFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&newJsFile);
    stream <<  sampleFileData;
    newJsFile.close();
    return true;
}

void NewInterActivityForm::createHandleBars()
{
    QList<QStringList> tableEntries = getTemplateTableData();
    for(int i = 0 ; i < tableEntries.length() ; i++ )
    {
        createHandleBarFile(tableEntries.at(i));
    }
}

bool NewInterActivityForm::createHandleBarFile(QStringList tableEntry)
{
    QString filePath = currentFolderPath() + "/" + TEMPLATE_FOLDER + "/" + tableEntry.at(1) + ".handlebars";
    QFile newHandleBarFile(filePath);
    if(newHandleBarFile.exists())
    {
        return false;
    }
    newHandleBarFile.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream stream(&newHandleBarFile);
    QString eleID = tableEntry.at(3);
    stream <<  "<div id=\"{{idPrefix}}" + eleID.replace("#","") + "\" class=\"\"></div>";
    newHandleBarFile.close();
    return true;
}

bool NewInterActivityForm::validate()
{
    return ui->folderNameText->text().length() > 0 &&
            ui->moduleText->text().length() > 0 &&
            ui->idPrefixText->text().length() > 0 &&
            ui->modelClassText->text().length() > 0 &&
            ui->overViewTabTemplateText->text().length() > 0 ;
}

void NewInterActivityForm::on_cancalBtn_clicked()
{
    clearAllFormData();
    this->close();
}

void NewInterActivityForm::clearAllFormData()
{
    return;
    foreach(QLineEdit *widget, this->findChildren<QLineEdit*>()) {
        widget->clear();
    }

    foreach(QCheckBox *widget, this->findChildren<QCheckBox*>()) {
        widget->setChecked(false);
    }
    emptyComponentTable();
    emptyTabsTable();
}

void NewInterActivityForm::emptyComponentTable()
{
    int rows = ui->componentTable->rowCount();
    for(int i = 0 ; i < rows ; i++ )
    {
        ui->componentTable->removeRow(i);
    }
}

void NewInterActivityForm::emptyTabsTable()
{
    int rows = ui->tableWidget->rowCount();
    for(int i = 0 ; i < rows ; i++ )
    {
        ui->tableWidget->removeRow(i);
    }
}

QString  NewInterActivityForm::currentFolderPath()
{
    return (str_basePath + "/" + ui->folderNameText->text());
}

void NewInterActivityForm::on_addComponentBtn_clicked()
{
    int lastRow = ui->componentTable->rowCount();
    ui->componentTable->insertRow(lastRow);
    ui->componentTable->setItem(lastRow,0,new QTableWidgetItem("Companent name"));
    ui->componentTable->setItem(lastRow,1,new QTableWidgetItem("base path"));
}

void NewInterActivityForm::on_removeComponentBtn_clicked()
{
    int currentRow = ui->componentTable->currentRow();
    ui->componentTable->removeRow(currentRow);
}

void NewInterActivityForm::readConfigJson()
{

}

void NewInterActivityForm::writeConfigJson()
{
    QString moduleName = ui->moduleText->text();
    QString themeType = ui->themeTypeText->text();
    QString playerTheme = ui->playerThemeText->text();
    QString idPrefix = ui->idPrefixText->text();
    QString modelClassName = ui->modelClassText->text();

    QJsonObject jObject;
    jObject["module"] =moduleName;
    jObject["idPrefix"] = idPrefix;
    jObject["newSaveState"] = ui->cb_newSaveState->isChecked();
    jObject["isTwoStepLoad"] = ui->cb_isTwoStepLoad->isChecked();
    jObject["isSaveStateAllowed"] = ui->cb_isSaveStateAllowed->isChecked();
    jObject["tabsData"] = getTemplateJSON();
    jObject["themeType"] = themeType.toInt();
    jObject["playerTheme"] = playerTheme.toInt();
    QJsonObject dataObject ;
    dataObject["class"] = modelClassName;
    jObject["model"] = dataObject;

    QJsonObject resourceObj;
    QJsonArray tempArray;
    QJsonObject tempObject;
    resourceObj["isNewJsonType"] = ui->cb_isNewJsonType->isChecked();
    resourceObj["css"]= tempArray;
    resourceObj["js"]= tempArray;
    resourceObj["media"]= tempObject;
    resourceObj["templates"]= tempArray;
    resourceObj["json"]= tempArray;

    QJsonObject mainJSONObj;
    mainJSONObj["config"] = jObject;
    mainJSONObj["resources"] = resourceObj;
    mainJson = mainJSONObj;
    // qDebug() << "CONFIG : \n" << mainJson;
}

QList<QStringList> NewInterActivityForm::getTemplateTableData()
{
    QList<QStringList> tableData;
    int rowCount = ui->tableWidget->rowCount();

    QStringList overViewTab;
    overViewTab << "Overview" << ui->overViewTabTemplateText->text()
                << ui->overViewTabClass->text() << ui->overViewTabEL->text();
    tableData.append(overViewTab);

    for(int i = 0 ; i < rowCount ; i++){
        QStringList currentRow ;
        currentRow <<  ui->tableWidget->item(i,0)->text() <<  ui->tableWidget->item(i,1)->text() <<
                       ui->tableWidget->item(i,2)->text() << ui->tableWidget->item(i,3)->text();
        // qDebug() << currentRow;
        tableData.append(currentRow);
    }
    return tableData;
}

QList<QStringList> NewInterActivityForm::getComponentTableData()
{
    QList<QStringList> tableData;
    int rowCount = ui->componentTable->rowCount();
    for(int i = 0 ; i < rowCount ; i++){
        QStringList currentRow ;
        currentRow << ui->componentTable->item(i,0)->text() << ui->componentTable->item(i,1)->text();
        tableData.insert(i,currentRow);
    }
    return tableData;
}

QJsonArray NewInterActivityForm::getTemplateJSON()
{
    QString overViewTabName = ui->overViewTabTemplateText->text();
    QString overVIewTabEL = ui->overViewTabEL->text();
    QString overViewClass = ui->overViewTabClass->text();
    QString overViewScreenId = ui->overViewScreenId->text();
    QString leftContainerImageId = ui->leftImageId->text();
    int i = 0;
    QList<QStringList> templateData = getTemplateTableData();
    QJsonArray tabsJArray;
    // Exlude first entry "Overview"
    for(i= 1 ; i < templateData.count() ; i++ )
    {
        QJsonObject tabsData ;
        QJsonObject dataEl;
        QJsonObject viewData;
        tabsData["templateName"] = templateData.at(i).at(1);
        viewData["class"] =  templateData.at(i).at(2);
        dataEl["el"] = templateData.at(i).at(3);
        viewData["data"] =  dataEl;
        tabsData["view"] = viewData;
        tabsJArray.insert(i-1,tabsData);
    }

    /********* OVERVIEW TAB DATA *********/
    QJsonObject tabsData;
    QJsonObject dataEl;
    QJsonObject viewData;
    QJsonObject overViewTabImageJson;
    tabsData["templateName"] = overViewTabName;
    viewData["class"] = overViewClass;
    dataEl["el"] = overVIewTabEL;
    viewData["data"] = dataEl;
    tabsData["view"] = viewData;
    overViewTabImageJson["leftImageContainerID"] = leftContainerImageId;
    overViewTabImageJson["screenID"] = overViewScreenId;
    tabsData["overviewTabData"] = overViewTabImageJson;
    tabsJArray.insert(0,tabsData);
    return tabsJArray;
}



