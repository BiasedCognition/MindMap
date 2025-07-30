#ifndef MINDMAPNODE_H
#define MINDMAPNODE_H

#include <QGraphicsItem>
#include <QColor>
#include <QList>
#include <QStyleOption>
#include <QDir>

class Connection;

class MindMapNode : public QGraphicsItem
{
public:
    explicit MindMapNode(const QString& text, const QString& path, QGraphicsItem* parent = nullptr);

    // 重写图形项接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    // 节点操作
    void setText(const QString& text);
    QString text() const;
    void setColor(const QColor& color);
    QColor color() const;

    // 文件夹路径操作
    void setFolderPath(const QString& path);
    QString folderPath() const;
    QDir directory() const;

    // 子节点管理
    void addChild(MindMapNode* child);
    void removeChild(MindMapNode* child);
    QList<MindMapNode*> children() const;

    // 连接管理
    void addConnection(Connection* connection);
    void removeConnection(Connection* connection);
    QList<Connection*> connections() const;

    // 节点关系
    MindMapNode* parentNode() const;
    bool isRoot() const;

    // 类型标识
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QString m_text;
    QColor m_color;
    QString m_folderPath;
    QList<MindMapNode*> m_children;
    QList<Connection*> m_connections;
};

#endif // MINDMAPNODE_H