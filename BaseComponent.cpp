#include "BaseComponent.h"
#include "SettingsManager.h"
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QScreen>

BaseComponent::BaseComponent(QWidget *parent) : QWidget(parent) {
    // 基礎視窗設定：無邊框、背景透明、工具視窗(不顯示在工作列)
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

void BaseComponent::initStyle() {
    // 在父類別定義通用的 QSS 樣式
    // 使用 .BaseComponent 語法可以確保子類別繼承這些外觀
    this->setStyleSheet(
        "BaseComponent {"
        "  background-color: rgba(30, 30, 30, 180);" /* 半透明深灰色背景 */
        "  border-radius: 20px;"                     /* 統一圓角 */
        "  border: 1px solid rgba(255, 255, 255, 40);" /* 細微的白色邊框線 */
        "}"
        );
}

void BaseComponent::setWindowLayer(int layer) {
    Qt::WindowFlags flags = this->windowFlags();
    if (layer == 1) { // 永遠置頂
        flags |= Qt::WindowStaysOnTopHint;
        flags &= ~Qt::WindowStaysOnBottomHint;
    } else if (layer == 2) { // 置於底部
        flags |= Qt::WindowStaysOnBottomHint;
        flags &= ~Qt::WindowStaysOnTopHint;
    } else { // 普通
        flags &= ~Qt::WindowStaysOnTopHint;
        flags &= ~Qt::WindowStaysOnBottomHint;
    }
    this->setWindowFlags(flags);
    this->show(); // 重新顯示以應用 flag
}

// --- 拖曳與吸附核心邏輯 ---
void BaseComponent::mousePressEvent(QMouseEvent *event) {
    // 1. 檢查全域禁止拖曳開關
    bool isGlobalLocked = SettingsManager::instance()->isGlobalDragLocked();

    // 2. 只有在「非全域鎖定」且「該元件允許拖曳」時，才啟動移動邏輯
    if (event->button() == Qt::LeftButton && !isGlobalLocked && m_isDraggable) {
        m_isMoving = true;
        m_dragPosition = event->globalPos() - this->frameGeometry().topLeft();
        event->accept();
    } else if (isGlobalLocked) {
        qDebug() << "喵！目前處於全域鎖定狀態，禁止移動！";
    }
}

void BaseComponent::mouseMoveEvent(QMouseEvent *event) {
    if (m_isMoving && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPos() - m_dragPosition;

        // 如果開啟吸附功能
        if (m_isSnapEnabled) {
            performSnap(newPos);
        }

        this->move(newPos);
        event->accept();
    }
}

void BaseComponent::mouseReleaseEvent(QMouseEvent *event) {
    m_isMoving = false;
}

void BaseComponent::performSnap(QPoint &newPos) {
    int snapMargin = 20; // 吸附感應範圍(px)

    // 獲取目前視窗所在的螢幕 (支援多螢幕)
    QScreen *currentScreen = this->screen();
    if (!currentScreen) currentScreen = QApplication::primaryScreen();

    // 使用 availableGeometry 以避開工作列
    QRect screenRect = currentScreen->availableGeometry();

    // 螢幕左邊緣 (相對於螢幕起始座標)
    if (qAbs(newPos.x() - screenRect.left()) < snapMargin)
        newPos.setX(screenRect.left());

    // 螢幕右邊緣
    if (qAbs(newPos.x() + this->width() - screenRect.right()) < snapMargin)
        newPos.setX(screenRect.right() - this->width());

    // 螢幕上邊緣
    if (qAbs(newPos.y() - screenRect.top()) < snapMargin)
        newPos.setY(screenRect.top());

    // 螢幕下邊緣
    if (qAbs(newPos.y() + this->height() - screenRect.bottom()) < snapMargin)
        newPos.setY(screenRect.bottom() - this->height());
}

void BaseComponent::paintEvent(QPaintEvent *event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
