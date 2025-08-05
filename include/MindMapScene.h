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

    // 布局功能
    void updateLayout();

    // 折叠/展开功能
    void recursiveSave(MindMapNode* node);
    void recursiveSetExpanded(MindMapNode* node, bool expanded);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:


    void showNodeContextMenu(MindMapNode* node, const QPoint& screenPos);
    void showSceneContextMenu(const QPoint& screenPos, const QPointF& scenePos);

    void recursiveLayout(MindMapNode* node, qreal x, qreal& y, int depth);
    void recursiveLoadChildren(MindMapNode* parent);
    void restoreConnections(MindMapNode* node);

    MindMapNode* m_rootNode; // 根节点
    QString m_mapPath;
};

#endif // MINDMAPSCENE_H