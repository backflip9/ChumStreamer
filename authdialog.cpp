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
}

authDialog::~authDialog()
{
    delete ui;
}

void authDialog::on_loginButton_clicked()
{
    /*
    qDebug() << ui->serverEdit->text();
    qDebug() << ui->usernameEdit->text();
    qDebug() << ui->passwordEdit->text();
    */
    QUrl serverFromForm;
    serverFromForm.setScheme(ui->httpsCheckBox->isChecked()?"https":"http");
    //QString protocol=(ui->httpsCheckBox->isChecked())?"http://":"https://";
    serverFromForm.setHost(ui->serverEdit->text());
    serverFromForm.setPort(ui->portEdit->text().toInt());
    //parentClass->Server()=protocol + ui->serverEdit->text() + ':' + ui->portEdit->text();
    parentClass->Server()=serverFromForm;
    parentClass->Username()=ui->usernameEdit->text();
    parentClass->Password()=ui->passwordEdit->text();
    qDebug() << "Set!";
    //now that we have writeSave(), we don't need all the other stuff
    parentClass->writeSave();
    /*
    if(QFile(cacheFilePath).exists())
    {
      if(QFile(cacheFilePath).remove()){
          qDebug() << "removed subping.dat";
          QFile dat(cacheFilePath);
          if(dat.open(QIODevice::WriteOnly)){
              qDebug() << "writing server: "<<parentClass->Server().toString();
              //dat.write(parentClass->Server().toLatin1());
              dat.write(parentClass->Server().toString().toLatin1());
              dat.write("\n");
              dat.write(parentClass->Username().toLatin1());
              dat.write("\n");
              dat.write(parentClass->Password().toLatin1());
              dat.close();
          }
      }
      else{
          qDebug() << "failed to delete dat";
      }
    }
    else{
        qDebug() << cacheFilePath + " didn't exist, creating";
        QFile dat(cacheFilePath);
        dat.open(QIODevice::WriteOnly);
        dat.write(ui->serverEdit->text().toLatin1());
        dat.write("\n");
        dat.write(ui->usernameEdit->text().toLatin1());
        dat.write("\n");
        dat.write(ui->passwordEdit->text().toLatin1());
        dat.close();
    }
    */
}

void authDialog::setParentClass(chumstreamer *oneClass)
{
    parentClass=oneClass;
}

void authDialog::setAuthInfo()
{
    //case 1: this isn't the first time clicking on this dialogue box, and the members in parentClass are already set.
    if(!parentClass->Server().isEmpty() || !parentClass->Username().isEmpty() || !parentClass->Password().isEmpty())
    {
      //QUrl server(parentClass->Server());
      setTextBoxes();
      return;
    }

    //case 2: this is the first time and we have to set the class members by reading from the save file
    /*
    if(QDir(QDir::homePath()).exists(QFile(cacheFilePath).fileName()))
    {
        qDebug() << "file exists in homePath()";
        QFile dat(cacheFilePath);
        if(dat.open(QIODevice::ReadOnly))
        {
            parentClass->Server()=QUrl(dat.readLine().trimmed());
            qDebug() << "read server from file: "<<parentClass->Server().toString();
            parentClass->Username()=dat.readLine().trimmed();
            parentClass->Password()=dat.readLine().trimmed();
            setTextBoxes();
            dat.close();
            return;
        }
        else{
            qDebug() << "couldn't open dat as readonly";
        }
    }
    else{
        qDebug() << "dat didn't exist";
    }
    qDebug() << "fileName:";
    qDebug() << cacheFilePath;
    */
}

void authDialog::setTextBoxes()
{
  ui->serverEdit->setText(parentClass->Server().host());
  ui->usernameEdit->setText(parentClass->Username());
  ui->passwordEdit->setText(parentClass->Password());
  ui->portEdit->setText(QString::number(parentClass->Server().port()));
  if(parentClass->Server().scheme()=="https"){ui->httpsCheckBox->setCheckState(Qt::Checked);}
  /*
  else if(server.scheme()=="http")
  {
    ui->httpsCheckBox->setCheckState(Qt::Unchecked);
  }
  */
}
