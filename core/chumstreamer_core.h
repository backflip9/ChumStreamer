#ifndef chumstreamer_core_H
#define chumstreamer_core_H

#include <QMainWindow>
#include<QtNetwork/QNetworkAccessManager>
#include<QVector>
#include<QMediaPlayer>
#include<QDir>
#include "musicfolderinfo.h"
#include "modellist.h"

//forward declarations
class QNetworkReply;
class QFile;
class QStringList;
class QListWidgetItem;
class QByteArray;
class QMediaPlayer;
class ChumListItem;
class QDomNode;
//class QList;

/*
namespace Ui {
class chumstreamer_core;
}
*/

class chumstreamer_core : public QMainWindow
{
    //Q_OBJECT

public:
    explicit chumstreamer_core(QWidget *parent = nullptr);
    ~chumstreamer_core();
    QUrl& Server(){return server;}
    QString& Username(){return username;}
    QString& Password(){return password;}
    void setDir(const QString& dir);
    //public for authdialog to use
    void writeSave();

    /*
signals:
    void artistListUpdate(QList<ChumListItem* >);
    void newArtist(ChumListItem*,bool prepend=false);
    void newFolder(ChumListItem*)
    */
public slots:
    virtual void dirModelMirror(ChumListItem* newItem,/*bool checked,*/int index=-1)=0;//pure virtual
    virtual void artistModelMirror(ChumListItem* newItem,int index=-1)=0;//pure virtual
    virtual void playlistModelMirror(ChumListItem* newItem,int index=-1)=0;//pure virtual

private slots:
    //buttons:
    //void on_pushButton_clicked();
    void bufferStream();
    void notifySongEnd();
    //void playFile();
    //void on_configureButton_clicked();
    //void on_nextButton_clicked();
    //helpers:
    void addArtists();
    void getMusicFolders();
    virtual void displayDir();//i think we'll need to do some weird voodoo magic on android to get the list of folders to show up
    void displayImage();
    //virtual void imageFromPixmap(QPixmap input){return;}
    virtual void imageFromPixmap(QPixmap input)=0;//pure virtual

    //void on_artistListWidget_itemDoubleClicked(QListWidgetItem *item);

    //void on_backButton_clicked();

    //void on_addButton_clicked();

    void on_playButton_clicked();

    //void on_playPauseButton_clicked();

    //void on_pushButton_2_clicked();
    virtual void setSongInfo()=0;//pure virtual

    //void on_artistListWidget_currentRowChanged(int currentRow);

    //void on_artistListWidget_Clicked();

    //void on_artistListWidget_ClickedWithEvent(QMouseEvent *);


    void on_artistListWidget_playlistAppend();

    void on_artistListWidget_playlistPrepend();

    /*
    void on_artistListWidget_ReturnKey();

    void on_artistListWidget_Back();

    void on_volumeSlider_valueChanged(int value);

    void on_repeatToggleButton_clicked();

    void on_nextTrackButton_clicked();
    */

    void playlistAddRecursive();

    /*
    void on_artistListWidget_Clear();

    void on_volumeSlider_sliderReleased();

    */
    void on_randomToggleButton_clicked();

    void on_clearButton_clicked();


protected:
    //Ui::chumstreamer_core *ui;
    QNetworkAccessManager manager;
    /*
    QList<ChumListItem *> artistModel;
    QList<ChumListItem *> playlistModel;
    */
    modelList* directoryModel=new modelList;
    modelList* artistModel=new modelList;
    modelList* playlistModel=new modelList;
    //virtual void show(){QMainWindow::show();}
    //QVector<QNetworkReply* > replyVec;
    //QNetworkReply* reply;
    QString cacheFilePath=QDir::homePath() + "/chumstreamer_core.json";
    //QString server="";
    QUrl server;
    QString username="";
    QString password="";
    //QString filePath;
    QUrl buildQueryString(QString path="");
    void setMusicFolders();
    bool handleNetworkError(QNetworkReply* reply,QString funcName="");
    virtual void songInfoDisplay(bool hide)=0;//pure virtual
    void playNext();
    QStringList artistList;
    QVector<musicFolderInfo> musicFolderVec;
    void playlistAddFromChumListItem(ChumListItem* oneChum,bool prepend);
    QVector<musicFolderInfo> prevDirIDVec;
    virtual void keyPressEvent(QKeyEvent*)=0;//pure virtual
    virtual void mouseReleaseEvent(QMouseEvent*)=0;//pure virtual
    void addToPlaylistFromSlot(bool prepend,ChumListItem* optionalChum=NULL);
    QMediaPlayer* player = new QMediaPlayer;
    QByteArray currentSong;
    const int DEFAULT_VOLUME=20;
    int getCurrentPlaylistIndex();
    void streamSong();
    bool chooseNext();
    //stores whether the user last pressed E or A, since we're going to be adding playlist items in slot functions that i don't know how to pass args to
    bool latestPrepend=false;//for asynchronous network requests that add things to the playlist
    //will work on this later
    //void streamSongQIO();
    void firstRecursiveRequest(QString id);
    void playlistAddFromNode(QDomNode oneDir);
    void setImage(QString songID);
    void getSongInfo(QString songID);
    void grayOutPlaylist();
    QString currentAlbumArt="0";
    bool repeatingEnabled;
    bool repeating();
    virtual void toggleRepeating();
    virtual void toggleRepeating(bool);
    bool randomEnabled=false;
    virtual void toggleRandom();
    virtual void toggleRandom(bool);
    //QJsonDocument readSave();
    bool applyFromSave();
    //virtual bool applyFromSave();//desktop has a volume parameter but mobile won't
    bool hasRed();
    QString checkedFolders;
    //void displayRoot(const QString& bitmask);
    void displayRoot();
    void clearPlaylist();
    bool random();
    void goBack();
    void stop();
    virtual void nextDir(QListWidgetItem *item);//to change ui things after the fact. the parent function will be called in the child implementation
};

#endif // chumstreamer_core_H
