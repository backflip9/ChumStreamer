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
#include "../core/chumlistitem.h"
#include "authdialog.h"
//#include "md5.cpp";
#include "../core/chumstreamer_core.h"
#include "chumstreamer_desktop.h"
#include "ui_chumstreamer_desktop.h"
#include<time.h>

chumstreamer_desktop::chumstreamer_desktop(QWidget *parent) :
    chumstreamer_core(parent),
    ui(new Ui::chumstreamer_desktop)
{
  ui->setupUi(this);
  //ensure that the models stay in sync with the list widgets


  connect(directoryModel,&modelList::listChanged,this,&chumstreamer_core::dirModelMirror);
  connect(artistModel,&modelList::listChanged,this,&chumstreamer_core::artistModelMirror);
  connect(playlistModel,&modelList::listChanged,this,&chumstreamer_core::playlistModelMirror);


  //omfg an actual working lambda
  connect(directoryModel,&modelList::listCleared,[this](){ui->musicFolderListWidget->clear();});
  connect(artistModel,&modelList::listCleared,[this](){ui->artistListWidget->clear();});
  connect(playlistModel,&modelList::listCleared,[this](){ui->playlistListWidget->clear();});

  setRepeatText();
  setRandomText();


  filePath = QDir::homePath() + "/Documents/tmpDownload.xml";
  //cacheFilePath=QDir::homePath() + "/chumstreamer_desktop.json";
  //ui->imageLabel->hide();
  ui->pwdLabel->hide();
  ui->nextButton->hide();
  ui->playPauseButton->hide();
  ui->volumeSlider->setTickInterval(2);
  ui->volumeSlider->setRange(0,100);
  //ui->volumeSlider->setValue(DEFAULT_VOLUME);

  //hide individual song info(should be blank if we're just starting up)
  songInfoDisplay(false);
  ui->imageLabel->hide();
  player->setVolume(ui->volumeSlider->value());
  //copypaste from authDialog class
  //populate the members with auth info from the dat file, if it exists
  /*
  if(chumstreamer_core::applyFromSave())
  {
    setMusicFolders();
  }
  */
  /*
  if(QDir(QDir::homePath()).exists(QFile(cacheFilePath).fileName()))
  {
      qDebug() << "file exists in homePath()";
      QFile dat(cacheFilePath);
      if(dat.open(QIODevice::ReadOnly))
      {
          Server()=QUrl(dat.readLine().trimmed());
          Username()=dat.readLine().trimmed();
          Password()=dat.readLine().trimmed();
          dat.close();
          setMusicFolders();
          return;
      }
      else
      {
          qDebug() << "couldn't open dat as readonly";
      }
  }
  else
  {
      qDebug() << "dat didn't exist";
  }
  */
}

chumstreamer_desktop::~chumstreamer_desktop()
{
    delete ui;
}


/*
void chumstreamer_desktop::addArtists()
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
  if(player->state()==QMediaPlayer::StoppedState)
  {
    songInfoDisplay(false);
    ui->imageLabel->hide();
  }
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
      ChumListItem *newArtist= new ChumListItem(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false);
      ui->artistListWidget->addItem(newArtist);
      //qDebug() << "color of newly added item: "<<ui->artistListWidget->item(0)->foreground().color();


      //saving cause it's a silly way to do things
      //ui->musicTabWidget->widget(artistTabIndex)->findChildren<QListWidget* >(artistListWidgetName).first()->item(ui->musicTabWidget->widget(artistTabIndex)->findChildren<QListWidget* >(artistListWidgetName).first()->count()-1)->setData(Qt::ToolTipRole,artistNode.attributes().namedItem("id").nodeValue());
    }
  }
  ui->artistListWidget->setFocus(Qt::TabFocusReason);
}
*/

/*
void chumstreamer_desktop::getMusicFolders()
{
  qDebug() << "get music folders!!";
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"getMusicFolders")){return;}
  //slot that we attach to a request to /rest/getMusicFolders that populates and displays the combobox
  //according to subsonic's lingo, a music "folder" is one of the different scanned directories on the server that you can restrict access to, and a _directory_ is a subdirectory of one of those "folders"
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
  ui->musicFolderListWidget->clear();
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
    ui->musicFolderListWidget->addItem(oneFolderCheckbox);
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
  }
  on_pushButton_clicked();
}
*/

/*
void chumstreamer_desktop::setMusicFolders()
{
  //initialize a network request and call getMusicFolders to change the values in the combobox based on the auth details entered in the authDialog
  QNetworkReply* musicFoldersReply=manager.get(QNetworkRequest(buildQueryString("getMusicFolders")));
  connect(musicFoldersReply,&QNetworkReply::finished,this,&chumstreamer_desktop::getMusicFolders);
}
*/


void chumstreamer_desktop::on_configureButton_clicked()
{
  authDialog authD;
  authD.setParentClass(this);
  //we shouldn't have to call this since we're gonna call applyFromSave on startup
  //authD.setAuthInfo();
  authD.setTextBoxes();
  authD.setModal(true);
  authD.exec();
  setMusicFolders();
  qDebug() << server.toString();
  qDebug() << username;
  qDebug() << password;
}



void chumstreamer_desktop::on_pushButton_clicked()
{
  displayRoot();
  /*
  QString modelBitmask="";
  for(int counter=0;counter<ui->musicFolderListWidget->count();counter++)
  {
    if(ui->musicFolderListWidget->item(counter)->checkState()==Qt::Checked)
    {
      modelBitmask += "1";
    }
    else{modelBitmask+="0";}
  }
  */
  //displayRoot(modelBitmask);
  /*
  qDebug() << "button clicked";

  //hide things that aren't relevant anymore
  ui->artistListWidget->clear();
  //handles its own conditions concerning the player's state
  songInfoDisplay(false);
  ui->pwdLabel->hide();
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
      //this isn't working atm
      //currentQuery.addQueryItem(ui->musicFolderListWidget->item(counter)->getArtistID());
      currentQuery.addQueryItem("musicFolderId",dynamic_cast<ChumListItem *>(ui->musicFolderListWidget->item(counter))->info->id);
      currentUrl.setQuery(currentQuery);
      qDebug("query to be sent to addArtists():");
      //qDebug(currentUrl.query());
      qDebug()<<currentUrl.query();
      //QNetworkRequest getArtistsRequest(currentUrl);
      QNetworkReply* getIndexesReply=manager.get(QNetworkRequest(currentUrl));
      connect(getIndexesReply,&QNetworkReply::finished,this,&chumstreamer_desktop::addArtists);
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
  */
}

/*
void chumstreamer_desktop::setDir(const QString& dir)
{
  QUrl currentUrl = buildQueryString("getMusicDirectory");
  QUrlQuery currentQuery(currentUrl.query());
  currentQuery.addQueryItem("id",dir);
  currentUrl.setQuery(currentQuery);
  qDebug() << "requesting at "<<currentUrl.toString();// << currentUrl.query();
  QNetworkRequest getDirRequest(currentUrl);
  QNetworkReply* getMusicDirReply=manager.get(getDirRequest);
  connect(getMusicDirReply,&QNetworkReply::finished,this,&chumstreamer_desktop::displayDir);
}

void chumstreamer_desktop::displayDir()
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
  ui->artistListWidget->clear();
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <directory> -> many <child> tags
  QDomNode directoryTag=oneElement.firstChild();
  QDomNodeList oneNodeList=directoryTag.childNodes();
  bool grabAlbumArt=false;
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode oneDir=oneNodeList.at(j);
    ChumListItem *chumOneFolderCheckbox = new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false");
    ui->artistListWidget->addItem(chumOneFolderCheckbox);

    //add the regular QListWidgetItem to the artist/song list. this involves setting the song ID as the tooltip value
    if(!grabAlbumArt && !oneDir.attributes().namedItem("coverArt").isNull() && oneDir.attributes().namedItem("isDir").nodeValue()=="false" && player->state()==QMediaPlayer::StoppedState)
    {
      setImage(oneDir.attributes().namedItem("coverArt").nodeValue());
      grabAlbumArt=true;
    }
  }
}
*/

/*
void chumstreamer_desktop::setImage(QString songID)
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
  connect(imageReply,&QNetworkReply::finished,this,&chumstreamer_desktop::displayImage);
}
*/

//this is shorter and calls a virtual function now
/*
void chumstreamer_desktop::displayImage()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"displayImage")){return;}
  //qDebug() << "displayImage with no network error";
  QPixmap oneImage;
  oneImage.loadFromData(oneReply->readAll());
  oneReply->deleteLater();
  oneReply=nullptr;
  ui->imageLabel->setPixmap(oneImage);
  ui->imageLabel->show();
}
*/



/*
void chumstreamer_desktop::on_nextButton_clicked()
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


/*
bool chumstreamer_desktop::handleNetworkError(QNetworkReply* reply,QString funcName)
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
*/

void chumstreamer_desktop::on_artistListWidget_itemDoubleClicked(QListWidgetItem* item)
{
  nextDir(item);
  /*
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
  ui->pwdLabel->setText(item->text());
  ui->pwdLabel->show();
  */
}

void chumstreamer_desktop::on_backButton_clicked()
{
  goBack();
  /*
  if(prevDirIDVec.size()==1||prevDirIDVec.size()==2){on_pushButton_clicked();return;}
  setDir(prevDirIDVec.at(1).id);
  ui->pwdLabel->setText(prevDirIDVec.at(1).name);
  prevDirIDVec.pop_front();
  */
}

//void chumstreamer_desktop::keyPressTest(QKeyEvent* oneKey)
void chumstreamer_desktop::keyPressEvent(QKeyEvent* oneKey)
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
void chumstreamer_desktop::mouseReleaseEvent(QMouseEvent* oneKey)
{
  qDebug() << "some mouse button: " <<oneKey->button();
}

/*
void chumstreamer_desktop::on_addButton_clicked()
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


void chumstreamer_desktop::on_playButton_clicked()
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
    return on_nextTrackButton_clicked();
  }
}

/*
void chumstreamer_desktop::streamSong()
{
  ui->playlistListWidget->item(getCurrentPlaylistIndex())->setForeground(Qt::red);
  QString songID=dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(getCurrentPlaylistIndex()))->info->id;
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
  connect(player,&QMediaPlayer::mediaStatusChanged,this,&chumstreamer_desktop::notifySongEnd);
}
*/


//i'll try to do this later
/*
void chumstreamer_desktop::streamSongQIO()
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
  connect(streamReply,&QNetworkReply::readyRead,this,&chumstreamer_desktop::bufferStream);
  //player->setMedia(streamRequest);
  //qDebug() << "set media as streamRequest";
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}


  //we'll just play one song at a time for now
  //connect(player,&QMediaPlayer::mediaStatusChanged,this,&chumstreamer_desktop::notifySongEnd);
}
*/

/*
void chumstreamer_desktop::bufferStream()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  //currentSong.append(oneReply->readAll());
  QMediaContent blankContent;
  player->setMedia(blankContent,oneReply);
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}
}
*/

void chumstreamer_desktop::notifySongEnd()
{
  QMediaPlayer* oneMedia=qobject_cast<QMediaPlayer* >(sender());
  if(oneMedia->mediaStatus()==QMediaPlayer::EndOfMedia)
  {
    qDebug() << "media has ended!";
    return on_nextTrackButton_clicked();
    //if(chooseNext()){return streamSong();}
  }
  else
  {
    qDebug() << "media has not ended. status: " <<oneMedia->mediaStatus();
  }
}

/*
bool chumstreamer_desktop::chooseNext()
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


void chumstreamer_desktop::on_playPauseButton_clicked()
{
}

void chumstreamer_desktop::on_pushButton_2_clicked()
{
  player->stop();
  qDebug() << "player stopped!";
  grayOutPlaylist();
}

/*
void chumstreamer_desktop::grayOutPlaylist()
{
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    ui->playlistListWidget->item(i)->setForeground(Qt::lightGray);
  }
}
*/


void chumstreamer_desktop::on_artistListWidget_currentRowChanged(int currentRow)
{
  //TODO: send the signal to this list widget's model that the current item has changed, although on android i suppose there won't be any current items. there will just be list itmes with  checkboxes like in ultrasonic. so maybs some of the playlistAddFromChumListItem stuff should be ported to desktop idk
  if(player->state()!=QMediaPlayer::StoppedState){return;}
  qDebug() << "on_artistListWidget_currentRowChanged: state: "<<player->state();
  //dumb hack to prevent crashes that occur when an item is double clicked, and the list is cleared, and as a result this method returns -1
  if(currentRow==-1){return;}
  if(dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->isSong==false){return;}
  ui->songTitleLabel->setText(ui->artistListWidget->currentItem()->text());
  getSongInfo(dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->id);
}

/*
void chumstreamer_desktop::getSongInfo(QString songID)
{
  QUrl songRequest=buildQueryString("getSong");
  QUrlQuery currentQuery(songRequest.query());
  currentQuery.addQueryItem("id",songID);
  songRequest.setQuery(currentQuery);
  //qDebug()<<"song query: "<<songRequest.toString();
  QNetworkRequest getSongInfoRequest(songRequest);
  QNetworkReply* songInfoReply=manager.get(getSongInfoRequest);
  connect(songInfoReply,&QNetworkReply::finished,this,&chumstreamer_desktop::setSongInfo);
}
*/

void chumstreamer_desktop::setSongInfo()
{
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
}

void chumstreamer_desktop::songInfoDisplay(bool show)
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


void chumstreamer_desktop::on_artistListWidget_Clicked()
{
  qDebug() << "chumstreamer_desktop.cpp: clickable list was clicked!!";
}


/*
//optionalChum is only going to be passed to this function when it's being called by playlistAddRecursive()
void chumstreamer_desktop::addToPlaylistFromSlot(bool prepend,ChumListItem* optionalChum)
{
  qDebug() << "one";
  //but only if a certain item is selected
  //we could use qobject_cast<clickableList *> here like we did with network replies, which could replace `ui->artistListWidget` /shrug
  if(optionalChum==NULL)
  {
    qDebug()<<"one point five";
    if(dynamic_cast<clickableList *>(ui->artistListWidget)->hasCurrentItem())
    {
      playlistAddFromChumListItem(dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem()),prepend);
      qDebug() << "two";
    } //there's no else for this cause we have nothing to do if no items are highlighted and the user presses A or E
  }
  else
  {
    playlistAddFromChumListItem(optionalChum,prepend);
  }
}
*/

/*
void chumstreamer_desktop::playlistAddFromChumListItem(ChumListItem* oneChum,bool prepend)
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
        ui->playlistListWidget->insertItem(getCurrentPlaylistIndex()+offset,newChum);
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


/*
void chumstreamer_desktop::firstRecursiveRequest(QString id)
{
  QUrl firstUrl(buildQueryString("getMusicDirectory"));
  QUrlQuery firstUrlQuery(firstUrl);
  firstUrlQuery.addQueryItem("id",id);
  firstUrl.setQuery(firstUrlQuery);
  qDebug()<<"firstRecursiveRequest: "<<firstUrl.toString();
  QNetworkRequest firstCallRequest(firstUrl);
  QNetworkReply* firstCallReply=manager.get(firstCallRequest);
  connect(firstCallReply,&QNetworkReply::finished,this,&chumstreamer_desktop::playlistAddRecursive);
}
*/

/*
void chumstreamer_desktop::playlistAddRecursive()
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
*/

/*
void chumstreamer_desktop::playlistAddFromNode(QDomNode oneDir)
{
  //at first, this code only ran if the node was a song, but then i realized that addToPlaylistFromSlot can just handle either case
  ChumListItem* tempSong=new ChumListItem(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false");
  addToPlaylistFromSlot(latestPrepend,tempSong);
}
*/

void chumstreamer_desktop::on_artistListWidget_playlistAppend()
{
  latestPrepend=false;
  chumstreamer_core::addToPlaylistFromSlot(false);
}

void chumstreamer_desktop::on_artistListWidget_playlistPrepend()
{
  latestPrepend=true;
  chumstreamer_core::addToPlaylistFromSlot(true);
}

void chumstreamer_desktop::on_artistListWidget_ReturnKey()
{
  return on_artistListWidget_itemDoubleClicked(ui->artistListWidget->currentItem());
}

void chumstreamer_desktop::on_artistListWidget_Back()
{
  return on_backButton_clicked();
}

void chumstreamer_desktop::on_volumeSlider_valueChanged(int value)
{
  player->setVolume(value);
}


void chumstreamer_desktop::on_nextTrackButton_clicked()
{
  if(chooseNext()){return streamSong();}
  else
  {
    qDebug()<<"on_nextTrackButton_clicked(): chooseNext() returned false";
    stop();
    //return on_pushButton_2_clicked();
  }
}

void chumstreamer_desktop::on_artistListWidget_Clear()
{
  //clear the model instead of the ui element
  playlistModel->clear();
  //ui->playlistListWidget->clear();
}

/*
int chumstreamer_desktop::getCurrentPlaylistIndex()
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

void chumstreamer_desktop::on_repeatToggleButton_clicked()
{
  toggleRepeating();
  //the base class calls this
  //writeSave();
}

void chumstreamer_desktop::toggleRepeating()
{
  chumstreamer_core::toggleRepeating();
  setRepeatText();
}

void chumstreamer_desktop::setRepeatText()
{
  if(repeating())
  {
    ui->repeatToggleButton->setText("repeat all");
  }
  else
  {
    ui->repeatToggleButton->setText("no repeat");
  }
}

//if we can't predict what the repeat button is currently set to
void chumstreamer_desktop::toggleRepeating(bool staticBool)
{
  chumstreamer_core::toggleRepeating(staticBool);
  setRepeatText();
}

void chumstreamer_desktop::on_volumeSlider_sliderReleased()
{
  writeSave();
}

void chumstreamer_desktop::on_randomToggleButton_clicked()
{
  toggleRandom();
  //the parent calls this
  //writeSave();
}
/*
bool chumstreamer_desktop::random()
{
  return ui->randomToggleButton->text()=="random on";
}
*/
void chumstreamer_desktop::toggleRandom()
{
  chumstreamer_core::toggleRandom();
  setRandomText();
}

void chumstreamer_desktop::setRandomText()
{
  if(random())
  {
    ui->randomToggleButton->setText("random on");
  }
  else
  {
    ui->randomToggleButton->setText("random off");
  }
}
//if we can't predict what the repeat button is currently set to
void chumstreamer_desktop::toggleRandom(bool staticBool)
{
  chumstreamer_core::toggleRandom(staticBool);
  setRandomText();
}

void chumstreamer_desktop::on_clearButton_clicked()
{
  on_artistListWidget_Clear();
}

/*
bool chumstreamer_desktop::hasRed()
{
  for(int i=0;i<ui->playlistListWidget->count();i++)
  {
    if(ui->playlistListWidget->item(i)->foreground()==Qt::red){return true;}
  }
  return false;
}
*/

void chumstreamer_desktop::imageFromPixmap(QPixmap oneImage)
{
  ui->imageLabel->setPixmap(oneImage);
  ui->imageLabel->show();
}

void chumstreamer_desktop::dirModelMirror(ChumListItem* newItem/*,bool checked*/,int index)
{
  /*
  if(checked)
  {
    newItem->setCheckState(Qt::Checked);
  }
  else{
    newItem->setCheckState(Qt::Unchecked);
  }
  */
  /*
  qDebug() << "chumstreamer_desktop::dirModelMirror(): checkstate: "<<newItem->checkState();
  qDebug() << "chumstreamer_desktop::dirModelMirror(): flags: "<<newItem->flags();
  qDebug() << "chumstreamer_desktop::dirModelMirror(): address: "<<newItem;
  */
  if(index==-1)
  {
    ui->musicFolderListWidget->addItem(newItem);
    return;
  }
  else
  {
    ui->musicFolderListWidget->insertItem(index,newItem);
    return;
  }
}
void chumstreamer_desktop::artistModelMirror(ChumListItem* newItem,int index)
{
  /*
  qDebug() << "artistModelMirror(): checkstate: "<<newItem->checkState();
  qDebug() << "artistModelMirror(): flags: "<<newItem->flags();
  */
  if(index==-1)
  {
    ui->artistListWidget->addItem(newItem);
    return;
  }
  else
  {
    ui->artistListWidget->insertItem(index,newItem);
    return;
  }
}
void chumstreamer_desktop::playlistModelMirror(ChumListItem* newItem,int index)
{
  if(index==-1)
  {
    ui->playlistListWidget->addItem(newItem);
    return;
  }
  else
  {
    ui->playlistListWidget->insertItem(index,newItem);
    return;
  }
}

/*
void chumstreamer_desktop::clearListWidget()
{
  qDebug() << "casting...";
  qobject_cast<QListWidget* >(sender())->clear();
  qDebug() << "clearing...";
}
*/
