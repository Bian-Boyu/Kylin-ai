#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include "ui_chatwindow.h"  // 确保路径正确
#include "rolemanager.h"
#include "customrole.h"
#include "customroledialog.h"
#include "chatwindow.h"   // 包含 ChatWindow 的定义

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchToChat();
    void switchToRoleManager();
    void handleRoleSelected(const QString &roleName);
    void handleNewRole(const RoleData &data);
    void updateRoleList(); // 添加声明

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    QWidget *chatPage;
    QWidget *rolePage;
    ChatWindow *chatWindow;  
    RoleManager *roleManager;
    
    void setupUi();
    void setupConnections();
};

#endif // MAINWINDOW_H