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

    m_mapPath = path;
    return true;
}

bool MindMapScene::saveMap()
{
    if (m_mapPath.isEmpty()) {
        QMessageBox::warning(nullptr, "警告", "请先创建或打开思维导图");
        return false;
    }

    // 在严格树状结构中，文件系统结构就是我们的保存格式
    // 这里可以添加元数据保存（如节点位置、颜色等）
    return true;
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

    // 设置位置（在父节点右侧）
    child->setPos(parent->pos() + QPointF(200, 0));

    // 创建连接
    Connection* connection = new Connection(parent, child);
    addItem(connection);
    connection->updatePath();

    // 添加父子关系
    parent->addChild(child);

    return child;
}

void MindMapScene::showNodeContextMenu(MindMapNode* node, const QPoint& screenPos)
{
    QMenu menu;

    QAction* editAction = menu.addAction("编辑节点");
    QAction* deleteAction = menu.addAction("删除节点");
    QAction* colorAction = menu.addAction("设置颜色");
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
        }
    }
    else if (selectedAction == deleteAction && !node->isRoot()) {
        // 删除节点及其所有连接
        removeNode(node);
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
}

void MindMapScene::showSceneContextMenu(const QPoint& screenPos, const QPointF& scenePos)
{
    Q_UNUSED(scenePos);

    QMenu menu;

    QAction* newAction = menu.addAction("新建思维导图");
    QAction* openAction = menu.addAction("打开思维导图");
    QAction* saveAction = menu.addAction("保存思维导图");

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