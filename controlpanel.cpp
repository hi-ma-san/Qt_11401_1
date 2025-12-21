#include "controlpanel.h"
#include "ui_controlpanel.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QVBoxLayout>

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    m_settingsForm(nullptr) // 初始化
{
    ui->setupUi(this);

    // 1. 載入工具配置
    if (!loadToolsConfiguration()) {
        qCritical() << "Failed to load tool configurations.";
        // 可以在這裡顯示錯誤訊息給使用者
    }

    // 2. 實例化並嵌入 ToolSettingsForm
    //    【注意】：假設右側容器 QWidget 名為 settingsContainer_widget

    m_settingsForm = new ToolSettingsForm(ui->settingsContainer_widget);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->settingsContainer_widget->layout());
    if (!layout) {
        // 如果容器還沒有佈局管理器，則創建一個新的
        layout = new QVBoxLayout(ui->settingsContainer_widget);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    layout->addWidget(m_settingsForm);

    // 確保 ToolSettingsForm 顯示
    m_settingsForm->show();

    // 3. 連接信號
    if (!m_toolsData.isEmpty()) {
        // 預設選中第一項並觸發載入
        ui->toolList_widget->setCurrentRow(0);

        // 連接到 ControlPanel 自己的槽
        connect(ui->toolList_widget, &QListWidget::currentRowChanged,
                this, &ControlPanel::on_toolList_currentRowChanged);
    }
}

//讀取JSON
bool ControlPanel::loadToolsConfiguration()
{
    // 假設 JSON 檔案已放入 resources.qrc 中的 :/config/ 前綴
    QFile loadFile(":/config/tools_config.json");

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open tools_config.json file.");
        return false;
    }

    QByteArray data = loadFile.readAll();
    QJsonDocument jsonDoc(QJsonDocument::fromJson(data));

    if (!jsonDoc.isArray()) {
        qWarning("JSON root is not an array.");
        return false;
    }

    QJsonArray toolsArray = jsonDoc.array();

    // 遍歷 JSON 陣列，動態填充列表
    for (const QJsonValue &value : toolsArray) {
        QJsonObject toolObject = value.toObject();

        // 1. 儲存數據 (QVariantMap)
        QVariantMap toolMap = toolObject.toVariantMap();
        m_toolsData.append(toolMap);

        // 2. 填充 QListWidget (左側清單)
        QString toolName = toolObject["name"].toString();
        // 【優化】這裡可以加入 icon_path，讓列表顯示圖示
        QListWidgetItem *item = new QListWidgetItem(toolName);

        // 假設 icon_path 存在且是有效的資源路徑
        // item->setIcon(QIcon(toolObject["icon_path"].toString()));

        ui->toolList_widget->addItem(item); // 將項目加入列表
    }

    return true;
}
void ControlPanel::on_toolList_currentRowChanged(int currentRow)
{
    qDebug()  << currentRow;
    // 檢查行號是否在有效的數據範圍內
    if (currentRow >= 0 && currentRow < m_toolsData.size()) {

        // 1. 取得當前選中小工具的完整 JSON 數據 (QVariantMap)
        const QVariantMap &toolData = m_toolsData.at(currentRow);

        // 2. 呼叫 ToolSettingsForm 的載入函式，傳遞數據
        //    這一步需要 ToolSettingsForm 具備這個 public 函式：
        m_settingsForm->loadToolData(toolData);

        qDebug() << "ControlPanel: Selected tool changed to:" << toolData["name"].toString();
    } else {
        qWarning() << "ControlPanel: Invalid row selected:" << currentRow;
    }
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
