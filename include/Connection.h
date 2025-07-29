#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsPathItem>
#include "MindMapNode.h"

class Connection : public QGraphicsPathItem
{
public:
    Connection(MindMapNode* source, MindMapNode* destination, QGraphicsItem* parent = nullptr);

    void updatePath();

    MindMapNode* sourceNode() const { return m_source; }
    MindMapNode* destinationNode() const { return m_destination; }

private:
    MindMapNode* m_source;
    MindMapNode* m_destination;
};

#endif // CONNECTION_H
