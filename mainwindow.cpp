#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initialize();
}

void MainWindow::loadSavedSettings(){
    str_basePath = "D:/DE";
    str_rootPath = "D:/DE";
}

void MainWindow::initialize(){
    loadSavedSettings();
    qfs_model = new QFileSystemModel;
    qfs_model->setRootPath(str_basePath);
    ui->treeView->setModel(qfs_model);
    ui->treeView->setRootIndex(qfs_model->index(str_rootPath));
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    qfsw = new QFileSystemWatcher(this);
    form = new NewInterActivityForm(this);
    form->changeBasePath(str_rootPath);
    setWindowTitle(str_basePath);
    js = new JS(str_basePath);
    tmplt = new Templates(str_basePath);
    config = new ConfigHandler(str_basePath);
    css = new CSS(str_basePath);
    locAcc = new LOCACC(str_basePath);
    ui->locTreeWidget->addTopLevelItem(locAcc->getLocAccTree());
    initTrayIcon();
    connectSignals();
    manageLocAccItemsVisibility(-1);
}

void MainWindow::initTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/images/tray_icon.ico");
    trayIcon->setIcon(icon);
    QMenu *menu = new QMenu(this);
    menu->addAction("Show",this,SLOT(show()));
    menu->addAction("Hide",this,SLOT(hide()));
    QMenu *minifyMenu = new QMenu("Minify",this);
    minifyMenu->addAction("Common");
    minifyMenu->addAction("Interactive");
    minifyMenu->addAction("Preloader");
    menu->addMenu(minifyMenu);
    menu->addAction("Compile Handlebars");
    menu->addAction("Refresh",this,SLOT(scanChanges()));
    menu->addAction("Exit",this,SLOT(close()));
    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

void MainWindow::connectSignals(){
    connect(tmplt,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateTemplateList(QFileInfoList)));
    connect(ui->templateList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(templateFileListClicked(QListWidgetItem*)));

    connect(js,SIGNAL(jsfilesChanged(QFileInfoList)),this,SLOT(updateJSList(QFileInfoList)));
    connect(ui->jsViewList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(jsFileListClicked(QListWidgetItem*)));

    connect(css,SIGNAL(filesChanged(QFileInfoList)),this,SLOT(updateCssList(QFileInfoList)));
    connect(ui->cssList,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(cssFileListClicked(QListWidgetItem*)));

    connect(form,SIGNAL(newInterActivityCreated(QString)),this,SLOT(changeBasePath(QString)));
    connect(form,SIGNAL(newJSONPrepared(QJsonObject)),config,SLOT(newInteractivityCreated(QJsonObject)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scanChanges(){
    changeBasePath(str_basePath);
}

void MainWindow::registerWatcher(){
    js->registerWatcher();
    tmplt->registerWatcher();
}

void MainWindow::deregisterWatcher(){
    js->deRegisterWatcher();
    tmplt->deRegisterWatcher();
}

/**************************************************************
 *
 *  O V E R V I E W    T A B
 *
 **************************************************************/

void MainWindow::changeBasePath(QString strBasePath)
{
    deregisterWatcher();
    str_basePath = strBasePath;
    config->changeBasePath(str_basePath);
    tmplt->changeBasePath(str_basePath);
    js->changeBasePath(str_basePath);
    css->changeBasePath(str_basePath);
    locAcc->changeBasePath(str_basePath);
    updateDirTree();
    setWindowTitle("DE-Interactives " + str_basePath);
}

void MainWindow::on_openDialog_clicked()
{
    deregisterWatcher();
    str_basePath = QFileDialog::getExistingDirectory (this, tr("Directory"),str_rootPath);
    changeBasePath(str_basePath);
}

void MainWindow::updateDirTree(){
    ui->treeView->setRootIndex(qfs_model->index(str_basePath));
}

void MainWindow::resetAll(){
    qfsw->removePaths(qfsw->files());
    qfsw->removePaths(qfsw->directories());
}

/****************************************************************
             T E M P L A T E   H A N D  L I N G
******************************************************************/

void MainWindow::updateTemplateList(QFileInfoList fileList){
    ui->templateList->clear();
    QJsonObject obj;
    QJsonArray jArray ;
    QString currentFileName ;
    for(int i = 0 ; i < fileList.length() ; i++){
        currentFileName = fileList.at(i).fileName().replace(QString(".handlebars"),QString(".js"));
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        jArray.insert(i,obj);
        ui->templateList->addItem(currentFileName);
        qDebug() << fileList.at(i).fileName();
    }
    ui->templateList->setCurrentRow(0);
    config->setTemplateJArray(syncTmpltList(jArray));
    templateFileListClicked(ui->templateList->currentItem());
    config->writeConfigJson();
}

QJsonArray MainWindow::syncTmpltList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray templateJArray = config->getTemplateJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < templateJArray.count() ; j ++)
        {
            tempOrigObj = templateJArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                newArray.replace(i,tempNewObj);
                qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::templateFileListClicked(QListWidgetItem *item){
    if(item == NULL)
    {
        return;
    }
    QJsonArray tmpltJArray = config->getTemplateJArray();
    QString itemText = item->text();
    for(int i = 0; i < tmpltJArray.count() ; i++){
        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj["url"] == itemText){
            ui->templateNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_TemplateFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_templateNextLoadCheckBox_clicked()
{
    QJsonArray tmpltJArray = config->getTemplateJArray();
    QListWidgetItem *currentItem  = ui->templateList->currentItem();
    for(int i = 0; i < tmpltJArray.count() ; i++){

        QJsonObject obj = tmpltJArray.at(i).toObject();
        if(obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->templateNextLoadCheckBox->isChecked();
            tmpltJArray.replace(i,obj);
        }
    }
    config->setTemplateJArray(tmpltJArray);
    config->writeConfigJson();
    qDebug() << "ON CHECK BOX CLICKED : " << tmpltJArray;
}

/****************************************************************
             J S     H A N D L I N G
******************************************************************/

void MainWindow::jsFileListClicked(QListWidgetItem *item){
    if(item == NULL)
    {
        return;
    }
    QJsonArray jsViewJArray = config->getJSJArray();
    QString itemText = item->text();
    for(int i = 0; i < jsViewJArray.count() ; i++){
        QJsonObject obj = jsViewJArray.at(i).toObject();
        if(obj["url"] == itemText)
        {
            ui->jsViewNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_JSFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_jsViewList_itemSelectionChanged()
{
    jsFileListClicked(ui->jsViewList->currentItem());
}

void MainWindow::updateJSList(QFileInfoList fileList){
    ui->jsViewList->clear();
    QJsonArray jsViewJArray;
    QString currentFileName ;
    QJsonObject obj;
    QFileInfoList jsFolderFiles = js->getJSFolderInfoList();
    QFileInfoList modelFolderFiles = js->getModelFileInfoList();
    QFileInfoList viewFolderFiles = js->getViewFileInfoList();

    for(int i = 0 ; i < jsFolderFiles.count() ; i++ ){
        currentFileName = jsFolderFiles.at(i).fileName();
        ui->jsViewList->addItem(currentFileName);
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = false;
        jsViewJArray.insert(i,obj);
    }

    int addCnt = jsFolderFiles.length();
    for(int i = 0 ; i < modelFolderFiles.count() ; i++ ){
        currentFileName = modelFolderFiles.at(i).fileName();
        ui->jsViewList->addItem("models/" + currentFileName);
        obj["url"] ="models/" +  currentFileName;
        obj["isNextStepLoad"] = true;
        jsViewJArray.insert(addCnt + i,obj);
    }

    addCnt = jsFolderFiles.length() + modelFolderFiles.length();
    for(int i = 0 ; i < viewFolderFiles.count() ; i++ ){
        currentFileName = viewFolderFiles.at(i).fileName();
        ui->jsViewList->addItem("views/" + currentFileName);
        obj["url"] ="views/" +  currentFileName;
        obj["isNextStepLoad"] = true;
        jsViewJArray.insert(addCnt +  i,obj);
    }

    ui->jsViewList->setCurrentRow(0);
    config->setJSJArray(syncJSList(jsViewJArray));
    config->writeConfigJson();
    jsFileListClicked(ui->jsViewList->currentItem());
}


void MainWindow::on_jsViewNextLoadCheckBox_clicked()
{
    QListWidgetItem *currentItem  = ui->jsViewList->currentItem();
    QJsonArray jsViewJArray = config->getJSJArray();
    for(int i = 0; i < jsViewJArray.count() ; i++){

        QJsonObject obj = jsViewJArray.at(i).toObject();
        if( obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->jsViewNextLoadCheckBox->isChecked();
            jsViewJArray.replace(i,obj);
        }
    }
    config->setJSJArray(jsViewJArray);
    config->writeConfigJson();
    qDebug() << "ON CHECK BOX CLICKED : JS LIST \n" << jsViewJArray;
}

QJsonArray MainWindow::syncJSList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray jsOrigArray = config->getJSJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < jsOrigArray.count() ; j ++)
        {
            tempOrigObj = jsOrigArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                newArray.replace(i,tempNewObj);
                qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::on_templateList_itemSelectionChanged()
{
    templateFileListClicked(ui->templateList->currentItem());
}

/****************************************************************
                                                C S S        H A N D  L I N G
******************************************************************/

void MainWindow::updateCssList(QFileInfoList fileList){
    ui->cssList->clear();
    QJsonObject obj;
    QJsonArray cssArray ;
    QString currentFileName ;
    for(int i = 0 ; i < fileList.length() ; i++){
        currentFileName = fileList.at(i).fileName();
        obj["url"] = currentFileName;
        obj["isNextStepLoad"] = true;
        cssArray.insert(i,obj);
        ui->cssList->addItem(currentFileName);
        qDebug() << fileList.at(i).fileName();
    }
    ui->cssList->setCurrentRow(0);
    config->setCssJArray(syncCSSList(cssArray));
    cssFileListClicked(ui->cssList->currentItem());
    config->writeConfigJson();
}

QJsonArray MainWindow::syncCSSList(QJsonArray newArray)
{
    QJsonObject tempNewObj,tempOrigObj;
    QJsonArray cssJArray = config->getCssJArray();
    for(int i = 0 ; i < newArray.count() ; i++)
    {
        tempNewObj =  newArray.at(i).toObject();
        for(int j = 0 ; j < cssJArray.count() ; j ++)
        {
            tempOrigObj = cssJArray.at(j).toObject();
            if(tempNewObj["url"] == tempOrigObj["url"]){
                tempNewObj["isNextStepLoad"] = tempOrigObj["isNextStepLoad"];
                newArray.replace(i,tempNewObj);
                qDebug() <<"replced";
            }
        }
    }
    return newArray;
}

void MainWindow::cssFileListClicked(QListWidgetItem *item){
    if(item == NULL)
    {
        return;
    }
    QJsonArray cssJArray = config->getCssJArray();
    QString itemText = item->text();
    for(int i = 0; i < cssJArray.count() ; i++){
        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj["url"] == itemText){
            ui->cssNextLoadCheckBox->setChecked(obj["isNextStepLoad"].toBool());
            ui->label_CSSFileName->setText(itemText);
            break;
        }
    }
}

void MainWindow::on_cssNextLoadCheckBox_clicked()
{
    QJsonArray cssJArray = config->getCssJArray();
    QListWidgetItem *currentItem  = ui->cssList->currentItem();
    for(int i = 0; i < cssJArray.count() ; i++){

        QJsonObject obj = cssJArray.at(i).toObject();
        if(obj["url"] == currentItem->text())
        {
            obj["isNextStepLoad"] = ui->cssNextLoadCheckBox->isChecked();
            cssJArray.replace(i,obj);
        }
    }
    config->setCssJArray(cssJArray);
    config->writeConfigJson();
    qDebug() << "ON CHECK BOX CLICKED CSS: " << cssJArray;
}

void MainWindow::on_cssList_itemSelectionChanged()
{
    cssFileListClicked(ui->cssList->currentItem());
}

/***************************************************************
 *                         L O C - A C C     J S O N     H A N D L I N G
 ***************************************************************/

/*
#################    ADD  MESSAGE   #######################
*/

void MainWindow::on_addScreenBtn_clicked()
{
    QString screenId = ui->screenNameText->text();
    QString screenName = ui->screenNameText_2->text();
    QStringList screenData;
    screenData << screenId << screenName;
    if(!locAcc->addScreen(screenData))
    {
        QMessageBox::critical(this,"Same screen Id exists","Same screen Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

void MainWindow::on_addEleBtn_clicked()
{
    QString eleName = ui->eleNameText->text();
    QString eleAccId = ui->eleAccIdText->text();
    QString eleRole = ui->eleRoleText->text();
    QString eleType = ui->eleTypeText->text();
    QString eleTabIndex = ui->eleTabIndexText->text();
    QStringList eleData;
    eleData << eleName << eleAccId << eleType << eleRole << eleTabIndex;
    if(!locAcc->addElement(eleData,ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same element Id exists in screen.","Same element Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

void MainWindow::on_addMsgBtn_clicked()
{
    QStringList msgData;
    msgData << ui->msgIdText->text() << ui->locMsgText->text() << ui->accMsgText->text();
    if(!locAcc->addMessage(msgData,ui->cb_isAccTextSame->isChecked(),ui->locTreeWidget->currentItem()))
    {
         QMessageBox::critical(this,"Same message Id exists in element.","Same message Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

/*
#################    UPDATE MESSAGE   #######################
*/

void MainWindow::on_updtScreenBtn_clicked()
{
    QString screenId = ui->updtScreenIdText->text();
    QString screenName = ui->updtScreenNameText->text();
    QStringList screenData;
    screenData << screenId << screenName;
    if(!locAcc->updateScreen(screenData,ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same screen Id exists","Same screen Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

void MainWindow::on_updateMsgBtn_clicked()
{
    QStringList msgData;
    msgData << ui->updtMsgidText->text() << ui->updtLocMsgText->text() << ui->updtAccMsgText->text();
    if(!locAcc->updateMessage(msgData,ui->cb_isAccTextSameUpdt->isChecked(),ui->locTreeWidget->currentItem()))
    {
         QMessageBox::critical(this,"Same message Id exists in element.","Same message Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

void MainWindow::on_updtEleBtn_clicked()
{
    QString eleName = ui->updtEleIdText->text();
    QString eleAccId = ui->updtEleAccIdText->text();
    QString eleRole = ui->updtEleRoleText->text();
    QString eleType = ui->updtEleTypeText->text();
    QString eleTabIndex = ui->updTabIndexText->text();
    QStringList eleData;
    eleData << eleName << eleAccId << eleType << eleRole << eleTabIndex;
    if(!locAcc->updateElement(eleData,ui->locTreeWidget->currentItem()))
    {
        QMessageBox::critical(this,"Same element Id exists in screen.","Same element Id exists .Please use another Id.",QMessageBox::Cancel);
    }
}

void MainWindow::on_locTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    int indentationCount =  getTreeItemIndentationLevel(item);
    qDebug() << "LOC TREE CLICKED : " << item->text(column) << " col : " << column << " Heirarchy : "
             <<  indentationCount;
    manageLocAccItemsVisibility(indentationCount);
}

void MainWindow::manageLocAccItemsVisibility(int indentationLevel)
{
    ui->groupBox_addScreen->setVisible(indentationLevel == 0 );
    ui->groupBox_addElement->setVisible(indentationLevel == 1 );
    ui->groupBox_addMessage->setVisible(indentationLevel == 2 );

    ui->groupBox_optScr->setVisible(indentationLevel == 1);
    ui->groupBox_optEle->setVisible(indentationLevel == 2);
    ui->groupBox_optMsg->setVisible(indentationLevel == 3);

    ui->groupBox_updateScreen->setVisible(indentationLevel == 1);
    ui->groupBox_updateElement->setVisible(indentationLevel == 2);
    ui->groupBox_updateMessage->setVisible(indentationLevel == 3);

    updateLocDetails(indentationLevel);
}

void MainWindow::updateLocDetails(int indentationLevel)
{
    QStringList dataList;
    QTreeWidgetItem *currentItem = ui->locTreeWidget->currentItem();
    switch (indentationLevel) {
            case 1:
                dataList =locAcc->getScreenTreeData(currentItem);
                fillScreenDetail(dataList);
                break;
            case 2:
                dataList =locAcc->getElementTreeData(currentItem);
                fillElementDetail(dataList);
                break;
            case 3:
                dataList =locAcc->getMessageTreeData(currentItem);
                fillMessageDetail(dataList);
                break;
            default:
                break;
    }
}

void MainWindow::fillScreenDetail(QStringList data)
{
    ui->updtScreenIdText->setText(data.at(0));
    ui->updtScreenNameText->setText(data.at(0));
}

void MainWindow::fillElementDetail(QStringList data)
{
    ui->updtEleIdText->setText(data.at(0));
    ui->updtEleAccIdText->setText(data.at(1));
    ui->updtEleTypeText->setText(data.at(2));
    ui->updtEleRoleText->setText(data.at(3));
    ui->updTabIndexText->setText(data.at(4));
}

void MainWindow::fillMessageDetail(QStringList data)
{
    ui->updtMsgidText->setText(data.at(0));
    ui->cb_isAccTextSameUpdt->setChecked(data.at(1) == "1" ? true : false);
    ui->updtLocMsgText->setText(data.at(2));
    ui->updtAccMsgText->setText(data.at(3));
    qDebug() << "Message detail updated : " << data.at(1);
}

int MainWindow::getTreeItemIndentationLevel(QTreeWidgetItem *currentItem,int count)
{
    QTreeWidgetItem *parent = currentItem->parent();
    if(parent != NULL)
    {
        count = getTreeItemIndentationLevel(parent,count + 1);
    }
    return count;
}

void MainWindow::on_cb_isAccTextSame_clicked()
{
    bool cb_status = ui->cb_isAccTextSame->isChecked();
    ui->accMsgText->setDisabled(cb_status);
    if(cb_status)
    {
        ui->accMsgText->setText(ui->locMsgText->text());
    }
}

void MainWindow::on_locTreeWidget_itemSelectionChanged()
{
    on_locTreeWidget_itemClicked(ui->locTreeWidget->currentItem(),0);
}

void MainWindow::on_locMsgText_textChanged(const QString &arg1)
{
    if(ui->cb_isAccTextSame->isChecked())
    {
        ui->accMsgText->setText(ui->locMsgText->text());
    }
}

/*
  #############################################
            D E L E T I O N   O F   T H E   D A T A
*/

void MainWindow::on_dltScrBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Screen !!! Allow Deletion?",
                         QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
        locAcc->deleteScreen(ui->locTreeWidget->currentItem());
    }
}

void MainWindow::on_dltMsgBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Message !!! Allow Deletion?",
                         QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
       locAcc->deleteMessage(ui->locTreeWidget->currentItem());
    }

}

void MainWindow::on_dltEleBtn_clicked()
{
    int result = QMessageBox::warning(this,"Are you Sure ???" , "You are about to delete Element !!! Allow Deletion?",
                         QMessageBox::Ok, QMessageBox::Cancel);
    if(result == QMessageBox::Ok)
    {
       locAcc->deleteElement(ui->locTreeWidget->currentItem());
    }
}


/***************************************************************
 *                          C R E A T E S    N E W    I N T E R A C T I V I T Y
 ***************************************************************/
void MainWindow::on_createNewInter_clicked()
{
    form->clearAllFormData();
    form->show();
}

