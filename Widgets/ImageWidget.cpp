#include "ImageWidget.h"
#include <QVBoxLayout>
#include <QFileInfo>
#include <QMovie>
#include <QDebug>

ImageWidget::ImageWidget(QWidget *parent)
    : BaseComponent(parent), m_gifMovie(nullptr), m_scale(100)
{
    // 初始化顯示標籤
    m_displayLabel = new QLabel(this);
    m_displayLabel->setAlignment(Qt::AlignCenter);
    m_displayLabel->setScaledContents(true); // 讓媒體內容自動填滿標籤範圍

    // 設定佈局並預留 15px 邊框
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->addWidget(m_displayLabel);

    this->initStyle();
    this->resize(200, 200); // 初始預設大小
}

void ImageWidget::initStyle() {
    /** 呼叫父類別基礎樣式 **/
    BaseComponent::initStyle();

    // 讓標籤本身保持透明
    m_displayLabel->setStyleSheet("background: transparent; border: none;");
}

/** --- 行為設定處理 --- **/

void ImageWidget::setCustomSetting(const QString &key, const QVariant &value) {
    if (key == "clear_media") {
        /** 核心邏輯：立即釋放檔案鎖定 讓外部可以進行刪除或覆蓋 **/
        m_displayLabel->setMovie(nullptr);
        m_displayLabel->setPixmap(QPixmap());
        if (m_gifMovie) {
            m_gifMovie->stop();
            delete m_gifMovie; // 立刻 delete 釋放控制權
            m_gifMovie = nullptr;
        }
        qDebug() << "ImageWidget: 已釋放媒體資源 準備更換檔案";
    }
    else if (key == "path") {
        loadMedia(value.toString());
    }
    else if (key == "scale") {
        m_scale = value.toInt();
        updateComponentSize(); // 呼叫統一的尺寸更新邏輯
    }
}

/** --- 媒體載入與尺寸運算核心 --- **/

void ImageWidget::loadMedia(const QString &path) {
    if (path.isEmpty()) return;
    m_currentPath = path;

    // 清理舊的 GIF 播放器
    if (m_gifMovie) {
        m_gifMovie->stop();
        delete m_gifMovie;
        m_gifMovie = nullptr;
    }

    QFileInfo info(path);
    QString ext = info.suffix().toLower();
    QSize rawSize;

    // 根據格式載入媒體喵
    if (ext == "gif") {
        m_gifMovie = new QMovie(path);
        // 先暫時啟動以獲取第一幀的大小
        m_gifMovie->start();
        rawSize = m_gifMovie->currentPixmap().size();
        m_displayLabel->setMovie(m_gifMovie);
    } else {
        m_displayLabel->setMovie(nullptr);
        QPixmap pix(path);
        if (!pix.isNull()) {
            rawSize = pix.size();
            m_displayLabel->setPixmap(pix);
        }
    }

    // 3. 計算 400x400 上限內的基準尺寸 (不含 Margins)
    if (!rawSize.isEmpty()) {
        m_baseSize = rawSize.scaled(400, 400, Qt::KeepAspectRatio);
        updateComponentSize(); // 立即應用目前的縮放比例喵
    }
}

void ImageWidget::updateComponentSize() {
    if (!m_baseSize.isValid()) return;

    // 根據百分比計算內容尺寸喵
    QSize contentSize = m_baseSize * (m_scale / 100.0);

    // 標籤設定為內容大小
    m_displayLabel->setFixedSize(contentSize);

    // 視窗大小 = 內容大小 + 左右/上下邊框 (15 * 2 = 30)
    QSize windowSize = contentSize + QSize(30, 30);

    this->setFixedSize(windowSize);

    // 如果是 GIF 需要告訴 QMovie 現在要縮放成什麼大小
    if (m_gifMovie) {
        m_gifMovie->setScaledSize(contentSize);
    }
}

void ImageWidget::updateData() {
    /** 圖片組件無須週期性更新資料 **/
}

ImageWidget::~ImageWidget() {
    if (m_gifMovie) m_gifMovie->stop();
}
