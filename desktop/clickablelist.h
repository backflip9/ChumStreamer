#ifndef CLICKABLELIST_H
#define CLICKABLELIST_H

#include <QWidget>
#include <QListWidget>

//class clickableList : public QWidget
class clickableList : public QListWidget
{
    Q_OBJECT
public:
    explicit clickableList(QWidget *parent = nullptr):QListWidget(parent){}
    //QListWidget::currentItem() is a shit function and i don't wanna manually check for a null return value every time i call it
    bool hasCurrentItem(){return currentItem() != nullptr;}

signals:
    void Clicked();
    void Clear();
    //void ClickedWithEvent(QMouseEvent* m);
    void playlistAppend();
    void playlistPrepend();
    void ReturnKey();
    void Back();
protected:
    //void mousePressEvent(QMouseEvent* m);
    void mousePressEvent(QMouseEvent* m);
    void keyPressEvent(QKeyEvent* k);

//public slots:
};

#endif // CLICKABLELIST_H
