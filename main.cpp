#include "ControlPanel.h"
#include <QApplication>

/* for test */
#include "BaseComponent.h"
#include <QTimer> // 確保這行存在喵！
#include "SettingsManager.h"
/*------------------------------*/

// testwidget
class TestWidget : public BaseComponent {
public:
    using BaseComponent::BaseComponent;

    void updateData() override { /* 測試暫不需要更新 */ }
};

void createTestWidget()
{
    TestWidget *test = new TestWidget();
    test->initStyle(); // 初始化樣式
    test->resize(200, 200); // 設定大小
    test->show(); // 顯示出來

    //lock test
    QTimer::singleShot(5000, [](){
        SettingsManager::instance()->setGlobalDragLocked(true);
    });
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControlPanel w;
    w.show();

    //createTestWidget();
    return a.exec();
}
