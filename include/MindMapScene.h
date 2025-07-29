#ifndef MINDMAPSCENE_H
#define MINDMAPSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include "MindMapNode.h"

class MindMapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum Mode { None, AddNode, AddConnection };

    explicit MindMapScene(QObject* parent = nullptr);

    void setMode(Mode mode) { m_mode = mode; }

    void removeNode(MindMapNode* node);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void showNodeContextMenu(MindMapNode* node, const QPoint& screenPos);
    void showSceneContextMenu(const QPoint& screenPos, const QPointF& scenePos);


    Mode m_mode;
    MindMapNode* m_tempNode;
};

#endif // MINDMAPSCENE_H
