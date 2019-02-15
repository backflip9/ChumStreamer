#include<QtNetwork/QNetworkAccessManager>
#include<QtNetwork/QNetworkRequest>
#include<QtNetwork/QNetworkReply>
#include<QFile>
#include<QBuffer>
//#include<QListWidgetItem>
#include<QDomDocument>
//#include<vector>
#include<QVector>
#include<QStringListModel>
#include<QMouseEvent>
#include<QMediaPlayer>
#include<QDir>
#include<QString>
#include<QUrl>
#include<QUrlQuery>
#include<QDebug>
#include "ui_ChumStreamer.h"
#include "ChumStreamer.h"
#include "authdialog.h"
#include "chumlistitem.h"
#define artistTabIndex 0

ChumStreamer::ChumStreamer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChumStreamer)
{
    ui->setupUi(this);
    filePath = QDir::homePath() + "/Documents/tmpDownload.xml";
    cacheFilePath=QDir::homePath() + "/subping.dat";
    //ui->
    ui->imageLabel->hide();
    ui->pwdLabel->hide();
    ui->nextButton->hide();
    ui->volumeSlider->setTickInterval(2);
    ui->volumeSlider->setRange(0,100);
    ui->volumeSlider->setValue(DEFAULT_VOLUME);

    //hide individual song info(should be blank if we're just starting up)
    songInfoDisplay(false);
    player->setVolume(ui->volumeSlider->value());
    qDebug() << "something written in vim\n";
    //copypaste from authDialog class
    //populate the members with auth info from the dat file, if it exists
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
}

QUrl ChumStreamer::buildQueryString(QString page)
{
  QUrl result(server);
  qDebug()<< "buildQueryString one: "<<result.toString();
  if(page!="")
  {
    result.setPath("/rest/" + page);
  }
  QUrlQuery myQuery;
  myQuery.addQueryItem("u",Username());
  myQuery.addQueryItem("p",Password());
  myQuery.addQueryItem("c","ChumStreamer");
  myQuery.addQueryItem("v","1.15.0");
  result.setQuery(myQuery.query());
  qDebug()<< "buildQueryString: "<<result.toString();
  return result;
}

ChumStreamer::~ChumStreamer()
{
    delete ui;
}


void ChumStreamer::addArtists()
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
    /*
    reply->deletelater();
    reply=nullptr;
    */
    oneReply->deleteLater();
    oneReply=nullptr;
    qDebug("addartists success");
    /*
    ui->songTitleLabel->hide();
    ui->albumNameLabel->hide();
    ui->albumArtistLabel->hide();
    */
    songInfoDisplay(false);
    //remove all the existing items
    //ui->artistListWidget->clear();
    //qDebug() << "cleared artistListWidget in addArtists()";
    //i guess we don't need qdomelements in the first place??
    QDomElement oneElement=myDoc.documentElement();
    //the progression should be <subsonic-response> -> <artists> -> many <index> tags
    QDomNode artistsTag= oneElement.firstChild();
    QDomNodeList oneNodeList=artistsTag.childNodes();
    //this is a member var now
    //QStringList artistList;
    for(int j=0;j<oneNodeList.length();j++)
    {
      QDomNode twoNode=oneNodeList.at(j);
      /*
      qDebug() << "twoNode nodename: " << twoNode.nodeName();
      qDebug() << "twoNode attrname: " << twoNode.attributes().namedItem("name").nodeValue();
      */
      QDomNodeList twoNodeList=twoNode.childNodes();
      for(int k=0;k<twoNodeList.length();k++)
      {
        QDomNode artistNode =twoNodeList.at(k);
        artistList << artistNode.attributes().namedItem("name").nodeValue();


        //we're hardcoding the isSong attribute to be false cause getIndexes returns weird tags that are unique, and don't have the isDir attribute
        ChumListItem *newArtist= new ChumListItem(musicFolderInfo(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
        ui->artistListWidget->addItem(newArtist);


        //saving cause it's a silly way to do things
        //this also works but is ugly as fuck
        //ui->musicTabWidget->widget(artistTabIndex)->findChildren<QListWidget* >(artistListWidgetName).first()->item(ui->musicTabWidget->widget(artistTabIndex)->findChildren<QListWidget* >(artistListWidgetName).first()->count()-1)->setData(Qt::ToolTipRole,artistNode.attributes().namedItem("id").nodeValue());
      }
    }
    ui->artistListWidget->setFocus(Qt::TabFocusReason);
}

void ChumStreamer::getMusicFolders()
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
  /*
  reply->deleteLater();
  reply=nullptr;
  */
  oneReply->deleteLater();
  oneReply=nullptr;
  qDebug("success");
  //was gonna use this from stackoverflow but then i found the qlistwidget.clear() method
  //qDeleteAll(ui->musicFolderListWidget->sele
  ui->musicFolderListWidget->clear();
  //myFile.close();
  //i guess we don't need qdomelements in the first place??
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <artists> -> many <index> tags
  QDomNode musicFoldersTag=oneElement.firstChild();
  //QDomNode artistsTag= myDoc.namedItem("artists");
  //QDomNode twoNode;
  QDomNodeList oneNodeList=musicFoldersTag.childNodes();
  /*
  QStringList musicFolderList;
  //paranoia
  while(!musicFolderList.isEmpty())
  {
    musicFolderList.pop_back();
  }
  */
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode oneMusicFolder=oneNodeList.at(j);
    //musicFolderList << oneMusicFolder.attributes().namedItem("name").nodeValue();
    musicFolderVec.push_back(musicFolderInfo(oneMusicFolder.attributes().namedItem("name").nodeValue(),oneMusicFolder.attributes().namedItem("id").nodeValue()));
    QListWidgetItem *oneFolderCheckbox = new QListWidgetItem(oneMusicFolder.attributes().namedItem("name").nodeValue(),ui->musicFolderListWidget);
    //we're using the constructor that we made for the subclass ChumListItem that has an artistID which actually is just an ID that refers to the music folder it represents. we set that ID as the 3rd argument to the ctor here.
    //ChumListItem *oneFolderCheckbox = new ChumListItem(oneMusicFolder.attributes().namedItem("name").nodeValue(),ui->musicFolderListWidget,oneMusicFolder.attributes().namedItem("id").nodeValue());
    oneFolderCheckbox->setFlags(oneFolderCheckbox->flags() | Qt::ItemIsUserCheckable);
    oneFolderCheckbox->setCheckState(Qt::Checked);
    /*
    qDebug() << "twoNode nodename: " << oneMusicFolder.nodeName();
    qDebug() << "twoNode attrname: " << oneMusicFolder.attributes().namedItem("name").nodeValue();
    */
  }
  /*
  QStringListModel *oneQSLM=new QStringListModel(this);
  oneQSLM->setStringList(musicFolderList);
  ui->musicFolderComboBox->setModel(oneQSLM);
  ui->musicFolderComboBox->show();
  */
}

void ChumStreamer::setMusicFolders()
{
  //initialize a network request and call getMusicFolders to change the values in the combobox based on the auth details entered in the authDialog
  QNetworkRequest getMusicFoldersRequest(buildQueryString("getMusicFolders"));
  QNetworkReply* musicFoldersReply=manager.get(getMusicFoldersRequest);
  connect(musicFoldersReply,&QNetworkReply::finished,this,&ChumStreamer::getMusicFolders);
}

void ChumStreamer::on_configureButton_clicked()
{
    authDialog authD;
    authD.setParentClass(this);
    authD.setAuthInfo();
    authD.setModal(true);
    authD.exec();
    setMusicFolders();
    qDebug() << server;
    qDebug() << username;
    qDebug() << password;
}

void ChumStreamer::on_pushButton_clicked()
{
    qDebug() << "button clicked";

    //hide things that aren't relevant anymore
    ui->artistListWidget->clear();
    /*
    ui->songTitleLabel->hide();
    ui->albumNameLabel->hide();
    ui->albumArtistLabel->hide();
    */
    songInfoDisplay(false);
    ui->pwdLabel->hide();
    ui->imageLabel->hide();
    prevDirIDVec.clear();
    prevDirIDVec.push_front(musicFolderInfo("","-1"));

    qDebug() << "cleared artistListWidget in pushbutton slot";

    const QUrl originalUrl=buildQueryString("getIndexes");
    //qDebug() << "requesting at "<<myUrl.path() << myUrl.query();
    //add iterator here to loop through the folder IDs that were checked
    //QList checkboxList=ui->musicFolderListWidget->count()
    for(int counter=0;counter<ui->musicFolderListWidget->count();counter++)
    {
      if(ui->musicFolderListWidget->item(counter)->checkState()==Qt::Checked)
      {
        qDebug() << musicFolderVec[counter].name << " was checked!!";
        //call setMusicFolders to add them to the qlistwidget. we aren't gonna pass a qstringlist by reference thru all those stack frames even if it might be easier, we're just gonna call setMusicFolders multiple times until the artist list is populated with all the relevant items
        QUrl currentUrl=originalUrl;
        QUrlQuery currentQuery(currentUrl.query());
        //this isn't working atm
        //currentQuery.addQueryItem(ui->musicFolderListWidget->item(counter)->getArtistID());
        currentQuery.addQueryItem("musicFolderId",musicFolderVec[counter].id);
        currentUrl.setQuery(currentQuery.query());
        qDebug("query to be sent to addArtists():");
        //qDebug(currentUrl.query());
        qDebug()<<currentUrl.query();
        QNetworkRequest getArtistsRequest(currentUrl);
        QNetworkReply* getIndexesReply=manager.get(getArtistsRequest);
        connect(getIndexesReply,&QNetworkReply::finished,this,&ChumStreamer::addArtists);
      }
    }
}

void ChumStreamer::setDir(const QString& dir)
{
  QUrl currentUrl = buildQueryString("getMusicDirectory");
  QUrlQuery currentQuery(currentUrl.query());
  currentQuery.addQueryItem("id",dir);
  currentUrl.setQuery(currentQuery.query());
  qDebug() << "requesting at "<<currentUrl.path() << currentUrl.query();
  QNetworkRequest getDirRequest(currentUrl);
  QNetworkReply* getMusicDirReply=manager.get(getDirRequest);
  connect(getMusicDirReply,&QNetworkReply::finished,this,&ChumStreamer::displayDir);
}

void ChumStreamer::displayDir()
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
  ui->imageLabel->hide();
  ui->artistListWidget->clear();
  //i guess we don't need qdomelements in the first place??
  QDomElement oneElement=myDoc.documentElement();
  //the progression should be <subsonic-response> -> <directory> -> many <child> tags
  QDomNode directoryTag=oneElement.firstChild();
  QDomNodeList oneNodeList=directoryTag.childNodes();
  bool grabAlbumArt=false;
  for(int j=0;j<oneNodeList.length();j++)
  {
    QDomNode oneDir=oneNodeList.at(j);
    //ChumListItem *newArtist= new ChumListItem(musicFolderInfo(artistNode.attributes().namedItem("name").nodeValue(),artistNode.attributes().namedItem("id").nodeValue(),false));
    ChumListItem *chumOneFolderCheckbox = new ChumListItem(musicFolderInfo(oneDir.attributes().namedItem("title").nodeValue(),oneDir.attributes().namedItem("id").nodeValue(),oneDir.attributes().namedItem("isDir").nodeValue()=="false"));
    qDebug() << "created ChumListItem";
    ui->artistListWidget->addItem(chumOneFolderCheckbox);

    //QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());


    //qDebug() << "latest item ID? "<< dynamic_cast<ChumListItem* >(ui->artistListWidget->item(ui->artistListWidget->count()-1))->info->folderId;
    //ChumListItem *oneFolderCheckbox = new ChumListItem(oneMusicFolder.attributes().namedItem("name").nodeValue(),ui->musicFolderListWidget,oneMusicFolder.attributes().namedItem("id").nodeValue());
    //qDebug() << "setting data: " << oneDir.attributes().namedItem("id").nodeValue();
    //add the regular QListWidgetItem to the artist/song list. this involves setting the song ID as the tooltip value
    /*
    ui->artistListWidget->addItem(oneDir.attributes().namedItem("title").nodeValue());
    ui->artistListWidget->item(ui->artistListWidget->count()-1)->setData(Qt::ToolTipRole,oneDir.attributes().namedItem("id").nodeValue());
    */
    if(!grabAlbumArt && !oneDir.attributes().namedItem("coverArt").isNull() && oneDir.attributes().namedItem("isDir").nodeValue()=="false")
    {
      QUrl imageUrl=buildQueryString("getCoverArt");
      QUrlQuery imageQuery(imageUrl.query());
      imageQuery.addQueryItem("id",oneDir.attributes().namedItem("coverArt").nodeValue());
      imageQuery.addQueryItem("size","190");
      imageUrl.setQuery(imageQuery.query());
      QNetworkRequest imageRequest(imageUrl);
      QNetworkReply* imageReply=manager.get(imageRequest);
      connect(imageReply,&QNetworkReply::finished,this,&ChumStreamer::displayImage);
      grabAlbumArt=true;
    }
  }
}

void ChumStreamer::displayImage()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  if(handleNetworkError(oneReply,"displayImage")){return;}
  qDebug() << "displayImage with no network error";
  QPixmap oneImage;
  oneImage.loadFromData(oneReply->readAll());
  oneReply->deleteLater();
  oneReply=nullptr;
  ui->imageLabel->setPixmap(oneImage);
  ui->imageLabel->show();
}


/*
void ChumStreamer::on_nextButton_clicked()
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

bool ChumStreamer::handleNetworkError(QNetworkReply* reply,QString funcName)
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

void ChumStreamer::on_artistListWidget_itemDoubleClicked(QListWidgetItem *item)
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
  ui->pwdLabel->setText(item->text());
  ui->pwdLabel->show();
}

void ChumStreamer::on_backButton_clicked()
{
  if(prevDirIDVec.size()==1||prevDirIDVec.size()==2){on_pushButton_clicked();return;}
  setDir(prevDirIDVec.at(1).id);
  ui->pwdLabel->setText(prevDirIDVec.at(1).name);
  prevDirIDVec.pop_front();
}

//void ChumStreamer::keyPressTest(QKeyEvent* oneKey)
void ChumStreamer::keyPressEvent(QKeyEvent* oneKey)
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
void ChumStreamer::mouseReleaseEvent(QMouseEvent* oneKey)
{
  qDebug() << "some mouse button: " <<oneKey->button();
}

/*
void ChumStreamer::on_addButton_clicked()
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

void ChumStreamer::on_playButton_clicked()
{
  currentPlaylistIndex=0;
  return streamSong();
  //testing:
  //return streamSongQIO();
}

void ChumStreamer::streamSong()
{
  ui->playlistListWidget->item(currentPlaylistIndex)->setForeground(Qt::red);
  QUrl streamUrl=buildQueryString("stream");
  QUrlQuery currentQuery(streamUrl.query());
  currentQuery.addQueryItem("id",dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(currentPlaylistIndex))->info->id);
  streamUrl.setQuery(currentQuery.query());
  QNetworkRequest streamRequest(streamUrl);
  player->setMedia(streamRequest);
  qDebug() << "set media as streamRequest";
  player->play();
  connect(player,&QMediaPlayer::mediaStatusChanged,this,&ChumStreamer::notifySongEnd);
}


void ChumStreamer::streamSongQIO()
{
  ui->playlistListWidget->item(currentPlaylistIndex)->setForeground(Qt::red);
  QUrl streamUrl=buildQueryString("stream");
  QUrlQuery currentQuery(streamUrl.query());
  currentQuery.addQueryItem("id",dynamic_cast<ChumListItem *>(ui->playlistListWidget->item(currentPlaylistIndex))->info->id);
  streamUrl.setQuery(currentQuery.query());
  QNetworkRequest streamRequest(streamUrl);
  QNetworkReply* streamReply=manager.get(streamRequest);
  connect(streamReply,&QNetworkReply::readyRead,this,&ChumStreamer::bufferStream);
  //player->setMedia(streamRequest);
  //qDebug() << "set media as streamRequest";
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}


  //we'll just play one song at a time for now
  //connect(player,&QMediaPlayer::mediaStatusChanged,this,&ChumStreamer::notifySongEnd);
}

void ChumStreamer::bufferStream()
{
  QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  //currentSong.append(oneReply->readAll());
  QMediaContent blankContent;
  player->setMedia(blankContent,oneReply);
  if(player->state()!=QMediaPlayer::PlayingState){player->play();}
}
void ChumStreamer::playFile()
{
  return;
  //QIODevice something;
  //QNetworkReply* oneReply=qobject_cast<QNetworkReply* >(sender());
  /*
  QFile cachedSong(QDir::homePath() + "/cachedSong.mp3");
  if(!cachedSong.open(QIODevice::WriteOnly)){
    qDebug() << "path: " << QDir::homePath() + "/cachedSong.mp3";
    qDebug() << "playFile failure";
    return;
  }
  cachedSong.write(oneReply->readAll());
  cachedSong.close();
  player->setMedia(cachedSong);
  */
  //player->setMedia(QUrl::fromLocalFile(QDir::homePath() + "cachedSong.mp3"));
  //player->play();
  //connect(player,&QMediaPlayer::mediaStatusChanged,this,&ChumStreamer::notifySongEnd);
}

void ChumStreamer::notifySongEnd()
{
  QMediaPlayer* oneMedia=qobject_cast<QMediaPlayer* >(sender());
  if(oneMedia->mediaStatus()==QMediaPlayer::EndOfMedia)
  {
    qDebug() << "media has ended!";
    ui->playlistListWidget->item(currentPlaylistIndex)->setForeground(Qt::white);
    if(chooseNext()){return streamSong();}
  }
  else
  {
    qDebug() << "media has not ended. status: " <<oneMedia->mediaStatus();
  }
}

bool ChumStreamer::chooseNext()
{
  if(currentPlaylistIndex==ui->playlistListWidget->count()-1)
  {
    if(ui->repeatToggleButton->text()=="repeat all")
    {
      currentPlaylistIndex=0;
      qDebug() <<"current item is last item, looping back to beginning";
      return true;
    }
    else
    {
      qDebug()<<"current item is last item, playlist has concluded";
      return false;
    }
  }
  else
  {
    currentPlaylistIndex++;
    return true;
  }
}


void ChumStreamer::on_playPauseButton_clicked()
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
  else
  {
    qDebug() << "playpause: neither!";
  }
}

void ChumStreamer::on_pushButton_2_clicked()
{

  player->stop();
  qDebug() << "player stopped!";
}


void ChumStreamer::on_artistListWidget_currentRowChanged(int currentRow)
{
  //dumb hack to prevent crashes that occur when an item is double clicked, and the list is cleared, and as a result this method returns -1
  if(currentRow==-1){return;}
  if(dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->isSong==false){return;}
  ui->songTitleLabel->setText(ui->artistListWidget->currentItem()->text());
  qDebug() << "current row changed signal!" << "row: " << currentRow;
  QUrl songRequest=buildQueryString("getSong");
  QUrlQuery currentQuery(songRequest.query());
  //currentQuery.addQueryItem("id",ui->artistListWidget->item(currentRow)->data(Qt::ToolTipRole).toString());
  currentQuery.addQueryItem("id",dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->id);
  qDebug() << "currentRowChanged: got song id from chumlistitem: "<<dynamic_cast<ChumListItem *>(ui->artistListWidget->item(currentRow))->info->id;
  songRequest.setQuery(currentQuery.query());
  QNetworkRequest getSongInfoRequest(songRequest);
  QNetworkReply* songInfoReply=manager.get(getSongInfoRequest);
  connect(songInfoReply,&QNetworkReply::finished,this,&ChumStreamer::setSongInfo);
}

void ChumStreamer::setSongInfo()
{
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

void ChumStreamer::songInfoDisplay(bool show)
{
  if(show)
  {
    ui->songTitleLabel->show();
    ui->albumNameLabel->show();
    ui->albumArtistLabel->show();
  }
  else
  {
    ui->songTitleLabel->hide();
    ui->albumNameLabel->hide();
    ui->albumArtistLabel->hide();
  }
}


void ChumStreamer::on_artistListWidget_Clicked()
{
  qDebug() << "ChumStreamer.cpp: clickable list was clicked!!";

}

/*
void ChumStreamer::on_artistListWidget_ClickedWithEvent(QMouseEvent *)
{
  qDebug() << "ChumStreamer.cpp: CLICKEDWITHEVENT";
}
*/

void ChumStreamer::addToPlaylistFromSlot(bool prepend)
{
  qDebug() << "one";
  //but only if a certain item is selected
  if(dynamic_cast<clickableList *>(ui->artistListWidget)->hasCurrentItem()) //we could use qobject_cast<clickableList *> here like we did with network replies, which could replace `ui->artistListWidget` /shrug
  {
    if(dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem())->info->isSong)
    {
      qDebug() << "two";
      //and only if that item doesn't already exist in the playlist
      if(ui->playlistListWidget->findItems(ui->artistListWidget->currentItem()->text(),Qt::MatchExactly).size()==0)
      {
        qDebug() << "three";
        //ChumListItem* newSong=new ChumListItem(musicFolderInfo(*dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem())->info));
        ChumListItem* newSong=new ChumListItem(*dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem()));
        qDebug() << "clickableList: built chumlistitem for adding to playlist";
        if(prepend)
        {
          ui->playlistListWidget->insertItem(0,newSong);
          qDebug()<<"addToPlaylistFromSlot: prepended to playlist";
        }
        else{
          ui->playlistListWidget->addItem(newSong);
          qDebug()<<"addToPlaylistFromSlot: appended "<<dynamic_cast<ChumListItem *>(ui->artistListWidget->currentItem())->info->name << "to playlist";
        }
      }
    }
  }
}

//append a song to the playlist (will add folder support later)
void ChumStreamer::on_artistListWidget_playlistAppend()
{
  qDebug() << "append slot!";
  {
    addToPlaylistFromSlot(false);
  }
}

void ChumStreamer::on_artistListWidget_playlistPrepend()
{
  bool prepend=true;
  if(ui->playlistListWidget->count()==0){prepend=false;}
  addToPlaylistFromSlot(prepend);
}

void ChumStreamer::on_artistListWidget_ReturnKey()
{
  return on_artistListWidget_itemDoubleClicked(ui->artistListWidget->currentItem());
}

void ChumStreamer::on_artistListWidget_Back()
{
  return on_backButton_clicked();
}

void ChumStreamer::on_volumeSlider_valueChanged(int value)
{
  player->setVolume(value);
}

void ChumStreamer::on_repeatToggleButton_clicked()
{
  if(ui->repeatToggleButton->text()=="no repeat")
  {
    ui->repeatToggleButton->setText("repeat all");
  }
  else if(ui->repeatToggleButton->text()=="repeat all")
  {
    ui->repeatToggleButton->setText("no repeat");
  }
}

void ChumStreamer::on_nextTrackButton_clicked()
{
  ui->playlistListWidget->item(currentPlaylistIndex)->setForeground(Qt::white);
  if(chooseNext()){return streamSong();}
  else{qDebug()<<"on_nextTrackButton_clicked(): chooseNext() returned false";}
}
