#ifndef MODELLIST_H
#define MODELLIST_H

#include<QList>
#include <QObject>

//forward decl
class ChumListItem;

class modelList:public QObject
{
  Q_OBJECT

public:
    explicit modelList():curr(-1){};
    void insert(int i,const ChumListItem* value);
    void append(const ChumListItem* value);
    bool find(const QString& needle);
    ChumListItem* at(int index){return candy.at(index);}
    int size(){return candy.size();}
    void clear(){return candy.clear();}
    bool isEmpty(){return candy.empty();}
    QList<ChumListItem* > candy;
    bool hasCurrentItem(){return curr!=-1;}
    //int getCurrentItem(){return curr;}
    ChumListItem* getCurrentItem(){return candy[curr];}
    void setCurrentItem(const int input){curr=input;}
signals:
    void listChanged(ChumListItem* newItem,int index=-1);//if we're appending, we leave it at -1
private:
    //int curr=-1;
    int curr;
};

#endif // MODELLIST_H
