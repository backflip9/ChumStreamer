#include "authdialog.h"
#include "ui_authdialog.h"
#include "chumstreamer.h"
#include <QDialog>
#include<QDebug>
#include<QFile>
#include<QDir>

authDialog::authDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::authDialog)
{
    ui->setupUi(this);
    //this->setTextBoxes();
}

authDialog::~authDialog()
{
    delete ui;
}

void authDialog::on_loginButton_clicked()
{
    qDebug() << "on_loginButton_clicked";
    QUrl url;
    url.setScheme(ui->httpsCheckBox->isChecked() ? "https" : "http");
    //QString protocol=(ui->httpsCheckBox->isChecked())?"http://":"https://";
    url.setHost(ui->serverEdit->text());
    url.setPort(ui->portEdit->text().toInt());
    url.setUserName(ui->usernameEdit->text());
    url.setPassword(ui->passwordEdit->text());
    parentClass->server = url;
    //parentClass->Server()=protocol + ui->serverEdit->text() + ':' + ui->portEdit->text();
    //now that we have writeSave(), we don't need all the other stuff
    parentClass->writeSave();
}

void authDialog::setParentClass(chumstreamer *oneClass)
{
    parentClass=oneClass;
}

void authDialog::setAuthInfo()
{
}

void authDialog::setTextBoxes()
{
  ui->serverEdit->setText(parentClass->server.host());
  ui->usernameEdit->setText(parentClass->server.userName());
  ui->passwordEdit->setText(parentClass->server.password());
  qDebug() << "port: " << parentClass->server.port();
  if(parentClass->server.port() != -1)
  {
    ui->portEdit->setText(QString::number(parentClass->server.port()));
  }
  ui->httpsCheckBox->setCheckState(parentClass->server.scheme()=="https" ? Qt::Checked : Qt::Unchecked);
}
