#include "controlpanel.h"
#include "ui_controlpanel.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QVBoxLayout>
#include <QMessageBox>

ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlPanel),
    m_settingsForm(nullptr) // 初始化
{
    ui->setupUi(this);

    // 載入工具配置
    if (!loadToolsConfiguration()) {
        qCritical() << "Failed to load tool configurations.";
    }

    // 嵌入 ToolSettingsForm
    m_settingsForm = new ToolSettingsForm(ui->settingsContainer_widget);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->settingsContainer_widget->layout());
    if (!layout) {
        layout = new QVBoxLayout(ui->settingsContainer_widget);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    layout->addWidget(m_settingsForm);

    // 確保 ToolSettingsForm 顯示
    m_settingsForm->show();

    // 連接
    if (!m_toolsData.isEmpty()) {
        // 預設選中第一項並觸發載入
        ui->toolList_widget->setCurrentRow(0);

        // 連接到列表切換的槽
        connect(ui->toolList_widget, &QListWidget::currentRowChanged,
                this, &ControlPanel::on_toolList_currentRowChanged);
    }

    //連接主題與全域設定按鈕
    connect(ui->saveTheme_button, &QPushButton::clicked, this, &ControlPanel::on_saveTheme_clicked);
    connect(ui->applySetting_button, &QPushButton::clicked, this, &ControlPanel::on_applySetting_clicked);
}

// 讀取 JSON 配置
bool ControlPanel::loadToolsConfiguration()
{
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

    for (const QJsonValue &value : toolsArray) {
        QJsonObject toolObject = value.toObject();

        // 儲存
        m_toolsData.append(toolObject.toVariantMap());

        // 填充 QListWidget
        ui->toolList_widget->addItem(toolObject["name"].toString());
    }

    return true;
}

// 處理小工具列表選擇變化
void ControlPanel::on_toolList_currentRowChanged(int currentRow)
{
    if (currentRow >= 0 && currentRow < m_toolsData.size()) {
        const QVariantMap &toolData = m_toolsData.at(currentRow);
        m_settingsForm->loadToolData(toolData);

        qDebug() << "ControlPanel: Selected tool changed to:" << toolData["name"].toString();
    } else {
        qWarning() << "ControlPanel: Invalid row selected:" << currentRow;
    }
}

// 主題與設定處理函數

void ControlPanel::on_saveTheme_clicked()
{
    QString themeName = ui->themeName_lineEdit->text();
    if(themeName.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("請輸入主題名稱喵！"));
        return;
    }
    qDebug() << "正在儲存當前佈局為主題:" << themeName;
    // TODO: 實作儲存所有 Widget 狀態的 JSON 邏輯
}

void ControlPanel::on_applySetting_clicked()
{
    bool isLocked = ui->globalLockDrag_checkBox->isChecked();
    bool showTray = ui->trayIcon_checkBox->isChecked();

    qDebug() << "套用設定: 全域禁止拖曳=" << isLocked << ", 顯示通知圖示=" << showTray;
    // TODO: 透過 Manager 通知所有 Widget 更新行為
    QMessageBox::information(this, tr("設定"), tr("全域設定已成功套用喵！"));
}

ControlPanel::~ControlPanel()
{
    delete ui;
}
