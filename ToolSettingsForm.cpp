#include "ToolSettingsForm.h"
#include "ui_ToolSettingsForm.h"
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QCoreApplication>

ToolSettingsForm::ToolSettingsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolSettingsForm)
{
    ui->setupUi(this);
    ui->data_media_group->hide();
}

ToolSettingsForm::~ToolSettingsForm() { delete ui; }

void ToolSettingsForm::loadToolData(const QVariantMap &toolData)
{
    m_currentToolData = toolData;
    QString toolId = m_currentToolData["id"].toString();

    // 載入基本開關
    ui->enableTool_checkBox->setChecked(m_currentToolData["default_enabled"].toBool());

    // 更新介面顯示
    updateSpecificFields(toolId);
}

void ToolSettingsForm::updateSpecificFields(const QString &toolId)
{
    ui->data_media_group->hide();

    if (toolId == "media_player")
    {
        ui->data_media_group->setTitle("音樂路徑設定");
        ui->path_label->setText("音樂資料夾:");
        ui->data_media_group->show();
    } else if (toolId == "image_viewer")
    {
        ui->data_media_group->setTitle("圖片備份設定");
        ui->path_label->setText("當前圖片:");
        ui->data_media_group->show();
    }
}

void ToolSettingsForm::on_browsePath_button_clicked()
{
    QString toolId = m_currentToolData["id"].toString();

    if (toolId == "image_viewer")
    {
        QString filePath = QFileDialog::getOpenFileName(this, "選擇要顯示的圖片/GIF", "", "Images (*.png *.jpg *.jpeg *.gif)");
        if (!filePath.isEmpty())
        {
            // 實作備份
            QString saveDir = QCoreApplication::applicationDirPath() + "/user_assets/images/";
            QDir().mkpath(saveDir); // 確保資料夾存在

            QFileInfo info(filePath);
            QString targetPath = saveDir + "display_image." + info.suffix();

            if (QFile::exists(targetPath)) QFile::remove(targetPath);
            if (QFile::copy(filePath, targetPath))
            {
                ui->musicPath_lineEdit->setText(targetPath);
                qDebug() << "已成功備份到：" << targetPath;
            }
        }
    }
    else if (toolId == "media_player")
    {
        QString dir = QFileDialog::getExistingDirectory(this, "選擇音樂資料夾");
        if (!dir.isEmpty()) ui->musicPath_lineEdit->setText(dir);
    }
}

void ToolSettingsForm::on_saveSettings_button_clicked()
{
    // 這裡未來會實作將 UI 上的值存入 QSettings 或 JSON
    qDebug() << "正在為" << m_currentToolData["id"].toString() << "儲存設定內容...";
    // 例如：SettingsManager::instance()->save(m_currentToolData["id"].toString(), ...);
}
