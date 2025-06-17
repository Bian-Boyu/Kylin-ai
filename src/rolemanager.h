#ifndef ROLEMANAGER_H
#define ROLEMANAGER_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QListWidget>
#include "customrole.h"
#include "ui_rolemanager.h"



class RoleManager : public QWidget
{
    Q_OBJECT

public:
    explicit RoleManager(QWidget *parent = nullptr);
    ~RoleManager();

    // 角色管理方法
    void addCustomRole(const QString &name, const QString &description, const QString &prompt);
    bool addRole(const QString &name, const QString &description, const QString &prompt);
    void removeRole(const QString &name);
    void editRole(const QString &name, const QString &description, const QString &prompt);
    QStringList getRoleList() const;
    CustomRole* getRole(const QString &name) const;

signals:
    void roleAdded(const QString &name);
    void roleRemoved(const QString &name);
    void roleModified(const QString &name);
    void roleSelected(const QString &name);
    void errorOccurred(const QString &error);

private slots:
    void onAddButtonClicked();
    void onRemoveButtonClicked();
    void onSaveButtonClicked();
    void onRoleSelected(QListWidgetItem *item);

private:
    Ui::RoleManager *ui;  // Ui指针声明
    QMap<QString, CustomRole*> roles;
    
    void loadBuiltInRoles();
    void saveCustomRoles();
    void loadCustomRoles();
    void initializeDefaultRoles();
    void updateRoleList();
    void clearForm();
};

#endif // ROLEMANAGER_H