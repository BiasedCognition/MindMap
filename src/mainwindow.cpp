#include "MainWindow.h"
#include "MindMapScene.h"
#include <QGraphicsView>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QKeyEvent>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QApplication>

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
    setWindowTitle("树状结构思维导图 - 带JSON存储");
    resize(1200, 800);

    // 初始提示
    QMessageBox::information(this, "欢迎", "请新建或打开一个思维导图文件开始");
}

void MainWindow::createActions()
{
    m_newAction = new QAction("新建", this);
    connect(m_newAction, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getExistingDirectory(this, "选择根文件夹");
        if (!path.isEmpty()) {
            if (m_scene->createNewMap(path)) {
                statusBar()->showMessage("已创建新思维导图: " + path, 3000);
            }
        }
    });

    m_openAction = new QAction("打开", this);
    connect(m_openAction, &QAction::triggered, this, [this]() {
        QString path = QFileDialog::getExistingDirectory(this, "选择思维导图根文件夹");
        if (!path.isEmpty()) {
            if (m_scene->openMap(path)) {
                statusBar()->showMessage("已打开思维导图: " + path, 3000);
            }
        }
    });

    m_saveAction = new QAction("保存", this);
    connect(m_saveAction, &QAction::triggered, this, [this]() {
        if (m_scene->saveMap()) {
            statusBar()->showMessage("思维导图已保存", 3000);
        }
    });

    m_deleteAction = new QAction("删除节点", this);
    connect(m_deleteAction, &QAction::triggered, this, [this]() {
        for (QGraphicsItem* item : m_scene->selectedItems()) {
            if (item->type() == MindMapNode::Type) {
                MindMapNode* node = static_cast<MindMapNode*>(item);
                if (!node->isRoot()) {
                    m_scene->removeNode(node);
                    m_scene->updateLayout();
                }
            }
        }
    });

    m_zoomInAction = new QAction("放大", this);
    connect(m_zoomInAction, &QAction::triggered, this, [this]() {
        m_view->scale(1.2, 1.2);
    });

    m_zoomOutAction = new QAction("缩小", this);
    connect(m_zoomOutAction, &QAction::triggered, this, [this]() {
        m_view->scale(0.8, 0.8);
    });

    m_expandAllAction = new QAction("全部展开", this);
    connect(m_expandAllAction, &QAction::triggered, this, [this]() {
        if (m_scene->rootNode()) {
            m_scene->recursiveSetExpanded(m_scene->rootNode(), true);
            m_scene->updateLayout();
        }
    });

    m_collapseAllAction = new QAction("全部折叠", this);
    connect(m_collapseAllAction, &QAction::triggered, this, [this]() {
        if (m_scene->rootNode()) {
            m_scene->recursiveSetExpanded(m_scene->rootNode(), false);
            m_scene->updateLayout();
        }
    });
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar("操作");
    toolBar->addAction(m_newAction);
    toolBar->addAction(m_openAction);
    toolBar->addAction(m_saveAction);
    toolBar->addSeparator();
    toolBar->addAction(m_deleteAction);
    toolBar->addSeparator();
    toolBar->addAction(m_expandAllAction);
    toolBar->addAction(m_collapseAllAction);
    toolBar->addSeparator();
    toolBar->addAction(m_zoomInAction);
    toolBar->addAction(m_zoomOutAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("就绪");
}