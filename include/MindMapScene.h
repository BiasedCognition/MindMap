#ifndef MINDMAPSCENE_H
#define MINDMAPSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include "MindMapNode.h"

class MindMapScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MindMapScene(QObject* parent = nullptr);

    // 文件操作
    bool createNewMap(const QString& path);
    bool openMap(const QString& path);
    bool saveMap();

    // 节点操作
    void removeNode(MindMapNode* node);
    MindMapNode* addChildNode(MindMapNode* parent, const QString& text);

    // 获取根节点
    MindMapNode* rootNode() const { return m_rootNode; }
    QString mapPath() const { return m_mapPath; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void showNodeContextMenu(MindMapNode* node, const QPoint& screenPos);
    void showSceneContextMenu(const QPoint& screenPos, const QPointF& scenePos);

    MindMapNode* m_rootNode; // 根节点
    QString m_mapPath;
};

#endif // MINDMAPSCENE_H