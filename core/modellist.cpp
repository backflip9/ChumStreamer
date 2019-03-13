#include "chumlistitem.h"
#include "modellist.h"
#include <QList>

void modelList::insert(int index,ChumListItem* value)
{
  //ChumListItem* ret=new ChumListItem(*value);
  if(candy.size()==0 || candy.size()<=index)
  {
    candy.append(value);
  }
  candy.insert(index,value);
  emit listChanged(value,index);
}
void modelList::append(ChumListItem* value)
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
bool modelList::hasCurrentItem()
{
  for(int i=0;i<candy.size();i++)
  {
    if(candy[i]->selected==true)
    {
      return true;
    }
  }
  return false;
}
QList<ChumListItem* > modelList::getCurrentItem()
{
  QList<ChumListItem* > ret;
  for(int i=0;i<candy.size();i++)
  {
    if(candy[i]->selected==true)
    {
      ret.append(candy[i]);
    }
  }
  return ret;
}
