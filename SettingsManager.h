#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    // 獲取單例實例
    static SettingsManager* instance();

    // --- 全域設定的 Getter 與 Setter ---

    // 全域禁止拖曳
    bool isGlobalDragLocked() const { return m_globalDragLocked; }
    void setGlobalDragLocked(bool locked);

    // 系統托盤顯示
    bool showTrayIcon() const { return m_showTrayIcon; }
    void setShowTrayIcon(bool show);

signals:
    // 當全域設定改變時發出信號，通知所有小工具
    void globalDragLockedChanged(bool locked);
    void trayIconSettingChanged(bool show);

private:
    explicit SettingsManager(QObject *parent = nullptr);
    static SettingsManager* m_instance;
    static QMutex m_mutex;

    // 儲存實際狀態的變數
    bool m_globalDragLocked = false;
    bool m_showTrayIcon = true;
};

#endif // SETTINGSMANAGER_H
