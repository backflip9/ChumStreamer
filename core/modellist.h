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
    void insert(int i,ChumListItem* value);
    void append(ChumListItem* value);
    bool find(const QString& needle);
    ChumListItem* at(int index){return candy.at(index);}
    int size(){return candy.size();}
    void clear(){candy.clear(); emit listCleared();}
    bool isEmpty(){return candy.empty();}
    //bool hasCurrentItem(){return curr!=-1;}
    bool hasCurrentItem();
    //int getCurrentItem(){return curr;}
    QList<ChumListItem* > getCurrentItem();
    virtual void setCurrentItem(const int input){curr=input;}
    
    //member
    QList<ChumListItem* > candy;
signals:
    void listChanged(ChumListItem* newItem,int index=-1);//if we're appending, we leave it at -1
    void listCleared();
    //void testSignal(){return;}
private:
    //int curr=-1;
    int curr;
};

#endif // MODELLIST_H
