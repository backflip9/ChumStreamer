#ifndef MUSICFOLDERINFO_H
#define MUSICFOLDERINFO_H
enum songStatus{played,playing,unplayed}
struct musicFolderInfo
{
  musicFolderInfo(){name="";id="";}
  //for when we don't care about isSong
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
  songStatus status=unplayed;
  bool selected=false;
};
#endif //MUSICFOLDERINFO_H
