#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MindMapScene.h"

class QGraphicsView;
class QToolBar;
class QAction;
class QStatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void createActions();
    void createToolBar();
    void createStatusBar();

    MindMapScene* m_scene;
    QGraphicsView* m_view;

    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_deleteAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_expandAllAction;
    QAction* m_collapseAllAction;
};

#endif // MAINWINDOW_H