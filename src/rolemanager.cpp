#include "rolemanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>

#define ROLES_FILE "roles.json"

RoleManager::RoleManager(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::RoleManager)
{
    ui->setupUi(this);
    
    // 连接信号槽（控件名称必须与.ui文件完全一致）
    connect(ui->addButton, &QPushButton::clicked, this, &RoleManager::onAddButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &RoleManager::onRemoveButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &RoleManager::onSaveButtonClicked);
    connect(ui->roleList, &QListWidget::itemClicked, this, &RoleManager::onRoleSelected);
    
    initializeDefaultRoles();
    loadCustomRoles();
    updateRoleList();
}

RoleManager::~RoleManager()
{
    saveCustomRoles();
    qDeleteAll(roles);
    delete ui;
}

void RoleManager::addCustomRole(const QString &name, const QString &description, const QString &prompt)
{
    if (roles.contains(name)) {
        emit errorOccurred("角色名称已存在");
        return;
    }
    
    CustomRole *role = new CustomRole(name, description, prompt, this);
    roles[name] = role;
    updateRoleList();
    emit roleAdded(name);
}

bool RoleManager::addRole(const QString &name, const QString &description, const QString &prompt)
{
    if (roles.contains(name)) {
        return false;
    }
    CustomRole* newRole = new CustomRole(name, description, prompt, this);
    roles.insert(name, newRole);
    saveCustomRoles();
    updateRoleList();
    emit roleAdded(name);
    return true;
}

void RoleManager::removeRole(const QString &name)
{
    if (!roles.contains(name)) {
        emit errorOccurred("角色不存在");
        return;
    }
    
    // 不允许删除内置角色
    if (name == "默认" || name == "律师" || name == "教师" || 
        name == "程序员" || name == "作家") {
        emit errorOccurred("不能删除内置角色");
        return;
    }
    
    delete roles.take(name);
    updateRoleList();
    clearForm();
    emit roleRemoved(name);
}

void RoleManager::editRole(const QString &name, const QString &description, const QString &prompt)
{
    if (!roles.contains(name)) {
        emit errorOccurred("角色不存在");
        return;
    }
    
    CustomRole *role = roles[name];
    role->setDescription(description);
    role->setPrompt(prompt);
    saveCustomRoles();
    updateRoleList();
    emit roleModified(name);
}

QStringList RoleManager::getRoleList() const
{
    return roles.keys();
}

CustomRole* RoleManager::getRole(const QString &name) const
{
    return roles.value(name);
}

void RoleManager::loadBuiltInRoles()
{
    // 添加内置角色
    addCustomRole("默认", "通用助手", "你是一个智能助手，能够帮助用户解决各种问题。你会以友好、专业的态度回答用户的问题，提供准确、有用的信息。在回答时，你会注意语言的简洁性和逻辑性，确保信息传达清晰。如果遇到不确定的问题，你会诚实地表示，并尽可能提供相关的参考信息。");
    addCustomRole("律师", "法律顾问", "你是一位专业的律师，精通中国法律体系。你能够为用户提供准确的法律咨询，包括但不限于民事、刑事、商事等领域的法律问题。在回答时，你会引用相关法律条文，解释法律概念，分析案件要点，提供专业的法律建议。你会始终保持客观、严谨的态度，并提醒用户最终决策需要咨询执业律师。");
    addCustomRole("教师", "教育辅导", "你是一位富有耐心的教师，擅长因材施教。你能够根据用户的学习水平和需求，提供个性化的学习指导。你可以解答各类学科问题，帮助学生理解难点，提供学习方法建议。在回答时，你会循序渐进地引导思考，鼓励提问，并适时给予肯定和鼓励。你会注重培养学生的独立思考能力和学习兴趣。");
    addCustomRole("程序员", "技术顾问", "你是一位资深程序员，精通多种编程语言和开发技术。你能够帮助用户解决编程问题，提供代码优化建议，解释技术概念。在回答时，你会注重代码的可读性、可维护性和性能优化。你可以提供最佳实践建议，帮助用户养成良好的编程习惯。对于复杂问题，你会提供清晰的解决思路和详细的实现方案。");
    addCustomRole("作家", "创作指导", "你是一位经验丰富的作家，擅长多种文学体裁的创作。你具有丰富的想象力和创造力，能够帮助用户构思故事情节、塑造人物形象、设计对话场景。你可以提供写作技巧指导，帮助用户改进文章结构、优化语言表达、增强情感描写。在回答时，你会结合具体的写作案例，给出实用的建议和修改意见。");
}


void RoleManager::saveCustomRoles()
{
    QJsonArray roleArray;
    for (auto it = roles.begin(); it != roles.end(); ++it) {
        CustomRole *role = it.value();
        if (role->getName() == "默认" || role->getName() == "律师" || 
            role->getName() == "教师" || role->getName() == "程序员" || 
            role->getName() == "作家") {
            continue;
        }
        
        QJsonObject roleObj;
        roleObj["name"] = role->getName();
        roleObj["description"] = role->getDescription();
        roleObj["prompt"] = role->getPrompt();
        roleArray.append(roleObj);
    }
    
    QJsonDocument doc(roleArray);
    QFile file(ROLES_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    } else {
        emit errorOccurred("无法保存角色配置文件");
    }
}

void RoleManager::loadCustomRoles()
{
    QFile file(ROLES_FILE);
    if (!file.exists()) {
        return;
    }
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        
        if (doc.isArray()) {
            QJsonArray roleArray = doc.array();
            for (const QJsonValue &value : roleArray) {
                QJsonObject roleObj = value.toObject();
                QString name = roleObj["name"].toString();
                QString description = roleObj["description"].toString();
                QString prompt = roleObj["prompt"].toString();
                
                if (!roles.contains(name)) {
                    addRole(name, description, prompt);
                }
            }
        }
    }
}

void RoleManager::initializeDefaultRoles()
{
    loadBuiltInRoles();
}

void RoleManager::updateRoleList()
{
    ui->roleList->clear();
    QStringList roleNames = roles.keys();
    roleNames.sort();
    foreach (const QString &name, roleNames) {
        ui->roleList->addItem(name);
    }
}

void RoleManager::clearForm()
{
    ui->nameEdit->clear();
    ui->descriptionEdit->clear();
    ui->styleEdit->clear();
}

void RoleManager::onAddButtonClicked()
{
    clearForm();
    ui->nameEdit->setFocus();
}

void RoleManager::onRemoveButtonClicked()
{
    QListWidgetItem *item = ui->roleList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "警告", "请先选择一个角色");
        return;
    }
    
    QString roleName = item->text();
    if (QMessageBox::question(this, "确认", QString("确定要删除角色 '%1' 吗?").arg(roleName),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        removeRole(roleName);
    }
}

void RoleManager::onSaveButtonClicked()
{
    QString name = ui->nameEdit->text().trimmed();
    QString description = ui->descriptionEdit->toPlainText().trimmed();
    QString prompt = ui->styleEdit->toPlainText().trimmed();
    
    if (name.isEmpty()) {
        QMessageBox::warning(this, "警告", "角色名称不能为空");
        return;
    }
    
    if (roles.contains(name)) {
        editRole(name, description, prompt);
        QMessageBox::information(this, "成功", "角色修改成功");
    } else {
        addRole(name, description, prompt);
        QMessageBox::information(this, "成功", "角色添加成功");
    }
}

void RoleManager::onRoleSelected(QListWidgetItem *item)
{
    if (!item) return;
    
    QString roleName = item->text();
    if (CustomRole *role = getRole(roleName)) {
        ui->nameEdit->setText(role->getName());
        ui->descriptionEdit->setText(role->getDescription());
        ui->styleEdit->setText(role->getPrompt());
    }
}