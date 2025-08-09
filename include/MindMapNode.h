#ifndef MINDMAPNODE_H
#define MINDMAPNODE_H

#include <QGraphicsItem>
#include <QColor>
#include <QList>
#include <QStyleOption>
#include <QDir>
#include <QPainterPath>
#include <QJsonObject>

class Connection;

class MindMapNode : public QGraphicsItem
{
public:
    explicit MindMapNode(const QString& text, const QString& path, QGraphicsItem* parent = nullptr);
    ~MindMapNode();

    // 重写图形项接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    // JSON存储
    QJsonObject getJson() const;
    void applyJson(const QJsonObject& json);
    void saveToJson();
    void loadFromJson();
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
    void changeJson(const QString& header, const QString& info, const QString& action);

    // 节点操作
    void setText(const QString& text);
    QString text() const;
    void setColor(const QColor& color);
    QColor color() const;
    void addTag(const QString& tag);

    // 文件夹路径操作
    void setFolderPath(const QString& path);
    QString folderPath() const;
    QDir directory() const;

    // 子节点管理
    void addChild(MindMapNode* child);
    void removeChild(const QString& name);
    void removeAllChild();
    void loadChild(const QString& name);
    void loadAllChild();
    void hideChild(MindMapNode* child);
    void hideAllChild();
    QList<MindMapNode*> children() const;

    // 连接管理
    void addConnection(Connection* connection);
    void removeConnection(Connection* connection);
    QList<Connection*> connections() const;

    // 节点关系
    MindMapNode* parentNode() const;
    bool isRoot() const;

    // 折叠/展开功能
    bool isExpanded() const { return m_expanded; }
    void setExpanded(bool expanded);
    void toggleExpanded();
    QRectF expandButtonRect() const;



    // 位置信息
    void setPosition(const QPointF& pos);
    QPointF position() const { return pos(); }

    // 类型标识
    enum { Type = UserType + 1 };
    int type() const override { return Type; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    QString m_text;
    QColor m_color;
    QString m_folderPath;
    QList<MindMapNode*> m_children;
    QList<Connection*> m_connections;
    QList<QString> m_tags;
    bool m_expanded; // 是否展开子节点
    bool m_loading;  // 是否正在加载中（防止保存循环）
};

#endif // MINDMAPNODE_H