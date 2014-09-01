#ifndef NEWINTERACTIVITYFORM_H
#define NEWINTERACTIVITYFORM_H

#include <QtWidgets/QDialog>
#include <QMessageBox>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace Ui {
class NewInterActivityForm;
}

class NewInterActivityForm : public QDialog
{
    Q_OBJECT
    
public:
    QString str_basePath;
    QString str_newFolderName;
    QJsonObject mainJson;
    QJsonArray jArray;
    QDir baseDir;


    explicit NewInterActivityForm(QWidget *parent = 0);
    void changeBasePath(QString);
    bool validate();
    void clearAllFormData();
    void emptyTabsTable();
    void emptyComponentTable();
    bool createDirStructure();
    QString currentFolderPath();
    void writeConfigJson();
    void readConfigJson();
    void fillFormData();
    QList<QStringList> getTemplateTableData();
    QList<QStringList> getComponentTableData();
    void setTemplateTableDate(QList<QStringList>);
    void setComponentTableDate(QList<QStringList>);
    QJsonArray getTemplateJSON();
    void createFiles();
    void createLocAccFile();
    void createHandleBars();
    bool createHandleBarFile(QStringList tableEntry);
    void createJSs();
    bool createJSFile(QStringList tableEntry);
    void createCSS();
    ~NewInterActivityForm();
    
private slots:
    void on_addTabBtn_clicked();
    void on_removeTabBtn_clicked();
    void on_saveBtn_clicked();
    void on_cancalBtn_clicked();
    void on_addComponentBtn_clicked();
    void on_removeComponentBtn_clicked();

signals:
    void newInterActivityCreated(QString);
    void newJSONPrepared(QJsonObject);

private:
    Ui::NewInterActivityForm *ui;
};

#endif // NEWINTERACTIVITYFORM_H
