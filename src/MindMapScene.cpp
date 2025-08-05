#include "MindMapScene.h"
#include "Connection.h"
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QStyle>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QByteArray>
#include <QGraphicsView>

MindMapScene::MindMapScene(QObject* parent)
    : QGraphicsScene(parent), m_rootNode(nullptr)
{
}

bool MindMapScene::createNewMap(const QString& path)
{
    // 清除现有场景
    clear();
    m_rootNode = nullptr;

    // 创建根目录
    QDir dir(path);
    if (!dir.mkpath(".")) {
        QMessageBox::critical(nullptr, "错误", "无法创建根目录: " + path);
        return false;
    }

    // 创建根节点
    m_rootNode = new MindMapNode(dir.dirName(), path);
    addItem(m_rootNode);
    m_rootNode->setPos(0, 0);

    m_mapPath = path;
    updateLayout();
    return true;
}

bool MindMapScene::openMap(const QString& path)
{
    // 清除现有场景
    clear();
    m_rootNode = nullptr;

    // 检查目录是否存在
    QDir dir(path);
    if (!dir.exists()) {
        QMessageBox::critical(nullptr, "错误", "目录不存在: " + path);
        return false;
    }

    // 创建根节点
    m_rootNode = new MindMapNode(dir.dirName(), path);
    addItem(m_rootNode);
    m_rootNode->setPos(0, 0);

    // 递归加载子节点
    recursiveLoadChildren(m_rootNode);

    // 恢复连接
    restoreConnections(m_rootNode);

    m_mapPath = path;
    updateLayout();
    return true;
}

void MindMapScene::recursiveLoadChildren(MindMapNode* parent)
{
    if (!parent) return;

    QDir parentDir(parent->folderPath());
    QFileInfoList entries = parentDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo& entry : entries) {
        if (entry.isDir()) {
            QString childPath = entry.absoluteFilePath();

            // 创建子节点
            MindMapNode* child = new MindMapNode(entry.fileName(), childPath, parent);
            addItem(child);

            // 添加到父节点
            parent->addChild(child);

            // 递归加载子节点
            recursiveLoadChildren(child);
        }
    }
}

void MindMapScene::restoreConnections(MindMapNode* node)
{
    if (!node) return;

    // 加载节点的连接信息
    node->loadFromJson();

    // 递归处理子节点
    for (MindMapNode* child : node->children()) {
        restoreConnections(child);
    }
}

void MindMapScene::recursiveSave(MindMapNode* node)
{
    if (!node) return;

    // 保存当前节点
    node->saveToJson();

    // 递归保存子节点
    for (MindMapNode* child : node->children()) {
        recursiveSave(child);
    }
}

bool MindMapScene::saveMap()
{
    if (m_rootNode) {
        // 递归保存所有节点
        recursiveSave(m_rootNode);
        return true;
    }
    return false;
}



void MindMapScene::updateLayout()
{
    if (!m_rootNode) return;

    // 重置所有节点位置
    qreal y = 0;
    recursiveLayout(m_rootNode, 0, y, 0);

    // 更新视图
    if (!views().isEmpty()) {
        views().first()->centerOn(m_rootNode);
    }
}

void MindMapScene::recursiveLayout(MindMapNode* node, qreal x, qreal& y, int depth)
{
    if (!node->isExpanded()) return;

    // 设置当前节点位置
    node->setPosition(QPointF(x, y));
    y += 80; // 垂直间距

    // 布局子节点
    qreal childX = x + 200; // 水平缩进
    for (MindMapNode* child : node->children()) {
        recursiveLayout(child, childX, y, depth + 1);
    }
}

void MindMapScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        // 显示上下文菜单
        QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
        if (item && item->type() == MindMapNode::Type) {
            showNodeContextMenu(static_cast<MindMapNode*>(item), event->screenPos());
            event->accept();
            return;
        }
        else {
            // 传递场景坐标
            showSceneContextMenu(event->screenPos(), event->scenePos());
            event->accept();
            return;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

MindMapNode* MindMapScene::addChildNode(MindMapNode* parent, const QString& text)
{
    if (!parent) return nullptr;

    // 创建子节点
    MindMapNode* child = new MindMapNode(text, "");
    addItem(child);

    // 创建连接
    Connection* connection = new Connection(parent, child);
    addItem(connection);
    connection->updatePath();

    // 添加父子关系
    parent->addChild(child);

    // 更新布局
    updateLayout();

    return child;
}

void MindMapScene::showNodeContextMenu(MindMapNode* node, const QPoint& screenPos)
{
    QMenu menu;

    QAction* editAction = menu.addAction("编辑节点");
    QAction* deleteAction = menu.addAction("删除节点");
    QAction* colorAction = menu.addAction("设置颜色");
    menu.addSeparator();

    // 折叠/展开操作
    QAction* expandAction = nullptr;
    QAction* collapseAction = nullptr;

    if (!node->children().isEmpty()) {
        if (node->isExpanded()) {
            collapseAction = menu.addAction("折叠子节点");
        } else {
            expandAction = menu.addAction("展开子节点");
        }

        menu.addAction("切换折叠/展开");
    }

    menu.addSeparator();
    QAction* addChildAction = menu.addAction("添加子节点");
    menu.addSeparator();
    QAction* openFolderAction = menu.addAction("打开文件夹");

    // 根节点不能被删除
    if (node->isRoot()) {
        deleteAction->setEnabled(false);
    }

    QAction* selectedAction = menu.exec(screenPos);

    if (selectedAction == editAction) {
        bool ok;
        QString text = QInputDialog::getText(nullptr, "编辑节点", "输入新内容:",
                                           QLineEdit::Normal, node->text(), &ok);
        if (ok && !text.isEmpty()) {
            node->setText(text);
            updateLayout();
        }
    }
    else if (selectedAction == deleteAction && !node->isRoot()) {
        // 删除节点及其所有连接
        removeNode(node);
        updateLayout();
    }
    else if (selectedAction == colorAction) {
        node->setColor(QColor(rand() % 256, rand() % 256, rand() % 256));
    }
    else if (selectedAction == addChildAction) {
        bool ok;
        QString text = QInputDialog::getText(nullptr, "添加子节点", "输入节点内容:",
                                           QLineEdit::Normal, "子节点", &ok);
        if (ok && !text.isEmpty()) {
            addChildNode(node, text);
        }
    }
    else if (selectedAction == openFolderAction) {
        if (!node->folderPath().isEmpty()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(node->folderPath()));
        }
    }
    else if (selectedAction == expandAction) {
        node->setExpanded(true);
        updateLayout();
    }
    else if (selectedAction == collapseAction) {
        node->setExpanded(false);
        updateLayout();
    }
    else if (selectedAction && selectedAction->text() == "切换折叠/展开") {
        node->toggleExpanded();
        updateLayout();
    }
}

void MindMapScene::showSceneContextMenu(const QPoint& screenPos, const QPointF& scenePos)
{
    Q_UNUSED(scenePos);

    QMenu menu;

    QAction* newAction = menu.addAction("新建思维导图");
    QAction* openAction = menu.addAction("打开思维导图");
    QAction* saveAction = menu.addAction("保存思维导图");
    menu.addSeparator();
    QAction* expandAllAction = menu.addAction("全部展开");
    QAction* collapseAllAction = menu.addAction("全部折叠");

    QAction* selectedAction = menu.exec(screenPos);

    if (selectedAction == newAction) {
        QString path = QFileDialog::getExistingDirectory(nullptr, "选择根文件夹");
        if (!path.isEmpty()) {
            createNewMap(path);
        }
    }
    else if (selectedAction == openAction) {
        QString path = QFileDialog::getExistingDirectory(nullptr, "选择思维导图根文件夹");
        if (!path.isEmpty()) {
            openMap(path);
        }
    }
    else if (selectedAction == saveAction) {
        saveMap();
        QMessageBox::information(nullptr, "保存成功", "思维导图已保存到文件系统");
    }
    else if (selectedAction == expandAllAction && m_rootNode) {
        recursiveSetExpanded(m_rootNode, true);
        updateLayout();
    }
    else if (selectedAction == collapseAllAction && m_rootNode) {
        recursiveSetExpanded(m_rootNode, false);
        updateLayout();
    }
}

void MindMapScene::recursiveSetExpanded(MindMapNode* node, bool expanded)
{
    node->setExpanded(expanded);
    for (MindMapNode* child : node->children()) {
        recursiveSetExpanded(child, expanded);
    }
}

void MindMapScene::removeNode(MindMapNode* node)
{
    if (!node || node->isRoot()) return;

    // 递归删除子节点
    QList<MindMapNode*> children = node->children();
    for (MindMapNode* child : children) {
        removeNode(child);
    }

    // 删除连接
    QList<Connection*> connections = node->connections();
    for (Connection* connection : connections) {
        removeItem(connection);
        delete connection;
    }

    // 从父节点中移除
    if (node->parentNode()) {
        node->parentNode()->removeChild(node);
    }

    // 删除节点本身
    removeItem(node);
    delete node;
}