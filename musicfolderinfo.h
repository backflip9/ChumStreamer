#ifndef MUSICFOLDERINFO_H
#define MUSICFOLDERINFO_H
struct musicFolderInfo
{
  musicFolderInfo(){name="";id="";}
  musicFolderInfo(QString name,QString id):name(name),id(id){}
  musicFolderInfo(QString name,QString id,bool isSong):name(name),id(id),isSong(isSong){}
  musicFolderInfo(const musicFolderInfo& other):name(other.name),id(other.id),isSong(other.isSong){}
  /*
  musicFolderInfo(const musicFolderInfo& other)
  {
    name=other.name;
    id=other.id;
    isSong=other.isSong;
  }
  */

  //members
  QString name;
  QString id;
  bool isSong;
};
#endif //MUSICFOLDERINFO_H
