#include "Connection.h"
#include <QPainterPath>
#include <qpen.h>

Connection::Connection(MindMapNode* source, MindMapNode* destination, QGraphicsItem* parent)
    : QGraphicsPathItem(parent), m_source(source), m_destination(destination)
{
    setPen(QPen(Qt::darkGray, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1); // 确保在节点下方
    updatePath();

    if (m_source) m_source->addConnection(this);
    if (m_destination) m_destination->addConnection(this);
}

Connection::~Connection()
{
    m_source->removeConnection(this);
    m_destination->removeConnection(this);
}

void Connection::updatePath()
{
    if (!m_source || !m_destination) return;

    QPointF start = m_source->pos();
    QPointF end = m_destination->pos();

    // 创建曲线路径
    QPainterPath path;
    path.moveTo(start);

    // 计算控制点，创建曲线效果
    qreal dx = end.x() - start.x();
    qreal dy = end.y() - start.y();

    QPointF ctrl1(start.x() + dx * 0.5, start.y());
    QPointF ctrl2(start.x() + dx * 0.5, end.y());

    path.cubicTo(ctrl1, ctrl2, end);

    setPath(path);
}
