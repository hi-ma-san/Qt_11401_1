#ifndef BASECOMPONENT_H
#define BASECOMPONENT_H

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>

class BaseComponent : public QWidget
{
    Q_OBJECT
public:
    explicit BaseComponent(QWidget *parent = nullptr);
    virtual ~BaseComponent() = default;

    // 虛擬函式：由子類別實作具體內容
    virtual void initStyle() = 0;
    virtual void updateData() = 0; // 用於每秒更新

    // 設定與行為控制
    void setDraggable(bool enable) { m_isDraggable = enable; }
    void setSnapToEdge(bool enable) { m_isSnapEnabled = enable; }
    void setWindowLayer(int layer); // 0:普通, 1:置頂, 2:置底

protected:
    // 繪圖事件
    void paintEvent(QPaintEvent *event) override;

    // 滑鼠事件處理「拖曳」與「吸附」
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // 行為屬性
    bool m_isDraggable = true;
    bool m_isSnapEnabled = true;
    bool m_isMoving = false;
    QPoint m_dragPosition; // 紀錄滑鼠按下時的相對位置

    // 邊緣吸附
    void performSnap(QPoint &newPos);
};

#endif
