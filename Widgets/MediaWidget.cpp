#include "MediaWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

MediaWidget::MediaWidget(QWidget *parent)
    : BaseComponent(parent),
      m_currentIndex(-1),
      m_isPlaying(false)
{
    // 初始化播放器
    m_mediaPlayer = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_mediaPlayer->setAudioOutput(m_audioOutput);

    // 初始化 UI 組件
    m_titleLabel = new QLabel("MUSIC", this);
    m_currentSongLabel = new QLabel("No song loaded", this);
    m_previousBtn = new QPushButton("<", this);
    m_playPauseBtn = new QPushButton("▶︎", this);
    m_nextBtn = new QPushButton(">", this);

    // 設定按鈕大小
    m_previousBtn->setFixedSize(40, 40);
    m_playPauseBtn->setFixedSize(40, 40);
    m_nextBtn->setFixedSize(40, 40);

    // 建立佈局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(5);

    // 添加標題
    mainLayout->addWidget(m_titleLabel, 0, Qt::AlignCenter);

    // 添加當前歌曲標籤
    m_currentSongLabel->setWordWrap(true);
    m_currentSongLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_currentSongLabel);

    // 添加控制按鈕
    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addStretch();
    controlLayout->addWidget(m_previousBtn);
    controlLayout->addWidget(m_playPauseBtn);
    controlLayout->addWidget(m_nextBtn);
    controlLayout->addStretch();
    mainLayout->addLayout(controlLayout);

    // 連接訊號
    connect(m_playPauseBtn, &QPushButton::clicked, this, &MediaWidget::onPlayPauseClicked);
    connect(m_previousBtn, &QPushButton::clicked, this, &MediaWidget::onPreviousClicked);
    connect(m_nextBtn, &QPushButton::clicked, this, &MediaWidget::onNextClicked);
    connect(m_mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MediaWidget::onMediaStatusChanged);
    connect(m_mediaPlayer, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState state) {
        m_isPlaying = (state == QMediaPlayer::PlayingState);
        updatePlayPauseButton();
    });

    initStyle();
    this->setFixedSize(220, 140);
}

MediaWidget::~MediaWidget()
{
    if (m_mediaPlayer) {
        m_mediaPlayer->stop();
    }
}

void MediaWidget::initStyle()
{
    BaseComponent::initStyle();

    // 設定樣式
    this->setStyleSheet(this->styleSheet() +
                        "MediaWidget {"
                        "  background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
                        "    stop:0 rgba(0, 0, 0, 150), stop:0.55 rgba(0, 0, 0, 150), "
                        "    stop:0.56 rgba(30, 30, 30, 200), stop:1 rgba(30, 30, 30, 200)); "
                        "  border: 1px solid rgba(255, 255, 255, 30);"
                        "}"
                        "QLabel { color: white; background: transparent; font-family: 'Segoe UI', 'Microsoft JhengHei'; }"
                        "#titleLabel { font-size: 18px; font-weight: 600; letter-spacing: 1px; }"
                        "#songLabel { font-size: 11px; color: rgba(255, 255, 255, 180); }"
                        "QPushButton {"
                        "  background-color: rgba(255, 255, 255, 30);"
                        "  border: 1px solid rgba(255, 255, 255, 50);"
                        "  border-radius: 20px;"
                        "  color: white;"
                        "  font-size: 18px;"
                        "  font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "  background-color: rgba(255, 255, 255, 50);"
                        "}"
                        "QPushButton:pressed {"
                        "  background-color: rgba(255, 255, 255, 70);"
                        "}"
                        );

    m_titleLabel->setObjectName("titleLabel");
    m_currentSongLabel->setObjectName("songLabel");

    // 更新樣式
    this->style()->unpolish(this);
    this->style()->polish(this);
}

void MediaWidget::updateData()
{
    // 這個 widget 不需要定時更新
}

void MediaWidget::setCustomSetting(const QString &key, const QVariant &value)
{
    if (key == "path") {
        loadPlaylist(value.toString());
    }
}

void MediaWidget::loadPlaylist(const QString &folderPath)
{
    m_folderPath = folderPath;
    m_playlist.clear();
    m_currentIndex = -1;

    QDir dir(folderPath);
    if (!dir.exists()) {
        qDebug() << "MediaWidget: 資料夾不存在:" << folderPath;
        m_currentSongLabel->setText("Folder not found");
        return;
    }

    // 支援的音訊格式
    QStringList filters;
    filters << "*.mp3" << "*.wav" << "*.flac" << "*.ogg" << "*.m4a";
    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files);

    QFileInfoList fileList = dir.entryInfoList();
    for (const QFileInfo &fileInfo : fileList) {
        m_playlist.append(fileInfo.absoluteFilePath());
    }

    qDebug() << "MediaWidget: 載入了" << m_playlist.size() << "首歌曲";

    if (m_playlist.isEmpty()) {
        m_currentSongLabel->setText("No audio files found");
    } else {
        m_currentIndex = 0;
        updateSongLabel();
    }
}

void MediaWidget::playCurrentSong()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size()) {
        return;
    }

    QString filePath = m_playlist[m_currentIndex];
    m_mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    m_mediaPlayer->play();
    updateSongLabel();

    qDebug() << "MediaWidget: 播放" << filePath;
}

void MediaWidget::updateSongLabel()
{
    if (m_currentIndex < 0 || m_currentIndex >= m_playlist.size()) {
        m_currentSongLabel->setText("No song loaded");
        return;
    }

    QFileInfo fileInfo(m_playlist[m_currentIndex]);
    m_currentSongLabel->setText(fileInfo.fileName());
}

void MediaWidget::updatePlayPauseButton()
{
    if (m_isPlaying) {
        m_playPauseBtn->setText("||");
    } else {
        m_playPauseBtn->setText("▶︎");
    }
}

void MediaWidget::onPlayPauseClicked()
{
    if (m_playlist.isEmpty()) {
        return;
    }

    if (m_isPlaying) {
        m_mediaPlayer->pause();
    } else {
        if (m_mediaPlayer->playbackState() == QMediaPlayer::StoppedState) {
            // 如果完全停止了，重新播放當前歌曲
            playCurrentSong();
        } else {
            // 如果只是暫停，繼續播放
            m_mediaPlayer->play();
        }
    }
}

void MediaWidget::onPreviousClicked()
{
    if (m_playlist.isEmpty()) {
        return;
    }

    m_currentIndex--;
    if (m_currentIndex < 0) {
        m_currentIndex = m_playlist.size() - 1; // 循環到最後一首
    }

    playCurrentSong();
}

void MediaWidget::onNextClicked()
{
    if (m_playlist.isEmpty()) {
        return;
    }

    m_currentIndex++;
    if (m_currentIndex >= m_playlist.size()) {
        m_currentIndex = 0; // 循環到第一首
    }

    playCurrentSong();
}

void MediaWidget::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        // 歌曲播放完畢，自動播放下一首
        onNextClicked();
    }
}
