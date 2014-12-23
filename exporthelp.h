#ifndef EXPORTHELP_H
#define EXPORTHELP_H

#include <QDialog>

namespace Ui {
class ExportHelp;
}

class ExportHelp : public QDialog
{
    Q_OBJECT

public:
    explicit ExportHelp(QWidget *parent = 0);
    void setText(QString fnText);
    ~ExportHelp();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ExportHelp *ui;
};

#endif // EXPORTHELP_H
