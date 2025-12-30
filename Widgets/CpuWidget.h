#ifndef CPUWIDGET_H
#define CPUWIDGET_H

#include "Core/BaseComponent.h"
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>

#ifdef Q_OS_WIN
#include <windows.h>
#include <pdh.h>
#include <powrprof.h>
#include <vector>
#endif

class CpuWidget : public BaseComponent {
    Q_OBJECT
public:
    enum FrequencyMode {
        FreqMax = 0,
        FreqAverage = 1
    };
    Q_ENUM(FrequencyMode)

    explicit CpuWidget(QWidget *parent = nullptr);
    ~CpuWidget(); // Add destructor to close Pdh

    void initStyle() override;
    void updateData() override;
    void setCustomSetting(const QString &key, const QVariant &value) override;
    void setUpdateInterval(int ms) override; // 新增

    FrequencyMode frequencyMode() const { return m_freqMode; }

private:
    QLabel *m_titleLabel;
    QLabel *m_cpuLabel;
    QLabel *m_ramLabel;
    // QLabel *m_coreLabel; // Removed single label
    QWidget *m_coresContainer; // Container for core labels
    QVBoxLayout *m_coresLayout; // Layout for core labels
    QLabel *m_ramDetailLabel;
    QTimer *m_updateTimer;

    bool m_showCores = false;
    bool m_showRamDetail = false;
    bool m_showCoreFreq = false; // 新增：是否顯示個別核心頻率
    FrequencyMode m_freqMode = FreqMax;

    // Windows specific members for CPU calculation
#ifdef Q_OS_WIN
    FILETIME m_preIdleTime;
    FILETIME m_preKernelTime;
    FILETIME m_preUserTime;

    // Pdh for per-core usage
    PDH_HQUERY m_pdhQuery;
    std::vector<PDH_HCOUNTER> m_coreCounters;
    std::vector<QLabel*> m_coreLabels;
    
    // Pdh for Processor Performance (Frequency)
    PDH_HQUERY m_pdhFreqQuery;
    std::vector<PDH_HCOUNTER> m_freqCounters;

    double getCpuUsage();
    int getMemoryUsage();
    void initPdh();
    QString updateCoreUsage(); // 修改回傳型別為 QString
#endif
};

#endif // CPUWIDGET_H
