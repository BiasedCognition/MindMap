#ifndef MINDMAPNODE_H
#define MINDMAPNODE_H

#include <QGraphicsItem>
#include <QColor>
#include <QList>

class Connection;

class MindMapNode : public QGraphicsItem
{
public:
    explicit MindMapNode(const QString& text, QGraphicsItem* parent = nullptr);

    // 重写图形项接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    // 节点操作
    void setText(const QString& text);
    QString getText() const;

    void setColor(const QColor& color);
    QColor getColor() const;


    // ? Modification Explain
    // 由于思维导图不总是树，使用父-子关系管理并不恰当。
    

    // TODO 由于思维导图并不总为简单图，此处也应修改。
    // 连接管理
    void addConnection(Connection* connection);
    void removeConnection(Connection* connection);
    QList<Connection*> connections() const;

    // 类型标识
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    QString m_text;
    QColor m_color;
    QList<MindMapNode*> m_concatedNodes;
    QList<Connection*> m_connections;
};

#endif // MINDMAPNODE_H
