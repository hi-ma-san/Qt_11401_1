#ifndef MEDIAWIDGET_H
#define MEDIAWIDGET_H

#include "BaseComponent.h"
#include <QLabel>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStringList>

/**
 * @brief 音樂播放器組件
 * 繼承自 BaseComponent，支援播放資料夾內的音訊檔案
 */
class MediaWidget : public BaseComponent
{
    Q_OBJECT
public:
    explicit MediaWidget(QWidget *parent = nullptr);
    ~MediaWidget();

    /** --- 實作父類別虛擬函式 --- **/
    void updateData() override;
    void initStyle() override;

    /**
     * @brief 處理來自控制面板的特殊設定 (如資料夾路徑)
     * @param key 設定名稱 ("path")
     * @param value 設定值
     */
    void setCustomSetting(const QString &key, const QVariant &value) override;

    /** --- 狀態讀取 (用於 UI 同步) --- **/
    QString currentPath() const { return m_folderPath; }

private slots:
    void onPlayPauseClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    /** --- UI 組件 --- **/
    QLabel *m_titleLabel;        // 標題 "MUSIC"
    QLabel *m_currentSongLabel;  // 顯示當前歌曲名稱
    QPushButton *m_previousBtn;  // 上一首按鈕
    QPushButton *m_playPauseBtn; // 播放/暫停按鈕
    QPushButton *m_nextBtn;      // 下一首按鈕

    /** --- 播放器組件 --- **/
    QMediaPlayer *m_mediaPlayer;
    QAudioOutput *m_audioOutput;

    /** --- 播放清單與狀態 --- **/
    QString m_folderPath;        // 資料夾路徑
    QStringList m_playlist;      // 播放清單
    int m_currentIndex;          // 當前播放索引
    bool m_isPlaying;            // 播放狀態

    /** --- 內部邏輯處理 --- **/
    void loadPlaylist(const QString &folderPath);
    void playCurrentSong();
    void updateSongLabel();
    void updatePlayPauseButton();
};

#endif // MEDIAWIDGET_H
