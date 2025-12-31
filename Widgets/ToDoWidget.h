#ifndef TODOWIDGET_H
#define TODOWIDGET_H

#include "Core/BaseComponent.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>

class ToDoWidget : public BaseComponent {
    Q_OBJECT
public:
    explicit ToDoWidget(QWidget *parent = nullptr);
    ~ToDoWidget();

    void initStyle() override;
    void updateData() override; // Not strictly needed for Todo but required by BaseComponent

private:
    QLabel *m_titleLabel;
    QListWidget *m_listWidget;
    QLineEdit *m_inputEdit;
    QPushButton *m_addButton;

    void loadTodos();
    void saveTodos();
    void addNewItem(const QString &text, bool isDone = false);

private slots:
    void onAddClicked();
    void onItemChanged(QListWidgetItem *item);
};

#endif // TODOWIDGET_H
