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
    QString m_strBasePath;
    QString m_strNewFolderName;
    QJsonObject m_jsonMainObject;
    QJsonArray m_jsonArray;
    QDir m_dirBase;


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
    void setTemplateTableDate(QList<QStringList>);
    QJsonArray getTemplateJSON();
    void createFiles();
    void createImageFile();
    void createLocAccFile();
    void createHandleBars();
    bool createHandleBarFile(QStringList tableEntry);
    void createJSs();
    bool createJSFile(QStringList tableEntry,int fileType);
    void createCSS();

    QString getJSFolderPath();
    QString getJSViewFolderPath();
    QString getJSModelFolderPath();
    QString getCSSFolderPath();
    QString getMediaFolderPath();
    QString getTemplateFolderPath();
    QString getImageFolderPath();
    QString getLocAccFolderPath();

    ~NewInterActivityForm();
    
private slots:
    void on_addTabBtn_clicked();
    void on_removeTabBtn_clicked();
    void on_saveBtn_clicked();
    void on_cancalBtn_clicked();

signals:
    void newInterActivityCreated(QString);
    void newJSONPrepared(QJsonObject);

private:
    Ui::NewInterActivityForm *ui;
};

#endif // NEWINTERACTIVITYFORM_H
