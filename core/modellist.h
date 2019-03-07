#ifndef MODELLIST_H
#define MODELLIST_H
#include<QList>

#include <QObject>

class modelList
{
  Q_OBJECT

public:
    modelList():QList(){}
signals:
    void listChanged(ChumListItem* newItem,int index=-1);//if we're appending, we leave it at -1
    void insert(int i,const ChumListItem* value);
    void append(const ChumListItem* value);
    bool find(const QString& needle);
    int size(){return candy.size();}
    void clear(){return candy.clear();}
    bool isEmpty(){return candy.empty();}
    QList<ChumListItem* > candy;
//private:
};

#endif // MODELLIST_H
