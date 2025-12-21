#ifndef CONTROLSPANEL_H
#define CONTROLSPANEL_H

#include <QWidget>
#include <QList>
#include <QVariantMap>
#include "ToolSettingsForm.h" // 引入單一設定表單

namespace Ui { class ControlPanel; }

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

private slots:
    void on_toolList_currentRowChanged(int currentRow); // 處理列表選擇變化的槽

private:
    Ui::ControlPanel *ui;
    QList<QVariantMap> m_toolsData; // 儲存 JSON 數據

    ToolSettingsForm *m_settingsForm; // 單一通用設定表單的實例

    bool loadToolsConfiguration(); // 載入 JSON 配置
};

#endif // CONTROLSPANEL_H
