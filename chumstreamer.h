#ifndef CHUMSTREAMER_H
#define CHUMSTREAMER_H

#define APP_NAME "ChumStreamer"

#include <QMainWindow>
#include <QMainWindow>
#include <QtNetwork/QNetworkAccessManager>
#include <QVector>
#include <QMediaPlayer>
#include <memory>
#include "musicfolderinfo.h"

#define CONCAT(arg1, arg2) arg1 ## arg2

//forward declarations
class QNetworkReply;
class QFile;
class QStringList;
class QListWidgetItem;
class QByteArray;
class QMediaPlayer;
class ChumListItem;
class QDomNode;
class QShortcut;

namespace Ui {
class chumstreamer;
}

class chumstreamer : public QMainWindow
{
    Q_OBJECT

public:
    explicit chumstreamer(QWidget *parent = nullptr);
    ~chumstreamer();
    void setDir(const QString& dir);
    //public for authdialog to use
    void writeSave();
    QUrl server;

private slots:
    //buttons:
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

    void playlistAddRecursive();

    void on_artistListWidget_Clear();

    void on_volumeSlider_sliderReleased();

    void on_randomToggleButton_clicked();

    void on_clearButton_clicked();

    void on_stopButton_clicked();

    void on_connectButton_clicked();

private:
    typedef std::unique_ptr<QShortcut> ShortcutPtr;
#define CS_SHORTCUT_SUFFIX Shortcut
#define CS_DEFINE_SHORTCUT(name) \
    ShortcutPtr CONCAT(name, CS_SHORTCUT_SUFFIX);

    CS_DEFINE_SHORTCUT(play)
    CS_DEFINE_SHORTCUT(nextTrack)
    CS_DEFINE_SHORTCUT(stop)
    CS_DEFINE_SHORTCUT(repeatToggle)
    CS_DEFINE_SHORTCUT(randomToggle)
    CS_DEFINE_SHORTCUT(copyTitle)
    CS_DEFINE_SHORTCUT(folderListWidgetSelect)

    Ui::chumstreamer *ui;
    QNetworkAccessManager manager;
    //QVector<QNetworkReply* > replyVec;
    //QNetworkReply* reply;
    QString cacheFilePath;
    //QString server="";
    QString filePath;
    QUrl buildQueryString(const QString& = QString());
    void setMusicFolders();
    bool handleNetworkError(QNetworkReply*, const QString& = QString());
    void songInfoDisplay(bool);
    QStringList artistList;
    QVector<musicFolderInfo> musicFolderVec;
    void playlistAddFromChumListItem(ChumListItem*, bool);
    QVector<musicFolderInfo> prevDirIDVec;
    void keyPressEvent(QKeyEvent*);
    void addToPlaylistFromSlot(bool, ChumListItem* = nullptr);
    QMediaPlayer* player = new QMediaPlayer;
    QByteArray currentSong;
    const static int kDefaultVolume = 20;
    int getCurrentPlaylistIndex();
    void streamSong();
    bool chooseNext();
    //stores whether the user last pressed E or A, since we're going to be adding playlist items in slot functions that i don't know how to pass args to
    bool latestPrepend;
    void firstRecursiveRequest(QString);
    void playlistAddFromNode(QDomNode);
    void setImage(const QString&);
    void getSongInfo(QString);
    void grayOutPlaylist();
    QString currentAlbumArt;
    bool repeating();
    void toggleRepeating();
    void setRepeating(bool);
    bool random();
    void toggleRandom();
    void setRandom(bool);
    bool loadFromSave();
    bool hasRed();
    QString checkedFolders;
};

#endif // CHUMSTREAMER_H
