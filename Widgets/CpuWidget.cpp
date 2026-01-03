#include "CpuWidget.h"
#include <QStyle>
#include <QDebug>
#include <QThread>

#ifdef Q_OS_WIN
// MinGW 某些版本可能缺少此結構定義，手動補上以避免編譯錯誤
typedef struct _PROCESSOR_POWER_INFORMATION {
    ULONG Number;
    ULONG MaxMhz;
    ULONG CurrentMhz;
    ULONG MhzLimit;
    ULONG MaxIdleState;
    ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
#endif

CpuWidget::CpuWidget(QWidget *parent) : BaseComponent(parent) {
    m_titleLabel = new QLabel("SYSTEM", this);
    m_cpuLabel = new QLabel("CPU: --%", this);
    m_ramLabel = new QLabel("RAM: --%", this);
    m_ramDetailLabel = new QLabel("Used: -- / -- GB", this);

    // 垂直佈局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 10, 15, 10);
    mainLayout->setSpacing(5);

    // 標題
    mainLayout->addWidget(m_titleLabel, 0, Qt::AlignLeft);

    // 內容
    mainLayout->addWidget(m_cpuLabel, 0, Qt::AlignLeft);
    
    // 核心列表容器
    m_coresContainer = new QWidget(this);
    m_coresLayout = new QVBoxLayout(m_coresContainer);
    m_coresLayout->setContentsMargins(10, 0, 0, 0); // 縮排
    m_coresLayout->setSpacing(2);
    mainLayout->addWidget(m_coresContainer);
    m_coresContainer->hide(); // 預設隱藏

    mainLayout->addWidget(m_ramLabel, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_ramDetailLabel, 0, Qt::AlignLeft);

    // 預設隱藏詳細資訊
    m_ramDetailLabel->hide();

    // 初始化 Windows CPU 計算變數
#ifdef Q_OS_WIN
    m_preIdleTime.dwLowDateTime = 0;
    m_preIdleTime.dwHighDateTime = 0;
    m_preKernelTime.dwLowDateTime = 0;
    m_preKernelTime.dwHighDateTime = 0;
    m_preUserTime.dwLowDateTime = 0;
    m_preUserTime.dwHighDateTime = 0;

    initPdh();
#endif

    // 定時器 (1秒更新一次)
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &CpuWidget::updateData);
    m_updateTimer->start(1000);

    initStyle();
    updateData();
}

CpuWidget::~CpuWidget() {
#ifdef Q_OS_WIN
    if (m_pdhQuery) {
        PdhCloseQuery(m_pdhQuery);
    }
    if (m_pdhFreqQuery) {
        PdhCloseQuery(m_pdhFreqQuery);
    }
#endif
}

void CpuWidget::initStyle() {
    BaseComponent::initStyle();

    // 樣式設定
    this->setStyleSheet(this->styleSheet() +
                        "CpuWidget {"
                        "  background-color: rgba(0, 0, 0, 150);"
                        "  border: 1px solid rgba(255, 255, 255, 30);"
                        "  border-radius: 8px;"
                        "}"
                        "QLabel { color: white; background: transparent; font-family: 'Segoe UI', 'Microsoft JhengHei'; }"
                        "#titleLabel { font-size: 14px; font-weight: bold; color: rgba(255, 255, 255, 220); margin-bottom: 2px; }"
                        "#cpuLabel, #ramLabel { font-size: 12px; color: rgba(255, 255, 255, 190); }"
                        "#ramDetailLabel { font-size: 10px; color: rgba(255, 255, 255, 150); margin-left: 5px; }"
                        );

    m_titleLabel->setObjectName("titleLabel");
    m_cpuLabel->setObjectName("cpuLabel");
    m_ramLabel->setObjectName("ramLabel");
    m_ramDetailLabel->setObjectName("ramDetailLabel");

    this->style()->unpolish(this);
    this->style()->polish(this);
}

void CpuWidget::setUpdateInterval(int ms) {
    BaseComponent::setUpdateInterval(ms);
    if (m_updateTimer) {
        m_updateTimer->setInterval(ms);
    }
}

void CpuWidget::setCustomSetting(const QString &key, const QVariant &value) {
    if (key == "showCores") {
        m_showCores = value.toBool();
        m_coresContainer->setVisible(m_showCores);
        this->adjustSize(); // 調整視窗大小以適應內容
    } else if (key == "showRamDetail") {
        m_showRamDetail = value.toBool();
        m_ramDetailLabel->setVisible(m_showRamDetail);
        this->adjustSize();
    } else if (key == "showCoreFreq") {
        m_showCoreFreq = value.toBool();
        // 這裡不需要 adjustSize，因為是在 updateCoreUsage 內動態改變文字長度
    } else if (key == "freqAlgo") {
        m_freqMode = static_cast<FrequencyMode>(value.toInt());
    }
}

void CpuWidget::updateData() {
#ifdef Q_OS_WIN
    double cpu = getCpuUsage();
    int ramPercent = getMemoryUsage();

    // 預設文字，稍後若有計算頻率會再更新
    QString cpuText = QString("CPU Usage: %1%").arg(QString::number(cpu, 'f', 1));
    
    // 無論是否顯示核心列表，都計算頻率
    // updateCoreUsage 現在會回傳最大頻率字串，並更新核心列表(如果顯示的話)
    QString freqStr = updateCoreUsage();
    
    // 只有在啟用頻率顯示時才附加到主標籤
    if (m_showCoreFreq && !freqStr.isEmpty()) {
        cpuText += QString(" @ %1").arg(freqStr);
    }

    m_cpuLabel->setText(cpuText);
    m_ramLabel->setText(QString("RAM Usage: %1%").arg(ramPercent));

    if (m_showRamDetail) {
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(MEMORYSTATUSEX);

        if (GlobalMemoryStatusEx(&memInfo)) {
            double totalGB = memInfo.ullTotalPhys / (1024.0 * 1024.0 * 1024.0);
            double usedGB = (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024.0 * 1024.0 * 1024.0);
            m_ramDetailLabel->setText(QString("Used: %1 / %2 GB").arg(QString::number(usedGB, 'f', 1)).arg(QString::number(totalGB, 'f', 1)));
        }
    }
#else
    m_cpuLabel->setText("CPU: N/A");
    m_ramLabel->setText("RAM: N/A");
#endif
}

#ifdef Q_OS_WIN
// 輔助函式：將 FILETIME 轉換為 unsigned long long
unsigned long long fileTimeToInt64(const FILETIME &ft) {
    return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime);
}

double CpuWidget::getCpuUsage() {
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return 0.0;
    }

    unsigned long long idle = fileTimeToInt64(idleTime);
    unsigned long long kernel = fileTimeToInt64(kernelTime);
    unsigned long long user = fileTimeToInt64(userTime);

    unsigned long long preIdle = fileTimeToInt64(m_preIdleTime);
    unsigned long long preKernel = fileTimeToInt64(m_preKernelTime);
    unsigned long long preUser = fileTimeToInt64(m_preUserTime);

    unsigned long long idleDiff = idle - preIdle;
    unsigned long long kernelDiff = kernel - preKernel;
    unsigned long long userDiff = user - preUser;

    // KernelTime 包含 IdleTime，所以總系統時間 = Kernel + User
    unsigned long long totalSys = kernelDiff + userDiff;

    double cpuPercent = 0.0;
    if (totalSys > 0) {
        // (Total - Idle) / Total
        cpuPercent = (double)(totalSys - idleDiff) * 100.0 / totalSys;
    }

    // 儲存目前時間供下次計算
    m_preIdleTime = idleTime;
    m_preKernelTime = kernelTime;
    m_preUserTime = userTime;

    // 限制範圍 0-100
    if (cpuPercent < 0) cpuPercent = 0;
    if (cpuPercent > 100) cpuPercent = 100;

    return cpuPercent;
}

int CpuWidget::getMemoryUsage() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo)) {
        return (int)memInfo.dwMemoryLoad;
    }
    return 0;
}

void CpuWidget::initPdh() {
    PDH_STATUS status = PdhOpenQuery(NULL, 0, &m_pdhQuery);
    if (status != ERROR_SUCCESS) {
        qWarning() << "PdhOpenQuery failed:" << status;
        return;
    }

    // 取得邏輯核心數
    int coreCount = QThread::idealThreadCount();
    m_coreCounters.resize(coreCount);
    m_coreLabels.resize(coreCount);

    for (int i = 0; i < coreCount; ++i) {
        // 建立計數器路徑: \Processor(i)\% Processor Time
        QString path = QString("\\Processor(%1)\\% Processor Time").arg(i);
        status = PdhAddCounter(m_pdhQuery, path.toStdWString().c_str(), 0, &m_coreCounters[i]);
        if (status != ERROR_SUCCESS) {
            qWarning() << "PdhAddCounter failed for core" << i << ":" << status;
        }

        // 預先建立 Label
        QLabel *lbl = new QLabel(QString("Core %1: --%").arg(i), m_coresContainer);
        lbl->setStyleSheet("font-size: 10px; color: rgba(255, 255, 255, 150); margin-left: 10px;");
        m_coresLayout->addWidget(lbl);
        m_coreLabels[i] = lbl;
    }
    
    // 第一次收集數據 (通常第一次會失敗或無數據)
    PdhCollectQueryData(m_pdhQuery);

    // --- 初始化頻率查詢 (Processor Performance) ---
    status = PdhOpenQuery(NULL, 0, &m_pdhFreqQuery);
    if (status == ERROR_SUCCESS) {
        m_freqCounters.resize(coreCount);
        for (int i = 0; i < coreCount; ++i) {
            // 嘗試使用 "\Processor Information(0,i)\% Processor Performance" (假設單插槽系統)
            // 格式: \Processor Information(NUMA,Index)\% Processor Performance
            QString path = QString("\\Processor Information(0,%1)\\% Processor Performance").arg(i);
            status = PdhAddCounter(m_pdhFreqQuery, path.toStdWString().c_str(), 0, &m_freqCounters[i]);
            
            if (status != ERROR_SUCCESS) {
                 // Fallback: 嘗試舊版 "\Processor Performance(i)\% Processor Performance"
                 path = QString("\\Processor Performance(%1)\\% Processor Performance").arg(i);
                 status = PdhAddCounter(m_pdhFreqQuery, path.toStdWString().c_str(), 0, &m_freqCounters[i]);
                 if (status != ERROR_SUCCESS) {
                     qWarning() << "Failed to add freq counter for core" << i;
                     m_freqCounters[i] = NULL;
                 }
            }
        }
        PdhCollectQueryData(m_pdhFreqQuery);
    }

    // --- 初始化溫度查詢 (Thermal Zone) ---
    // 注意：這在許多系統上可能無效，因為需要 ACPI 驅動支援

}

QString CpuWidget::updateCoreUsage() {
    if (!m_pdhQuery) return "";

    // 1. 取得 CPU 使用率 (PDH)
    PDH_STATUS status = PdhCollectQueryData(m_pdhQuery);
    if (status != ERROR_SUCCESS) return "";

    // 2. 取得 CPU 基礎頻率 (CallNtPowerInformation)
    int coreCount = m_coreCounters.size();
    std::vector<PROCESSOR_POWER_INFORMATION> ppi(coreCount);
    long pwrStatus = CallNtPowerInformation(ProcessorInformation, NULL, 0, &ppi[0], coreCount * sizeof(PROCESSOR_POWER_INFORMATION));
    
    // 3. 收集頻率效能數據
    if (m_pdhFreqQuery) {
        PdhCollectQueryData(m_pdhFreqQuery);
    }

    double maxFreq = 0.0;
    double sumFreq = 0.0;
    int validCoreCount = 0;

    for (int i = 0; i < coreCount; ++i) {
        // --- A. 取得使用率 ---
        PDH_FMT_COUNTERVALUE value;
        status = PdhGetFormattedCounterValue(m_coreCounters[i], PDH_FMT_DOUBLE, NULL, &value);
        
        double usage = 0.0;
        if (status == ERROR_SUCCESS) {
            usage = value.doubleValue;
        }

        // --- B. 計算個別核心頻率 ---
        double currentRealMhz = 0.0;
        double baseMhz = 0.0;
        
        // 取得該核心的基礎頻率
        if (pwrStatus == 0) {
            baseMhz = ppi[i].MaxMhz;
        }

        // 取得該核心的效能百分比
        double perfPercent = 100.0;
        if (m_pdhFreqQuery && i < m_freqCounters.size() && m_freqCounters[i]) {
            PDH_FMT_COUNTERVALUE freqVal;
            if (PdhGetFormattedCounterValue(m_freqCounters[i], PDH_FMT_DOUBLE, NULL, &freqVal) == ERROR_SUCCESS) {
                perfPercent = freqVal.doubleValue;
            }
        }

        // 計算真實頻率
        currentRealMhz = baseMhz * (perfPercent / 100.0);
        
        // 統計數據
        if (currentRealMhz > maxFreq) {
            maxFreq = currentRealMhz;
        }
        if (currentRealMhz > 0) {
            sumFreq += currentRealMhz;
            validCoreCount++;
        }

        // 僅更新使用率文字，不顯示頻率
        QString coreText = QString("Core %1: %2%").arg(i).arg(QString::number(usage, 'f', 1));
        
        // 若啟用個別核心頻率顯示
        if (m_showCoreFreq) {
            QString coreFreqStr;
            if (currentRealMhz >= 1000) {
                coreFreqStr = QString("%1 GHz").arg(QString::number(currentRealMhz / 1000.0, 'f', 2));
            } else {
                coreFreqStr = QString("%1 MHz").arg(QString::number(currentRealMhz, 'f', 0));
            }
            coreText += QString(" @ %1").arg(coreFreqStr);
        }
        
        m_coreLabels[i]->setText(coreText);
    }

    // 根據模式決定顯示數值
    double displayFreq = 0.0;
    if (m_freqMode == FreqAverage && validCoreCount > 0) {
        displayFreq = sumFreq / validCoreCount;
    } else {
        displayFreq = maxFreq;
    }

    // 回傳頻率字串
    QString freqStr;
    if (displayFreq >= 1000) {
        freqStr = QString("%1 GHz").arg(QString::number(displayFreq / 1000.0, 'f', 2));
    } else {
        freqStr = QString("%1 MHz").arg(QString::number(displayFreq, 'f', 0));
    }
    return freqStr;
}
#endif
