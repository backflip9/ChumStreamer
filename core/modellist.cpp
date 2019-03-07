#include "modellist.h"

modelList::modelList(){}
void modelList::insert(int index,const ChumListItem* value)
{
  if(candy.size()==0 || candy.size()<=index)
  {
    candy.append(value);
  }
  candy.insert(index,value);
  emit listChanged(value,index);
}
void modelList::append(const ChumListItem* value)
{
  candy.append(value);
  emit listChanged(value);
}

bool modelList::find(const QString& needle)
{
  for(int i=0;i<candy.size();i++)
  {
    if(candy.at(i)->info->name==needle){return true;}
  }
  return false;
}
