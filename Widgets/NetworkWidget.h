#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include "Core/BaseComponent.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QMap>

#ifdef Q_OS_WIN
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#endif

struct NetworkInterfaceUI {
    QWidget *rowWidget;
    QLabel *nameLabel;
    QLabel *uploadLabel;
    QLabel *downloadLabel;
};

class NetworkWidget : public BaseComponent {
    Q_OBJECT
public:
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();

    void initStyle() override;
    void updateData() override;
    void setUpdateInterval(int ms) override;
    void setCustomSetting(const QString &key, const QVariant &value) override;

    QStringList getAvailableInterfaces() const;
    bool isShowInBits() const { return m_showInBits; }
    QStringList getSelectedInterfaces() const { return m_selectedInterfaces; }

private:
    QLabel *m_titleLabel;
    QWidget *m_container;
    QVBoxLayout *m_containerLayout;
    QTimer *m_updateTimer;

    bool m_showInBits = false;
    QStringList m_selectedInterfaces; // List of names to show.
    QStringList m_interfaceList; // All available interfaces found by PDH

    // Map interface name to its UI elements
    QMap<QString, NetworkInterfaceUI> m_uiRows;

    // Helper to format speed string
    QString formatSpeed(double bytesPerSec);
    
    // Helper to create a new row
    void createInterfaceRow(const QString &name);
    void removeInterfaceRow(const QString &name);

#ifdef Q_OS_WIN
    PDH_HQUERY m_pdhQuery = NULL;
    PDH_HCOUNTER m_pdhCounterSent = NULL;
    PDH_HCOUNTER m_pdhCounterReceived = NULL;
    void initPdh();
#endif
};

#endif // NETWORKWIDGET_H
