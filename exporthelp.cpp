#include "exporthelp.h"
#include "ui_exporthelp.h"

ExportHelp::ExportHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportHelp)
{
    ui->setupUi(this);
}

ExportHelp::~ExportHelp()
{
    delete ui;
}

void ExportHelp::setText(QString fnText)
{
    ui->tb_helpText->clear();
    ui->tb_helpText->setText(fnText);
}

void ExportHelp::on_pushButton_clicked()
{
    this->hide();
}
