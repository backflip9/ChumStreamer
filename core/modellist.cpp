#include "chumlistitem.h"
#include "modellist.h"
#include <QList>

void modelList::insert(int index,const ChumListItem* value)
{
  ChumListItem* ret=new ChumListItem(*value);
  if(candy.size()==0 || candy.size()<=index)
  {
    candy.append(ret);
  }
  candy.insert(index,ret);
  emit listChanged(ret,index);
}
void modelList::append(const ChumListItem* value)
{
  ChumListItem* ret=new ChumListItem(*value);
  candy.append(ret);
  emit listChanged(ret);
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
