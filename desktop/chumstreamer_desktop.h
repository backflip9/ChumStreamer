#ifndef chumstreamer_desktop_H
#define chumstreamer_desktop_H

#include <QMainWindow>
#include <QMainWindow>
#include<QtNetwork/QNetworkAccessManager>
#include<QVector>
#include<QMediaPlayer>
#include "../core/chumstreamer_core.h"

//forward declarations
class QNetworkReply;
class QFile;
class QStringList;
class QListWidgetItem;
class QByteArray;
class QMediaPlayer;
class ChumListItem;
class QDomNode;

namespace Ui {
class chumstreamer_desktop;
}

class chumstreamer_desktop : public chumstreamer_core
{
    Q_OBJECT

public:
    explicit chumstreamer_desktop(QWidget *parent = nullptr);
    ~chumstreamer_desktop();
    /*
    QUrl& Server(){return server;}
    QString& Username(){return username;}
    QString& Password(){return password;}
    */
    //void setDir(const QString& dir);
    //public for authdialog to use
    //void writeSave();

private slots:
    //buttons:
    void on_pushButton_clicked();
    //void bufferStream();
    void notifySongEnd();
    //void playFile();
    void on_configureButton_clicked();
    //void on_nextButton_clicked();
    //helpers:
    //void addArtists();
    //void getMusicFolders();
    //void displayDir();//this is void in the parent class but i don't think we need the keyword in this child class?
    //void displayImage();//this class uses imageFromPixmap now

    void on_artistListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_backButton_clicked();

    //void on_addButton_clicked();

    void on_playButton_clicked();

    void on_playPauseButton_clicked();

    void on_pushButton_2_clicked();
    void setSongInfo();


    void on_artistListWidget_currentRowChanged(int currentRow);

    void on_artistListWidget_Clicked();

    //void on_artistListWidget_ClickedWithEvent(QMouseEvent *);


    void on_artistListWidget_playlistAppend();

    void on_artistListWidget_playlistPrepend();

    void on_artistListWidget_ReturnKey();

    void on_artistListWidget_Back();

    void on_volumeSlider_valueChanged(int value);

    void on_repeatToggleButton_clicked();

    void on_nextTrackButton_clicked();

    //void playlistAddRecursive();

    void on_artistListWidget_Clear();

    void on_volumeSlider_sliderReleased();

    void on_randomToggleButton_clicked();

    void on_clearButton_clicked();

    void dirModelMirror(ChumListItem* newItem,/*bool checked,*/int index=-1);
    void artistModelMirror(ChumListItem* newItem,int index=-1);
    void playlistModelMirror(ChumListItem* newItem,int index=-1);

private:
    Ui::chumstreamer_desktop *ui;
    /*
    QNetworkAccessManager manager;
    //QVector<QNetworkReply* > replyVec;
    //QNetworkReply* reply;
    QString cacheFilePath;
    QUrl server;
    QString username="";
    QString password="";
    */
    QString filePath;
    //void setMusicFolders();
    //bool handleNetworkError(QNetworkReply* reply,QString funcName="");
    void songInfoDisplay(bool hide);
    QStringList artistList;
    QVector<musicFolderInfo> musicFolderVec;
    //void playlistAddFromChumListItem(ChumListItem* oneChum,bool prepend);
    QVector<musicFolderInfo> prevDirIDVec;
    void keyPressEvent(QKeyEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    //void addToPlaylistFromSlot(bool prepend,ChumListItem optionalChum=NULL);
    void addToPlaylistFromSlot(bool prepend,ChumListItem* optionalChum=NULL);
    QMediaPlayer* player = new QMediaPlayer;
    QByteArray currentSong;
    const int DEFAULT_VOLUME=20;
    int getCurrentPlaylistIndex();
    //void streamSong();
    //bool chooseNext();
    //stores whether the user last pressed E or A, since we're going to be adding playlist items in slot functions that i don't know how to pass args to
    bool latestPrepend=false;
    //will work on this later
    //void streamSongQIO();
    void firstRecursiveRequest(QString id);
    //void playlistAddFromNode(QDomNode oneDir);
    void setImage(QString songID);
    //void getSongInfo(QString songID);
    //void grayOutPlaylist();
    QString currentAlbumArt="0";
    //bool repeating();
    void toggleRepeating();//virtual
    void toggleRepeating(bool);//virtual
    //bool random();
    void toggleRandom();//virtual
    void toggleRandom(bool);//virtual
    void imageFromPixmap(QPixmap oneImage);
    void setRepeatText();
    void setRandomText();
    //void clearListWidget();//{qobject_cast<QListWidget* >(sender())->clear();}
    //QJsonDocument readSave();
    //bool applyFromSave();
    /*
    bool hasRed();
    QString checkedFolders;
    */
};

#endif // chumstreamer_desktop_H
