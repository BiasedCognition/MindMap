#include "MainWindow.h"
#include "MindMapScene.h"
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QKeyEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <string>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // 创建场景和视图
    m_scene = new MindMapScene(this);
    m_view = new QGraphicsView(m_scene);
    m_scene->genSceneFromFolder("C:/msys64");
    setCentralWidget(m_view);

    // 创建界面元素
    createActions();
    createToolBar();
    createStatusBar();

    // 设置窗口属性
    setWindowTitle("Qt6 思维导图");
    resize(1000, 700);
}

MainWindow::~MainWindow() {
}

void MainWindow::createActions()
{
    m_addNodeAction = new QAction("添加模式", this);
    connect(m_addNodeAction, &QAction::triggered, this, [this]() {
        m_scene->setMode(MindMapScene::AddNode);
        statusBar()->showMessage("添加节点模式: 在场景中点击添加节点", 0);
    });

    m_addConnectionAction = new QAction("连接模式", this);
    connect(m_addConnectionAction, &QAction::triggered, this, [this]() {
        m_scene->setMode(MindMapScene::AddConnection);
        statusBar()->showMessage("连接节点模式: 在源节点按下鼠标，到目标节点松开", 0);
    });

    m_dragAction = new QAction("拖动模式", this);
    connect(m_dragAction, &QAction::triggered, this, [this](){
        m_scene->setMode(MindMapScene::None);
        statusBar()->showMessage("拖动模式", 0);
    });


    m_clearAction = new QAction("清空场景", this);
    connect(m_clearAction, &QAction::triggered, m_scene, &QGraphicsScene::clear);
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar("操作");
    toolBar->addAction(m_dragAction);
    toolBar->addAction(m_addNodeAction);
    toolBar->addAction(m_addConnectionAction);
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
