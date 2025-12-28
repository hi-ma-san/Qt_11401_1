#include "ControlPanel.h"

#include <QApplication>

#include "BaseComponent.h"

// 建立一個臨時的測試類別
class TestWidget : public BaseComponent {
public:
    using BaseComponent::BaseComponent;
    void initStyle() override {

        this->setStyleSheet("background-color: rgba(255, 0, 0, 100); border: 2px solid red;");
    }
    void updateData() override { /* 測試暫不需要更新 */ }
};

// create TestWidget()
void createTestWidget()
{
    TestWidget *test = new TestWidget();
    test->initStyle(); // 初始化樣式
    test->resize(200, 200); // 設定大小
    test->show(); // 顯示出來
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControlPanel w;
    w.show();

    //createTestWidget();
    return a.exec();
}
