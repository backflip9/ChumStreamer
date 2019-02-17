#ifndef CHUMSTREAMER_H
#define CHUMSTREAMER_H

#include <QMainWindow>
#include <QMainWindow>
#include<QtNetwork/QNetworkAccessManager>
#include<QVector>
#include<QMediaPlayer>
#include "musicfolderinfo.h"

//forward declarations
class QNetworkReply;
class QFile;
class QStringList;
class QListWidgetItem;
class QByteArray;
class QMediaPlayer;

namespace Ui {
class chumstreamer;
}

class chumstreamer : public QMainWindow
{
    Q_OBJECT

public:
    explicit chumstreamer(QWidget *parent = nullptr);
    ~chumstreamer();
    QUrl& Server(){return server;}
    QString& Username(){return username;}
    QString& Password(){return password;}
    //i used this for testing when i forgot how pointers worked
    //QString getServer(){return server;}
    //void setServer(QString serverTxt){server=serverTxt;}
    void setDir(const QString& dir);

private slots:
    //buttons:
    void on_pushButton_clicked();
    void bufferStream();
    void notifySongEnd();
    //void playFile();
    void on_configureButton_clicked();
    //void on_nextButton_clicked();
    //helpers:
    void addArtists();
    void getMusicFolders();
    void displayDir();
    void displayImage();

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

private:
    Ui::chumstreamer *ui;
    QNetworkAccessManager manager;
    //QVector<QNetworkReply* > replyVec;
    //QNetworkReply* reply;
    QString cacheFilePath;
    //QString server="";
    QUrl server;
    QString username="";
    QString password="";
    QString filePath;
    QUrl buildQueryString(QString path="");
    void setMusicFolders();
    bool handleNetworkError(QNetworkReply* reply,QString funcName="");
    void songInfoDisplay(bool hide);
    QStringList artistList;
    QVector<musicFolderInfo> musicFolderVec;
    QVector<musicFolderInfo> prevDirIDVec;
    void keyPressEvent(QKeyEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void addToPlaylistFromSlot(bool prepend);
    QMediaPlayer* player = new QMediaPlayer;
    QByteArray currentSong;
    //QMediaPlayer *player;
    const int DEFAULT_VOLUME=20;
    int currentPlaylistIndex;
    void streamSong();
    bool chooseNext();
    void streamSongQIO();
};

#endif // CHUMSTREAMER_H
