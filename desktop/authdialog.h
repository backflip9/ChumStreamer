#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QDir>

class chumstreamer_desktop;
namespace Ui {
class authDialog;
}

class authDialog : public QDialog
{
    Q_OBJECT

public:
    explicit authDialog(QWidget *parent = nullptr);
    ~authDialog();
    void setParentClass(chumstreamer_desktop *oneClass);
    void setAuthInfo();
    void setTextBoxes();

private slots:
    void on_loginButton_clicked();

private:
    Ui::authDialog *ui;
    chumstreamer_desktop *parentClass;
    QString cacheFilePath=QDir::homePath() + "/subping.dat";
};

#endif // AUTHDIALOG_H
