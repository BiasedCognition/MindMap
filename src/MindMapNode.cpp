#include "MindMapNode.h"
#include "Connection.h"
#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QStyle>

MindMapNode::MindMapNode(const QString& text, QGraphicsItem* parent)
    : QGraphicsItem(parent), m_text(text), m_color(QColor(135, 206, 250))
{
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
}

QRectF MindMapNode::boundingRect() const
{
    QFontMetrics fm(QApplication::font());
    int width = fm.horizontalAdvance(m_text) + 30;
    int height = fm.height() + 20;
    return QRectF(-width/2, -height/2, width, height);
}

void MindMapNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);

    QRectF rect = boundingRect();
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, m_color.lighter(120));
    gradient.setColorAt(1, m_color);

    painter->setBrush(gradient);
    painter->setPen(QPen(isSelected() ? Qt::blue : Qt::darkGray, isSelected() ? 2 : 1));
    painter->drawRoundedRect(rect, 10, 10);

    painter->setPen(Qt::black);
    painter->drawText(rect, Qt::AlignCenter, m_text);

    if (option->state & QStyle::State_MouseOver) {
        painter->setBrush(QColor(255, 255, 255, 100));
        painter->drawRoundedRect(rect, 10, 10);
    }
}

void MindMapNode::setText(const QString& text)
{
    m_text = text;
    update();
}

QString MindMapNode::text() const
{
    return m_text;
}

void MindMapNode::setColor(const QColor& color)
{
    m_color = color;
    update();
}

QColor MindMapNode::color() const
{
    return m_color;
}

void MindMapNode::addChild(MindMapNode* child)
{
    if (!m_children.contains(child)) {
        m_children.append(child);
    }
}

void MindMapNode::removeChild(MindMapNode* child)
{
    m_children.removeAll(child);
}

QList<MindMapNode*> MindMapNode::children() const
{
    return m_children;
}

void MindMapNode::addConnection(Connection* connection)
{
    if (!m_connections.contains(connection)) {
        m_connections.append(connection);
    }
}

void MindMapNode::removeConnection(Connection* connection)
{
    m_connections.removeAll(connection);
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
    }
    return QGraphicsItem::itemChange(change, value);
}
