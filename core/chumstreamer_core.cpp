#include<QtNetwork/QNetworkAccessManager>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkReply>
#include<QFile>
#include<QBuffer>
//#include<QListWidgetItem>
#include<QDomDocument>
#include<QVector>
#include<QStringListModel>
#include<QMouseEvent>
#include<QMediaPlayer>
#include<QDir>
#include<QString>
#include<QUrl>
#include<QUrlQuery>
#include<QDebug>
#include<QJsonObject>
#include<QJsonDocument>
#include<algorithm>
#include "musicfolderinfo.h"
#include "chumlistitem.h"
//###include "authdialog.h"
//#include "md5.cpp";
#include "chumstreamer_core.h"
#include<time.h>

chumstreamer_core::chumstreamer_core(QWidget *parent) :
    QMainWindow(parent)
    //ui(new Ui::chumstreamer_core)
{
  srand(time(NULL));
  //ui->setupUi(this);
  if(applyFromSave())
  {
    setMusicFolders();
  }
}

chumstreamer_core::~chumstreamer_core()
{
    //delete ui;
}


QUrl chumstreamer_core::buildQueryString(QString page)
{
  QUrl result(server);
  if(page!="")
  {
    result.setPath("/rest/" + page);
  }
  QUrlQuery myQuery;
  myQuery.addQueryItem("u",Username());
  myQuery.addQueryItem("p",Password());
  myQuery.addQueryItem("c","chumstreamer_core");
  myQuery.addQueryItem("v","1.15.0");
  result.setQuery(myQuery);
  return result;
}

void chumstreamer_core::addArtists()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"addartists")){return;}
  QDomDocument myDoc;
  if(!myDoc.setContent(oneReply->readAll()))
  {
    qDebug("addartists setcontent failure");
    return;
  }
  //delete the reply so that subsequent queries won't fail?
  oneReply->deleteLater();
  oneReply=nullptr;
  qDebug("addartists success");
  //desktop
  /*
  if(player->state()==QMediaPlayer::StoppedState)
  {
    songInfoDisplay(false);
    ui->imageLabel->hide();
  }
  */
  //remove all the existing items
  //ui->artistListWidget->clear();
  //qDebug() << "cleared artistListWidget in addArtists()";
  //i guess we don't need qdomelements in the first place??
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <artists> -> many <index> tags
  QDomNode artistsTag= oneElement.firstChild();
  QDomNodeList oneNodeList=artistsTag.childNodes();
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode twoNode=oneNodeList.at(j);
    QDomNodeList twoNodeList=twoNode.childNodes();
    for(int k=0;k<twoNodeList.length();k++)
    {
      QDomNode artistNode =twoNodeList.at(k);
      artistList << artistNode.attributes().namedItem("name").nodeValue();
      //we're hardcoding the isSong attribute to be false cause getIndexes returns weird tags that are unique, and don't have the isDir attribute
      //emit newArtist(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
      //now the application can handle this with the signal emitted by modelList
      artistModel.append(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
    }
  }
  //emit artistListUpdate(artistModel);
  //ui->artistListWidget->setFocus(Qt::TabFocusReason);
}

//slot that we attach to a request to /rest/getMusicFolders that populates and displays the combobox
//according to subsonic's lingo, a music "folder" is one of the different scanned directories on the server that you can restrict access to, and a _directory_ is a subdirectory of one of those "folders"
void chumstreamer_core::getMusicFolders()
{
  qDebug() << "get music folders!!";
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"getMusicFolders")){return;}
  QDomDocument myDoc;
  //if(!myDoc.setContent(reply->readAll()))
  if(!myDoc.setContent(oneReply->readAll()))
  {
      qDebug("setcontent failure");
      return;
  }
  //delete the reply so that subsequent queries won't fail?
  oneReply->deleteLater();
  oneReply=nullptr;
  //this will have to be called manually by the child class when it wants to repopulate its musicFolderListWidget
  //ui->musicFolderListWidget->clear();
  //myFile.close();
  //i guess we don't need qdomelements in the first place??
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <artists> -> many <index> tags
  QDomNode musicFoldersTag=oneElement.firstChild();
  QDomNodeList oneNodeList=musicFoldersTag.childNodes();
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode oneMusicFolder=oneNodeList.at(j);
    ChumListItem *oneFolderCheckbox = new ChumListItem(oneMusicFolder.attributes().namedItem("name").nodeValue(),oneMusicFolder.attributes().namedItem("id").nodeValue());
    //ui->musicFolderListWidget->addItem(oneFolderCheckbox);
    oneFolderCheckbox->setFlags(oneFolderCheckbox->flags() | Qt::ItemIsUserCheckable);
    if(checkedFolders[j]=="0")
    {
      qDebug() << "checked folders string had a zero at this index; unchecked";
      oneFolderCheckbox->setCheckState(Qt::Unchecked);
    }
    else{
      qDebug() << "checked folders string had a one at this index; checked";
      oneFolderCheckbox->setCheckState(Qt::Checked);
    }
    //emit newFolder(oneFolderCheckbox);
    //directoryModel.append(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
    directoryModel.append(oneFolderCheckbox);
  }
  //populate artistListWidget somehow. actually maybe we can call this and the child can handle the signals that it emits.
  //on_pushButton_clicked();
  displayRoot();
}

void chumstreamer_core::setMusicFolders()
{
  //initialize a network request and call getMusicFolders to change the values in the combobox based on the auth details entered in the authDialog
  QNetworkReply* musicFoldersReply=manager.get(QNetworkRequest(buildQueryString("getMusicFolders")));
  connect(musicFoldersReply,&QNetworkReply::finished,this,&chumstreamer_core::getMusicFolders);
}

/*
void chumstreamer_core::on_pushButton_clicked()
{
  qDebug() << "button clicked";

  //hide things that aren't relevant anymore
  //ui->artistListWidget->clear();
  artistModel.clear();
  //handles its own conditions concerning the player's state
  songInfoDisplay(false);
  //ui->pwdLabel->hide();
  prevDirIDVec.clear();
  prevDirIDVec.push_front(musicFolderInfo("","-1"));

  qDebug() << "cleared artistListWidget in pushbutton slot";

  const QUrl originalUrl=buildQueryString("getIndexes");
  //qDebug() << "requesting at "<<myUrl.path() << myUrl.query();
  //add iterator here to loop through the folder IDs that were checked
  //QList checkboxList=ui->musicFolderListWidget->count()
  QString modelBitmask="";
  for(int counter=0;counter<ui->musicFolderListWidget->count();counter++)
  {
    if(ui->musicFolderListWidget->item(counter)->checkState()==Qt::Checked)
    {
      modelBitmask += "1";
      qDebug() << ui->musicFolderListWidget->item(counter)->text() << " was checked!!";
      //call setMusicFolders to add them to the qlistwidget. we aren't gonna pass a qstringlist by reference thru all those stack frames even if it might be easier, we're just gonna call setMusicFolders multiple times until the artist list is populated with all the relevant items
      QUrl currentUrl=originalUrl;
      QUrlQuery currentQuery(currentUrl.query());
      currentQuery.addQueryItem("musicFolderId",dynamic_cast<ChumListItem *>(ui->musicFolderListWidget->item(counter))->info->id);
      currentUrl.setQuery(currentQuery);
      qDebug("query to be sent to addArtists():");
      //qDebug(currentUrl.query());
      qDebug()<<currentUrl.query();
      //QNetworkRequest getArtistsRequest(currentUrl);
      QNetworkReply* getIndexesReply=manager.get(QNetworkRequest(currentUrl));
      connect(getIndexesReply,&QNetworkReply::finished,this,&chumstreamer_core::addArtists);
    }
    else
    {
      modelBitmask += "0";
    }
  }
  if(modelBitmask!=checkedFolders)
  {
    writeSave();
  }
}
*/

//void chumstreamer_core::displayRoot(const QString& bitmask)
void chumstreamer_core::displayRoot()
{
  prevDirIDVec.clear();
  prevDirIDVec.push_front(musicFolderInfo("","-1"));

  qDebug() << "cleared artistListWidget in pushbutton slot";

  const QUrl originalUrl=buildQueryString("getIndexes");
  //idk if you're allowed to do range-based for with qstrings
  //for(auto& i:bitmask)
  //for(int counter=0;counter<bitmask.length();counter++)
  for(int counter=0;counter<directoryModel.size();counter++)
  {
    //if(i=='1')
    if(directoryModel.at(counter)->checkState()==Qt::Checked)
    {
      //call setMusicFolders to add them to the qlistwidget. we aren't gonna pass a qstringlist by reference thru all those stack frames even if it might be easier, we're just gonna call setMusicFolders multiple times until the artist list is populated with all the relevant items
      QUrl currentUrl=originalUrl;
      QUrlQuery currentQuery(currentUrl.query());
      //this isn't working atm
      //currentQuery.addQueryItem(ui->musicFolderListWidget->item(counter)->getArtistID());
      //currentQuery.addQueryItem("musicFolderId",dynamic_cast<ChumListItem *>(ui->musicFolderListWidget->item(counter))->info->id);
      currentQuery.addQueryItem("musicFolderId",dynamic_cast<ChumListItem *>(directoryModel.at(counter))->info->id);
      currentUrl.setQuery(currentQuery);
      QNetworkReply* getIndexesReply=manager.get(QNetworkRequest(currentUrl));
      connect(getIndexesReply,&QNetworkReply::finished,this,&chumstreamer_core::addArtists);
    }
  }
}

void chumstreamer_core::setDir(const QString& dir)
{
  QUrl currentUrl = buildQueryString("getMusicDirectory");
  QUrlQuery currentQuery(currentUrl.query());
  currentQuery.addQueryItem("id",dir);
  currentUrl.setQuery(currentQuery);
  qDebug() << "requesting at "<<currentUrl.toString();// << currentUrl.query();
  QNetworkRequest getDirRequest(currentUrl);
  QNetworkReply* getMusicDirReply=manager.get(getDirRequest);
  connect(getMusicDirReply,&QNetworkReply::finished,this,&chumstreamer_core::displayDir);
}

void chumstreamer_core::displayDir()//virtual
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"displayDir")){return;}
  QDomDocument myDoc;
  //if(!myDoc.setContent(reply->readAll()))
  if(!myDoc.setContent(oneReply->readAll()))
  {
      qDebug("displayDir setcontent failure");
      return;
  }
  oneReply->deleteLater();
  oneReply=nullptr;

  qDebug("displayDir success");
  songInfoDisplay(false);
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <directory> -> many <child> tags
  QDomNode directoryTag=oneElement.firstChild();
  QDomNodeList oneNodeList=directoryTag.childNodes();
  bool grabAlbumArt=false;
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode oneDir=oneNodeList.at(j);
    //ChumListItem *chumOneFolderCheckbox = new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false");
    //emit newArtist(new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false"));
    artistModel.append(new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false"));
    //ui->artistListWidget->addItem(chumOneFolderCheckbox);

    //add the regular QListWidgetItem to the artist/song list. this involves setting the song ID as the tooltip value
    if(!grabAlbumArt && !oneDir.attributes().namedItem("coverArt").isNull() && oneDir.attributes().namedItem("isDir").nodeValue()=="false" && player->state()==QMediaPlayer::StoppedState)
    {
      setImage(oneDir.attributes().namedItem("coverArt").nodeValue());
      grabAlbumArt=true;
    }
  }
}

void chumstreamer_core::setImage(QString songID)
{
  if(songID==currentAlbumArt){qDebug()<<"songID was equal to currentAlbumArt";return;}
  qDebug() << "songID: "<<songID;
  qDebug() << "currentAlbumArt: "<<currentAlbumArt;
  QUrl imageUrl=buildQueryString("getCoverArt");
  QUrlQuery imageQuery(imageUrl.query());
  imageQuery.addQueryItem("id",songID);
  imageQuery.addQueryItem("size","190");
  imageUrl.setQuery(imageQuery);
  currentAlbumArt=songID;
  //QNetworkRequest imageRequest(imageUrl);
  QNetworkReply* imageReply=manager.get(QNetworkRequest(imageUrl));
  connect(imageReply,&QNetworkReply::finished,this,&chumstreamer_core::displayImage);
}

void chumstreamer_core::displayImage()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"displayImage")){return;}
  //qDebug() << "displayImage with no network error";
  QPixmap oneImage;
  oneImage.loadFromData(oneReply->readAll());
  oneReply->deleteLater();
  oneReply=nullptr;
  //this is virtual!
  imageFromPixmap(oneImage);
  /*
  ui->imageLabel->setPixmap(oneImage);
  ui->imageLabel->show();
  */
}




/*
void chumstreamer_core::on_nextButton_clicked()
{
  //gets the child dirs from whatever's currently being displayed in artistListWidget
  qDebug() << "current item: " << ui->artistListWidget->currentItem()->text();
  qDebug() << "current data: " << ui->artistListWidget->currentItem()->data(Qt::ToolTipRole).toString();
  //change the artists that are displayed in the qlistwidget in a single line!!
  setDir(ui->artistListWidget->findItems(ui->artistListWidget->currentItem()->text(),Qt::MatchExactly).first()->data(Qt::ToolTipRole).toString());
  ui->pwdLabel->setText(ui->artistListWidget->currentItem()->text());
  ui->pwdLabel->show();
}
*/


bool chumstreamer_core::handleNetworkError(QNetworkReply* reply,QString funcName)
{
  //to avoid copypaste
  if(reply->error())
  {
    if(!funcName.isEmpty())
    {
      qDebug() << "in function " << funcName << ": ";
    }
    qDebug() << "reply error number: " << reply->error();
    reply->deleteLater();
    reply=nullptr;
    return true;
  }
  return false;
}

//void chumstreamer_core::on_artistListWidget_itemDoubleClicked(QListWidgetItem *item)
void chumstreamer_core::nextDir(QListWidgetItem *item)
{
  //gets the child dirs from whatever's currently being displayed in artistListWidget
  //old version without chumlistitem
  if(dynamic_cast<ChumListItem* >(item)->info->isSong)
  {
    return addToPlaylistFromSlot(false);
  }
  QString artistID=dynamic_cast<ChumListItem* >(item)->info->id;
  //prevDirIDVec.push_front(musicFolderInfo(item->text(),item->data(Qt::ToolTipRole).toString()));
  prevDirIDVec.push_front(musicFolderInfo(item->text(),artistID));
  setDir(artistID);
  //these are desktop specific anyway
  /*
  ui->pwdLabel->setText(item->text());
  ui->pwdLabel->show();
  */
}

/*
void chumstreamer_core::on_backButton_clicked()
{
  if(prevDirIDVec.size()==1||prevDirIDVec.size()==2){on_pushButton_clicked();return;}
  setDir(prevDirIDVec.at(1).id);
  //ui->pwdLabel->setText(prevDirIDVec.at(1).name);
  prevDirIDVec.pop_front();
}
*/

void chumstreamer_core::goBack()
{
  //if(prevDirIDVec.size()==1||prevDirIDVec.size()==2){on_pushButton_clicked();return;}
  if(prevDirIDVec.size()==1||prevDirIDVec.size()==2){displayRoot();return;}
  setDir(prevDirIDVec.at(1).id);
  //ui->pwdLabel->setText(prevDirIDVec.at(1).name);
  prevDirIDVec.pop_front();
}


//all keyboard/mouse things are obviously desktop specific
//void chumstreamer_core::keyPressTest(QKeyEvent* oneKey)
/*
void chumstreamer_core::keyPressEvent(QKeyEvent* oneKey)
{
  switch(oneKey->key())
  {
    case Qt::Key_A:
    case Qt::Key_E:
    case Qt::Key_J:
    case Qt::Key_Down:
    case Qt::Key_K:
    case Qt::Key_Up:
    case Qt::Key_Return:
    case Qt::Key_H:
      ui->artistListWidget->setFocus(Qt::TabFocusReason);
      break;
    default:
      qDebug() << "keyboard key: "<<oneKey->key();
      break;
  }
}
void chumstreamer_core::mouseReleaseEvent(QMouseEvent* oneKey)
{
  qDebug() << "some mouse button: " <<oneKey->button();
}

void chumstreamer_core::on_addButton_clicked()
{
    //int addPosition=0;
    if(ui->frontRadio->isChecked())
    {
        //addPosition=0;
        qDebug() << "front!"<< "current Item is: " << ui->artistListWidget->currentItem()->text();
    }
    else if(ui->backRadio->isChecked())
    {
        //addPosition=ui->playlistListWidget->count()-1;
        qDebug() << "back!" << "current Item is: " << ui->artistListWidget->currentItem()->text();
        //ui->playlistListWidget->addItem(ui->artistListWidget->currentItem());
        ui->playlistListWidget->addItem(ui->artistListWidget->currentItem()->text());
        ui->playlistListWidget->item(ui->playlistListWidget->count()-1)->setData(Qt::ToolTipRole,ui->artistListWidget->currentItem()->data(Qt::ToolTipRole));

    }
    else{
        qDebug() << "neither!";
        return;
    }
}
*/


void chumstreamer_core::on_playButton_clicked()
{
  if(player->state()==QMediaPlayer::PlayingState)
  {
    player->pause();
    qDebug() << "paused!";
  }
  else if(player->state()==QMediaPlayer::PausedState)
  {
    player->play();
    qDebug() << "playing!";
  }
  else if(player->state()==QMediaPlayer::StoppedState)
  {
    qDebug() << "playpause: neither!";
    //on_nextTrackButton_clicked();
    playNext();
    return;
  }
}

void chumstreamer_core::streamSong()
{
  //ui->playlistListWidget->item(getCurrentPlaylistIndex())->setForeground(Qt::red);
  playlistModel.candy.at(getCurrentPlaylistIndex())->info->status=playing;
  //QString songID=dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(getCurrentPlaylistIndex()))->info->id;
  QString songID=playlistModel.candy.at(getCurrentPlaylistIndex())->info->id;
  QUrl streamUrl=buildQueryString("stream");
  QUrlQuery currentQuery(streamUrl.query());
  currentQuery.addQueryItem("id",songID);
  streamUrl.setQuery(currentQuery);
  QNetworkRequest streamRequest(streamUrl);
  player->setMedia(streamRequest);
  qDebug() << "set media as streamRequest";
  player->play();
  //wait until the player definitely isn't stopped to call these
  setImage(songID);
  getSongInfo(songID);
  connect(player,&QMediaPlayer::mediaStatusChanged,this,&chumstreamer_core::notifySongEnd);
}


//i'll try to do this later
/*
void chumstreamer_core::streamSongQIO()
{
  //ui->playlistListWidget->item(currentPlaylistIndex)->setForeground(Qt::red);
  ui->playlistListWidget->item(getCurrentPlaylistIndex())->setForeground(Qt::red);
  QUrl streamUrl=buildQueryString("stream");
  QUrlQuery currentQuery(streamUrl.query());
  //currentQuery.addQueryItem("id",dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(currentPlaylistIndex))->info->id);
  currentQuery.addQueryItem("id",dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(getCurrentPlaylistIndex()))->info->id);
  streamUrl.setQuery(currentQuery);
  QNetworkRequest streamRequest(streamUrl);
  QNetworkReply* streamReply=manager.get(streamRequest);
  connect(streamReply,&QNetworkReply::readyRead,this,&chumstreamer_core::bufferStream);
  //player->setMedia(streamRequest);
  //qDebug() << "set media as streamRequest";
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}


  //we'll just play one song at a time for now
  //connect(player,&QMediaPlayer::mediaStatusChanged,this,&chumstreamer_core::notifySongEnd);
}
*/

void chumstreamer_core::bufferStream()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  //currentSong.append(oneReply->readAll());
  QMediaContent blankContent;
  player->setMedia(blankContent,oneReply);
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}
}

void chumstreamer_core::notifySongEnd()
{
  //a cleaner, more agnostic way to do this. we could just refer to the member var
  QMediaPlayer* oneMedia=qobject_cast<QMediaPlayer* >(sender());
  if(oneMedia->mediaStatus()==QMediaPlayer::EndOfMedia)
  {
    qDebug() << "media has ended!";
    //return on_nextTrackButton_clicked();
    playNext();
    return;
    //if(chooseNext()){return streamSong();}
  }
  else
  {
    qDebug() << "media has not ended. status: " <<oneMedia->mediaStatus();
  }
}

/*
bool chumstreamer_core::chooseNext()
{
  if(ui->playlistListWidget->count()==0)
  {
    qDebug()<<"chooseNext: playlist is empty, returning false";
    return false;
  }
  //check if there's a song that's currently playing, and set it to cyan
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    if(ui->playlistListWidget->item(i)->foreground()==Qt::red)
    {
      ui->playlistListWidget->item(i)->setForeground(Qt::cyan);
    }
  }
  //find the first non-cyan song
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    if(ui->playlistListWidget->item(i)->foreground()!=Qt::cyan)
    {
      return true;
    }
  }
  //if they're all cyan, then our playlist has concluded
  if(repeating())
  {//if we're repeating, gray everything out and start again
    grayOutPlaylist();
    return chooseNext();
  }
  else
  {
    on_pushButton_2_clicked();
    return false;
  }
}
*/
bool chumstreamer_core::chooseNext()
{
  if(playlistModel.size()==0)
  {
    qDebug()<<"chooseNext: playlist is empty, returning false";
    return false;
  }
  //check if there's a song that's currently playing, and set it to cyan
  for(int i=0;i<playlistModel.size();i++)
  {
    if(playlistModel.at(i)->info->status==playing)
    {
      playlistModel.at(i)->info->status=played;
    }
  }
  //find the first non-cyan song
  for(int i=0;i<playlistModel.size();i++)
  {
    if(playlistModel.at(i)->info->status==unplayed)
    {
      return true;
    }
  }
  //if they're all cyan, then our playlist has concluded
  if(repeating())
  {//if we're repeating, gray everything out and start again
    grayOutPlaylist();
    return chooseNext();
  }
  else
  {
    //stop
    //on_pushButton_2_clicked();
    stop();
    return false;
  }
}


/*
void chumstreamer_core::on_playPauseButton_clicked()
{
}
*/

void chumstreamer_core::stop()
{

  player->stop();
  qDebug() << "player stopped!";
  grayOutPlaylist();
}

/*
void chumstreamer_core::on_pushButton_2_clicked()
{
  stop();
}
*/

void chumstreamer_core::grayOutPlaylist()
{
  for(int i=0;i<playlistModel.candy.size();i++)
  {
    //ui->playlistListWidget->item(i)->setForeground(Qt::lightGray);
    playlistModel.at(i)->info->status=unplayed;
  }
}


/*
void chumstreamer_core::on_artistListWidget_currentRowChanged(int currentRow)
{
  if(player->state()!=QMediaPlayer::StoppedState){return;}
  qDebug() << "on_artistListWidget_currentRowChanged: state: "<<player->state();
  //dumb hack to prevent crashes that occur when an item is double clicked, and the list is cleared, and as a result this method returns -1
  if(currentRow==-1){return;}
  if(dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->isSong==false){return;}
  ui->songTitleLabel->setText(ui->artistListWidget->currentItem()->text());
  getSongInfo(dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->id);
}
*/

void chumstreamer_core::getSongInfo(QString songID)
{
  QUrl songRequest=buildQueryString("getSong");
  QUrlQuery currentQuery(songRequest.query());
  currentQuery.addQueryItem("id",songID);
  songRequest.setQuery(currentQuery);
  //qDebug()<<"song query: "<<songRequest.toString();
  QNetworkRequest getSongInfoRequest(songRequest);
  QNetworkReply* songInfoReply=manager.get(getSongInfoRequest);
  connect(songInfoReply,&QNetworkReply::finished,this,&chumstreamer_core::setSongInfo);
}

void chumstreamer_core::setSongInfo()//pure virtual
{
  /*
  //if(player->state()!=QMediaPlayer::StoppedState){return;}
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"setsonginfo")){return;}
  QDomDocument myDoc;
  //if(!mydoc.setcontent(reply->readall()))
  if(!myDoc.setContent(oneReply->readAll()))
  {
      qDebug("setsonginfo setcontent failure");
      return;
  }
  oneReply->deleteLater();
  oneReply=nullptr;
  qDebug("setsonginfo success");
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <artists> -> many <index> tags
  QDomNode songTag= oneElement.firstChild();
  ui->songTitleLabel->setText(songTag.attributes().namedItem("title").nodeValue());
  ui->albumNameLabel->setText(songTag.attributes().namedItem("album").nodeValue());
  ui->albumArtistLabel->setText(songTag.attributes().namedItem("artist").nodeValue());
  songInfoDisplay(true);
  */
}

//desktop specific
/*
void chumstreamer_core::songInfoDisplay(bool show)
{
  if(show)
  {
    ui->songTitleLabel->show();
    ui->albumNameLabel->show();
    ui->albumArtistLabel->show();
    ui->imageLabel->show();
  }
  else
  {
    if(player->state()!=QMediaPlayer::StoppedState){return;}
    ui->songTitleLabel->hide();
    ui->albumNameLabel->hide();
    ui->albumArtistLabel->hide();
    ui->imageLabel->hide();
  }
}
*/



//optionalChum is only going to be passed to this function when it's being called by playlistAddRecursive()
void chumstreamer_core::addToPlaylistFromSlot(bool prepend,ChumListItem* optionalChum)//=NULL
{
  //but only if a certain item is selected
  if(optionalChum==NULL)
  {
    //if(dynamic_cast<clickableList *>(ui->artistListWidget)->hasCurrentItem())
    if(artistModel.hasCurrentItem())
    {
      //playlistAddFromChumListItem(dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem()),prepend);
      playlistAddFromChumListItem(dynamic_cast<ChumListItem *>(artistModel.getCurrentItem()),prepend);
    } //there's no else for this cause we have nothing to do if no items are highlighted and the user presses A or E
  }
  else
  {
    playlistAddFromChumListItem(optionalChum,prepend);
  }
}

/*
void chumstreamer_core::playlistAddFromChumListItem(ChumListItem* oneChum,bool prepend)
{
  if(oneChum->info->isSong)
  {
    //and only if that item doesn't already exist in the playlist
    //if(ui->playlistListWidget->findItems(oneChum).size()==0)
    if(ui->playlistListWidget->findItems(oneChum->info->name,Qt::MatchExactly).size()==0)
    {
      //apparently we have to do this cause *oneChum points to an existing ChumListItem that already exists in another QListWidget
      ChumListItem* newChum=new ChumListItem(*oneChum);
      if(prepend && ui->playlistListWidget->count()!=0)
      { 
        //qDebug() << "prending at: "<<index;
        int offset=1;
        if(hasRed()){offset=0;}
        //ui->playlistListWidget->insertItem(getCurrentPlaylistIndex()+offset,newChum);
        //emit newArtist(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
        playlistModel.insert(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false),getCurrentPlaylistIndex()+offset);
        //ui->playlistListWidget->insertItem(getCurrentPlaylistIndex()+1,newChum);
      }
      else{
        ui->playlistListWidget->addItem(newChum);
      }
    }
  }
  else
  {
    firstRecursiveRequest(oneChum->info->id);
  }
}
*/

void chumstreamer_core::playlistAddFromChumListItem(ChumListItem* oneChum,bool prepend)
{
  if(oneChum->info->isSong)
  {
    //and only if that item doesn't already exist in the playlist
    //if(ui->playlistListWidget->findItems(oneChum).size()==0)
    if(!playlistModel.find(oneChum->info->name))
    {
      //apparently we have to copy this object cause *oneChum points to an existing ChumListItem that already exists in another QListWidget
      ChumListItem* newChum=new ChumListItem(*oneChum);
      if(prepend && playlistModel.isEmpty())
      {
        //qDebug() << "prending at: "<<index;
        int offset=1;
        if(hasRed()){offset=0;}
        //ui->playlistListWidget->insertItem(getCurrentPlaylistIndex()+offset,newChum);
        //emit newArtist(new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
        //playlistModel.insert(newChum,getCurrentPlaylistIndex()+offset);
        playlistModel.insert(getCurrentPlaylistIndex()+offset,newChum);
        //ui->playlistListWidget->insertItem(getCurrentPlaylistIndex()+1,newChum);
      }
      else{
        //ui->playlistListWidget->addItem(newChum);
        playlistModel.append(newChum);
      }
    }
  }
  else
  {
    firstRecursiveRequest(oneChum->info->id);
  }
}

void chumstreamer_core::firstRecursiveRequest(QString id)
{
  QUrl firstUrl(buildQueryString("getMusicDirectory"));
  QUrlQuery firstUrlQuery(firstUrl);
  firstUrlQuery.addQueryItem("id",id);
  firstUrl.setQuery(firstUrlQuery);
  qDebug()<<"firstRecursiveRequest: "<<firstUrl.toString();
  QNetworkRequest firstCallRequest(firstUrl);
  QNetworkReply* firstCallReply=manager.get(firstCallRequest);
  connect(firstCallReply,&QNetworkReply::finished,this,&chumstreamer_core::playlistAddRecursive);
}

void chumstreamer_core::playlistAddRecursive()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"addartists")){return;}
  QDomDocument myDoc;
  if(!myDoc.setContent(oneReply->readAll()))
  {
      qDebug("addartists setcontent failure");
      return;
  }
  //delete the reply so that subsequent queries won't fail?
  oneReply->deleteLater();
  oneReply=nullptr;
  qDebug("playlistAddRecursive success");

  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <directory> -> many <child> tags
  QDomNode directoryTag=oneElement.firstChild();
  QDomNodeList oneNodeList=directoryTag.childNodes();
  if(latestPrepend)
  {
    for(int j=oneNodeList.length();j>=0;j--)
    {
      playlistAddFromNode(oneNodeList.at(j));
    }
  }
  else
  {
    for(int j=0;j<oneNodeList.length();j++)
    {
      playlistAddFromNode(oneNodeList.at(j));
    }
  }
}

void chumstreamer_core::playlistAddFromNode(QDomNode oneDir)
{
  //at first, this code only ran if the node was a song, but then i realized that addToPlaylistFromSlot can just handle either case
  ChumListItem* tempSong=new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false");
  addToPlaylistFromSlot(latestPrepend,tempSong);
}

void chumstreamer_core::on_artistListWidget_playlistAppend()
{
  latestPrepend=false;
  addToPlaylistFromSlot(false);
}

void chumstreamer_core::on_artistListWidget_playlistPrepend()
{
  latestPrepend=true;
  addToPlaylistFromSlot(true);
}

//desktop
/*
void chumstreamer_core::on_artistListWidget_ReturnKey()
{
  return on_artistListWidget_itemDoubleClicked(ui->artistListWidget->currentItem());
}

void chumstreamer_core::on_artistListWidget_Back()
{
  return on_backButton_clicked();
}

void chumstreamer_core::on_volumeSlider_valueChanged(int value)
{
  player->setVolume(value);
}

*/

void chumstreamer_core::playNext()
{
  if(chooseNext()){return streamSong();}
  else
  {
    qDebug()<<"on_nextTrackButton_clicked(): chooseNext() returned false";
    stop();
    //return on_pushButton_2_clicked();
  }
}

/*
void chumstreamer_core::on_nextTrackButton_clicked()
{
  return playNext();
}

void chumstreamer_core::on_artistListWidget_Clear()
{
  clearPlaylist();
}
*/

void chumstreamer_core::clearPlaylist()
{
  playlistModel.candy.clear();
}

/*
int chumstreamer_core::getCurrentPlaylistIndex()
{
  //case 1: there's already a song that's red, so we'll return that
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    if(ui->playlistListWidget->item(i)->foreground()==Qt::red){return i;}
  }
  if(random())
  {
    QVector<int> viableVec;
    for(int i=0;i<ui->playlistListWidget->count();i++)
    {
      if(ui->playlistListWidget->item(i)->foreground()!=Qt::cyan)
      {
        viableVec.push_back(i);
      }
    }
    //else the playlist is empty or something
    if(viableVec.size()==0){return 0;}
    return viableVec[rand() % viableVec.size()];
  }
  //case 2: there aren't any red songs, so return the first non-cyan song
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    if(ui->playlistListWidget->item(i)->foreground()!=Qt::cyan){return i;}
  }
  //else the playlist is empty or something
  return 0;
}
*/

int chumstreamer_core::getCurrentPlaylistIndex()
{
  //case 1: there's already a song that's red, so we'll return that
  for(int i=0;i<playlistModel.size();i++)
  {
    //if(ui->playlistListWidget->item(i)->foreground()==Qt::red){return i;}
    if(playlistModel.candy.at(i)->info->status==playing){return i;}
  }
  if(random())
  {
    QVector<int> viableVec;
    for(int i=0;i<playlistModel.size();i++)
    {
      //if(ui->playlistListWidget->item(i)->foreground()!=Qt::cyan)
      if(playlistModel.candy.at(i)->info->status==played)
      {
        viableVec.push_back(i);
      }
    }
    //else the playlist is empty or something
    if(viableVec.size()==0){return 0;}
    return viableVec[rand() % viableVec.size()];
  }
  //case 2: there aren't any red songs, so return the first non-cyan song
  else{
    //for(int i=0;i<ui->playlistListWidget->count();i++)
    for(int i=0;i<playlistModel.size();i++)
    {
      //if(ui->playlistListWidget->item(i)->foreground()!=Qt::cyan){return i;}
      if(playlistModel.candy.at(i)->info->status!=played){return i;}
    }
  }
  //else the playlist is empty or something
  return 0;
}

//todo: make sure toggleRepeating() writes to the save file. i can't think of a time when we wouldn't want to do that
/*
void chumstreamer_core::on_repeatToggleButton_clicked()
{
  toggleRepeating();
  writeSave();
}
*/

//desktop specific
void chumstreamer_core::toggleRepeating()
{
  repeatingEnabled^=true;
  writeSave();
}

//if we can't predict what the repeat button is currently set to
void chumstreamer_core::toggleRepeating(bool staticBool)
{
  repeatingEnabled=staticBool;
  writeSave();
}

bool chumstreamer_core::repeating()
{
  //return ui->repeatToggleButton->text()=="repeat all";
  return repeatingEnabled;
}

/*
void chumstreamer_core::on_volumeSlider_sliderReleased()
{
  writeSave();
}

void chumstreamer_core::on_randomToggleButton_clicked()
{
  toggleRandom();
  writeSave();
}

*/
bool chumstreamer_core::random()
{
  //return ui->randomToggleButton->text()=="random on";
  return randomEnabled;
}
void chumstreamer_core::toggleRandom()
{
  randomEnabled^=true;
}
//if we can't predict what the repeat button is currently set to
void chumstreamer_core::toggleRandom(bool staticBool)
{
  randomEnabled=staticBool;
}
void chumstreamer_core::on_clearButton_clicked()
{
  //on_artistListWidget_Clear();
  clearPlaylist();
}

bool chumstreamer_core::hasRed()
{
  //for(int i=0;i<ui->playlistListWidget->count();i++)
  for(int i=0;i<playlistModel.size();i++)
  {
    //if(ui->playlistListWidget->item(i)->foreground()==Qt::red){return true;}
    if(playlistModel.candy.at(i)->info->status==playing){return true;}
  }
  return false;
}

void chumstreamer_core::writeSave()
{
  QFile tmp(cacheFilePath);
  if(false){}
  qDebug() << "creating json save file...";
  QJsonObject loginInfo;
  loginInfo["username"]=Username();
  loginInfo["password"]=Password();
  loginInfo["server"]=Server().toString();
  //loginInfo["volume"]=ui->volumeSlider->value();
  loginInfo["random"]=random()?"1":"0";
  loginInfo["repeating"]=repeating()?"1":"0";
  QString folderBitmask="";
  //for(int counter=0;counter<ui->musicFolderListWidget->count();counter++)
  for(int counter=0;counter<directoryModel.size();counter++)
  {
    //idk if this works for the chumlistitems that we put into this model
    if(directoryModel.candy.at(counter)->checkState()==Qt::Checked)
    {
      folderBitmask+="1";
    }
    else{
      folderBitmask+="0";
    }
  }
  loginInfo["checkedFolders"]=folderBitmask;
  checkedFolders=folderBitmask;
  if(tmp.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    //QJsonDocument docToWrite(loginInfo);
    tmp.write(QJsonDocument(loginInfo).toJson());
    tmp.close();
    qDebug() << "wrote to save file: " << cacheFilePath;
  }
  else{
    qDebug() << "couldn't open save file, even though it doesn't exist";
  }
}

bool chumstreamer_core::applyFromSave()
{
  QFile saveFile(cacheFilePath);
  if(saveFile.exists())
  {
    if(saveFile.open(QIODevice::ReadOnly))
    {
      QJsonObject saveData(QJsonDocument().fromJson(saveFile.readAll()).object());
      QJsonDocument currentDoc;
      currentDoc.fromJson(saveFile.readAll());
      saveFile.close();
      Server()=saveData["server"].toString();
      Username()=saveData["username"].toString();
      Password()=saveData["password"].toString();
      //ui->volumeSlider->setValue(saveData["volume"].toInt());
      checkedFolders=saveData["checkedFolders"].toString();
      //if(saveData["repeating"]=="1"){toggleRepeating(true);}
      toggleRepeating(saveData["repeating"]=="1");
      toggleRandom(saveData["random"]=="1");
      qDebug() << "created QJsonObject from file and applied to main object members";
      return true;
    }
    else
    {
      qDebug() << "couldn't open save file, even though it exists";
    }
  }
  else{
    qDebug() << "save file didn't exist, couldn't apply";
  }
  return false;
}
