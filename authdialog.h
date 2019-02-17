#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QDir>

class chumstreamer;
namespace Ui {
class authDialog;
}

class authDialog : public QDialog
{
    Q_OBJECT

public:
    explicit authDialog(QWidget *parent = nullptr);
    ~authDialog();
    void setParentClass(chumstreamer *oneClass);
    void setAuthInfo();

private slots:
    void on_loginButton_clicked();

private:
    Ui::authDialog *ui;
    chumstreamer *parentClass;
    QString cacheFilePath=QDir::homePath() + "/subping.dat";
    void setTextBoxes();
};

#endif // AUTHDIALOG_H
