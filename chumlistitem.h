#ifndef CHUMLISTITEM_H
#define CHUMLISTITEM_H
#include<QObject>
#include<QListWidgetItem>
#include<QDebug>
#include<QMouseEvent>
#include "musicfolderinfo.h"
//class QMouseEvent;

class ChumListItem : public QListWidgetItem
{
public:
    //explicit ChumListItem(QString txt,QString artist,QListWidget *parent=nullptr) : QListWidgetItem(txt,parent), artistID(artist){}
    explicit ChumListItem(musicFolderInfo MFI,QListWidget *parent=nullptr) : QListWidgetItem(MFI.name,parent), info(new musicFolderInfo(MFI)){}
    ChumListItem(const ChumListItem& other):QListWidgetItem(other.info->name), info(new musicFolderInfo(*other.info)){}
    //for when we don't care about isSong
    ChumListItem(QString name,QString id):QListWidgetItem(name), info(new musicFolderInfo(name,id)){}
    ChumListItem(QString name,QString id,bool isSong):QListWidgetItem(name), info(new musicFolderInfo(name,id,isSong)){}
    musicFolderInfo* info;

    //explicit ChumListItem(QString txt,QListWidget *parent) : QListWidgetItem(txt,parent)
    /*
    QString getArtistID();
    void setArtistID(QString in);
    */

private:
protected:
    /*
    void keyPressEvent(QKeyEvent* k)
    {
      if(k->key()==Qt::Key_Comma)
        qDebug() << "key was comma!";
      else
      {
        qDebug() << "Key wasn't comma: "<<k->key();
      }

    }
    */
    void mousePressEvent(QMouseEvent* m)
    {
      qDebug() <<"Mouse event!";
      qDebug() << "mouse button: "<<m->button();
      //QListWidgetItem::mousePressEvent(m);
    }
};

#endif // CHUMLISTITEM_H
