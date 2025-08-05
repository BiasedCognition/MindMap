#include "MindMapNode.h"
#include "Connection.h"
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPen>
#include <QStyle>
#include <QDir>
#include <QGraphicsSceneMouseEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

MindMapNode::MindMapNode(const QString& text, const QString& path, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_text(text), m_folderPath(path), m_color(QColor(135, 206, 250)),
      m_expanded(true), m_loading(false)
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    // 确保文件夹存在
    if (!m_folderPath.isEmpty()) {
        QDir dir(m_folderPath);
        dir.mkpath(".");

        // 如果JSON文件存在，则加载
        if (QFile::exists(dir.filePath("node.json"))) {
            m_loading = true;
            loadFromJson();
            m_loading = false;
        } else {
            // 否则创建初始JSON
            saveToJson();
        }
    }
}

MindMapNode::~MindMapNode()
{
    // 保存节点信息
    saveToJson();
}

QRectF MindMapNode::boundingRect() const
{
    QFontMetrics fm(QApplication::font());
    int width = fm.horizontalAdvance(m_text) + 50; // 增加空间给展开按钮
    int height = fm.height() + 20;
    return QRectF(-width/2, -height/2, width, height);
}

void MindMapNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    QRectF rect = boundingRect();

    // 根节点使用不同的颜色
    QColor nodeColor = isRoot() ? QColor(255, 165, 0) : m_color;

    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, nodeColor.lighter(120));
    gradient.setColorAt(1, nodeColor);

    painter->setBrush(gradient);
    painter->setPen(QPen(isSelected() ? Qt::blue : Qt::darkGray, isSelected() ? 2 : 1));
    painter->drawRoundedRect(rect, 10, 10);

    painter->setPen(Qt::black);
    painter->drawText(rect, Qt::AlignCenter, m_text);

    // 如果有子节点，绘制展开/折叠按钮
    if (!m_children.isEmpty()) {
        QRectF buttonRect = expandButtonRect();
        painter->setBrush(Qt::white);
        painter->drawRect(buttonRect);

        painter->setPen(QPen(Qt::black, 2));
        // 绘制减号（展开状态）或加号（折叠状态）
        painter->drawLine(buttonRect.left() + 5, buttonRect.center().y(),
                         buttonRect.right() - 5, buttonRect.center().y());
        if (!m_expanded) {
            painter->drawLine(buttonRect.center().x(), buttonRect.top() + 5,
                             buttonRect.center().x(), buttonRect.bottom() - 5);
        }
    }

    if (option->state & QStyle::State_MouseOver) {
        painter->setBrush(QColor(255, 255, 255, 100));
        painter->drawRoundedRect(rect, 10, 10);
    }
}

QRectF MindMapNode::expandButtonRect() const
{
    QRectF rect = boundingRect();
    return QRectF(rect.right() - 25, rect.center().y() - 8, 16, 16);
}

void MindMapNode::setExpanded(bool expanded)
{
    if (m_expanded == expanded) return;

    m_expanded = expanded;

    // 设置子节点可见性
    for (MindMapNode* child : m_children) {
        child->setVisible(expanded);
        // 递归设置子节点的连接线可见性
        for (Connection* conn : child->connections()) {
            conn->setVisible(expanded);
        }
    }

    // 设置直接连接线的可见性
    for (Connection* conn : m_connections) {
        if (conn->destinationNode() && conn->destinationNode()->parentNode() == this) {
            conn->setVisible(expanded);
        }
    }

    // 保存状态
    if (!m_loading) saveToJson();

    update();
}

void MindMapNode::toggleExpanded()
{
    setExpanded(!m_expanded);
}

void MindMapNode::setText(const QString& text)
{
    m_text = text;
    update();

    // 保存状态
    if (!m_loading) saveToJson();
}

QString MindMapNode::text() const
{
    return m_text;
}

void MindMapNode::setColor(const QColor& color)
{
    m_color = color;
    update();

    // 保存状态
    if (!m_loading) saveToJson();
}

QColor MindMapNode::color() const
{
    return m_color;
}

void MindMapNode::setFolderPath(const QString& path)
{
    m_folderPath = path;

    // 保存状态
    if (!m_loading) saveToJson();
}

QString MindMapNode::folderPath() const
{
    return m_folderPath;
}

QDir MindMapNode::directory() const
{
    return QDir(m_folderPath);
}

void MindMapNode::addChild(MindMapNode* child)
{
    if (!m_children.contains(child)) {
        m_children.append(child);

        // 设置子节点的文件夹路径
        if (!m_folderPath.isEmpty()) {
            QDir dir(m_folderPath);
            dir.mkpath(child->text());
            child->setFolderPath(dir.filePath(child->text()));
        }
    }

    // 保存状态
    if (!m_loading) saveToJson();
}

void MindMapNode::removeChild(MindMapNode* child)
{
    m_children.removeAll(child);

    // 保存状态
    if (!m_loading) saveToJson();
}

QList<MindMapNode*> MindMapNode::children() const
{
    return m_children;
}

MindMapNode* MindMapNode::parentNode() const
{
    QGraphicsItem* parent = parentItem();
    if (parent && parent->type() == MindMapNode::Type) {
        return static_cast<MindMapNode*>(parent);
    }
    return nullptr;
}

bool MindMapNode::isRoot() const
{
    return parentNode() == nullptr;
}

void MindMapNode::addConnection(Connection* connection)
{
    if (!m_connections.contains(connection)) {
        m_connections.append(connection);
    }

    // 保存状态
    if (!m_loading) saveToJson();
}

void MindMapNode::removeConnection(Connection* connection)
{
    m_connections.removeAll(connection);

    // 保存状态
    if (!m_loading) saveToJson();
}

QList<Connection*> MindMapNode::connections() const
{
    return m_connections;
}

QVariant MindMapNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        // 更新所有连接线
        for (Connection* connection : m_connections) {
            connection->updatePath();
        }
        // 更新子节点位置
        for (MindMapNode* child : m_children) {
            child->update();
        }

        // 保存位置信息
        if (!m_loading) saveToJson();
    }
    return QGraphicsItem::itemChange(change, value);
}

void MindMapNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // 检查是否点击了展开/折叠按钮
    if (event->button() == Qt::LeftButton && !m_children.isEmpty()) {
        QRectF buttonRect = expandButtonRect();
        if (buttonRect.contains(event->pos())) {
            toggleExpanded();
            event->accept();
            return;
        }
    }

    QGraphicsItem::mousePressEvent(event);
}

void MindMapNode::setPosition(const QPointF& pos)
{
    setPos(pos);
    saveToJson(); // 确保位置变化时保存
}

// JSON存储功能实现
void MindMapNode::saveToJson()
{
    if (m_folderPath.isEmpty()) return;

    QFile file(QDir(m_folderPath).filePath("node.json"));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件写入:" << file.fileName();
        return;
    }

    QJsonObject json = toJson();
    file.write(QJsonDocument(json).toJson());
    file.close();
}

void MindMapNode::loadFromJson()
{
    if (m_folderPath.isEmpty()) return;

    QFile file(QDir(m_folderPath).filePath("node.json"));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件读取:" << file.fileName();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "无效的JSON文件:" << file.fileName();
        return;
    }

    fromJson(doc.object());
}

QJsonObject MindMapNode::toJson() const
{
    QJsonObject json;
    json["text"] = m_text;
    json["color"] = m_color.name();
    json["expanded"] = m_expanded;
    json["position_x"] = pos().x();
    json["position_y"] = pos().y();

    // 存储子节点路径（相对路径）
    QJsonArray childrenArray;
    for (MindMapNode* child : m_children) {
        QDir rootDir(m_folderPath);
        childrenArray.append(rootDir.relativeFilePath(child->folderPath()));
    }
    json["children"] = childrenArray;

    // 存储连接信息
    QJsonArray connectionsArray;
    for (Connection* conn : m_connections) {
        if (conn->destinationNode() && conn->destinationNode() != this) {
            QDir rootDir(m_folderPath);
            connectionsArray.append(rootDir.relativeFilePath(conn->destinationNode()->folderPath()));
        }
    }
    json["connections"] = connectionsArray;

    return json;
}

void MindMapNode::fromJson(const QJsonObject& json)
{
    if (json.contains("text")) m_text = json["text"].toString();
    if (json.contains("color")) m_color = QColor(json["color"].toString());
    if (json.contains("expanded")) m_expanded = json["expanded"].toBool();

    if (json.contains("position_x") && json.contains("position_y")) {
        qreal x = json["position_x"].toDouble();
        qreal y = json["position_y"].toDouble();
        setPos(x, y);
    }

    // 注意：子节点和连接将在场景加载时处理
}