#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include "BaseComponent.h"
#include <QLabel>
#include <QMovie>
#include <QSize>

/**
 * @brief 圖片與 GIF 顯示組件
 * 繼承自 BaseComponent，支援縮放、邊框預留以及動態 GIF 播放
 */
class ImageWidget : public BaseComponent
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = nullptr);
    ~ImageWidget();

    /** --- 實作父類別虛擬函式 --- **/
    void updateData() override;
    void initStyle() override;

    /**
     * @brief 處理來自控制面板的特殊設定 (如圖片路徑、縮放比例)
     * @param key 設定名稱 ("path", "scale")
     * @param value 設定值
     */
    void setCustomSetting(const QString &key, const QVariant &value) override;

    /** --- 狀態讀取 (用於 UI 同步) --- **/
    int currentScale() const { return m_scale; }       // 取得目前縮放百分比
    QString currentPath() const { return m_currentPath; } // 取得目前檔案路徑

private:
    /** --- 內部顯示組件 --- **/
    QLabel *m_displayLabel; // 承載圖片或 GIF 的標籤
    QMovie *m_gifMovie;     // GIF 動畫撥放器

    /** --- 尺寸與狀態變數 --- **/
    QString m_currentPath;  // 目前載入的媒體路徑
    QSize m_baseSize;       // 在 400x400 限制下的基準尺寸 (不含邊框)
    int m_scale;            // 目前縮放比例 (25 - 100)

    /** --- 內部邏輯處理 --- **/

    /** @brief 根據路徑判斷並載入媒體內容 (圖片或 GIF) **/
    void loadMedia(const QString &path);

    /** @brief 根據 m_baseSize 與 m_scale 重新計算視窗與標籤尺寸 **/
    void updateComponentSize();
};

#endif // IMAGEWIDGET_H
