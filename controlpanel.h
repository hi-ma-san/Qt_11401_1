#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>
#include <QList>
#include <QVariantMap>
#include "toolsettingsform.h"

namespace Ui { class ControlPanel; }

class ControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel();

private slots:
    // 處理左側小工具列表選擇變化
    void on_toolList_currentRowChanged(int currentRow);

    // 處理主題分頁的儲存按鈕
    void on_saveTheme_clicked();

    // 處理設定分頁的套用按鈕
    void on_applySetting_clicked();

private:
    Ui::ControlPanel *ui;

    // 儲存從 JSON 讀取的所有工具配置數據
    QList<QVariantMap> m_toolsData;

    // 右側嵌入式的通用設定表單
    ToolSettingsForm *m_settingsForm;

    // tools_config.json
    bool loadToolsConfiguration();
};

#endif // CONTROLPANEL_H
