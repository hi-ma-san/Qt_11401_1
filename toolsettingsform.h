#ifndef TOOLSETTINGSFORM_H
#define TOOLSETTINGSFORM_H

#include <QWidget>
#include <QVariantMap>

namespace Ui { class ToolSettingsForm; }

class ToolSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ToolSettingsForm(QWidget *parent = nullptr);
    ~ToolSettingsForm();

    void loadToolData(const QVariantMap &toolData);

private slots:
    void on_browsePath_button_clicked(); // 處理圖片複製或資料夾選擇
    void on_saveSettings_button_clicked(); // 儲存至本地設定

private:
    Ui::ToolSettingsForm *ui;
    QVariantMap m_currentToolData;
    void updateSpecificFields(const QString &toolId);
};

#endif
