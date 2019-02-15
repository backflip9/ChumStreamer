#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QDir>

class sub_ping_3;
namespace Ui {
class authDialog;
}

class authDialog : public QDialog
{
    Q_OBJECT

public:
    explicit authDialog(QWidget *parent = nullptr);
    ~authDialog();
    void setParentClass(sub_ping_3 *oneClass);
    void setAuthInfo();

private slots:
    void on_loginButton_clicked();

private:
    Ui::authDialog *ui;
    sub_ping_3 *parentClass;
    QString cacheFilePath=QDir::homePath() + "/subping.dat";
    void setTextBoxes();
};

#endif // AUTHDIALOG_H
