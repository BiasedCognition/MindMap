#include "MainWindow.h"
#include "MindMapScene.h"
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QKeyEvent>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 创建场景和视图
    m_scene = new MindMapScene(this);
    m_view = new QGraphicsView(m_scene);
    setCentralWidget(m_view);

    // 创建界面元素
    createActions();
    createToolBar();
    createStatusBar();

    // 设置窗口属性
    setWindowTitle("Qt6 思维导图");
    resize(1000, 700);
}

void MainWindow::createActions()
{
    m_addNodeAction = new QAction("添加节点", this);
    connect(m_addNodeAction, &QAction::triggered, this, [this]() {
        m_scene->setMode(MindMapScene::AddNode);
        statusBar()->showMessage("添加节点模式: 在场景中点击添加节点", 3000);
    });

    m_addConnectionAction = new QAction("连接节点", this);
    connect(m_addConnectionAction, &QAction::triggered, this, [this]() {
        m_scene->setMode(MindMapScene::AddConnection);
        statusBar()->showMessage("连接模式: 先点击源节点，再点击目标节点", 3000);
    });

    m_deleteAction = new QAction("删除选中项", this);
    connect(m_deleteAction, &QAction::triggered, this, [this]() {
        for (QGraphicsItem* item : m_scene->selectedItems()) {
            if (item->type() == MindMapNode::Type) {
                m_scene->removeNode(static_cast<MindMapNode*>(item));
            } else {
                m_scene->removeItem(item);
                delete item;
            }
        }
    });

    m_clearAction = new QAction("清空场景", this);
    connect(m_clearAction, &QAction::triggered, m_scene, &QGraphicsScene::clear);
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar("操作");
    toolBar->addAction(m_addNodeAction);
    toolBar->addAction(m_addConnectionAction);
    toolBar->addAction(m_deleteAction);
    toolBar->addAction(m_clearAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("就绪");
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete) {
        for (QGraphicsItem* item : m_scene->selectedItems()) {
            if (item->type() == MindMapNode::Type) {
                m_scene->removeNode(static_cast<MindMapNode*>(item));
            } else {
                m_scene->removeItem(item);
                delete item;
            }
        }
    }
    QMainWindow::keyPressEvent(event);
}
