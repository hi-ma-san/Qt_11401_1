#include "SettingsManager.h"
#include <QDebug>

SettingsManager* SettingsManager::m_instance = nullptr;
QMutex SettingsManager::m_mutex;

SettingsManager::SettingsManager(QObject *parent) : QObject(parent) {}

SettingsManager* SettingsManager::instance() {
    if (!m_instance) {
        QMutexLocker locker(&m_mutex);
        if (!m_instance) {
            m_instance = new SettingsManager();
        }
    }
    return m_instance;
}

void SettingsManager::setGlobalDragLocked(bool locked) {
    if (m_globalDragLocked != locked) {
        m_globalDragLocked = locked;
        emit globalDragLockedChanged(locked);
        qDebug() << "SettingsManager: 全域拖曳鎖定狀態改為 ->" << (locked ? "鎖定" : "開啟");
    }
}

void SettingsManager::setShowTrayIcon(bool show) {
    if (m_showTrayIcon != show) {
        m_showTrayIcon = show;
        emit trayIconSettingChanged(show);
        qDebug() << "SettingsManager: 顯示在系統托盤 ->" << (show ? "顯示" : "關閉");
    }
}
