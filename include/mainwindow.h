#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MindMapScene.h"

class QGraphicsView;
class QToolBar;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void createActions();
    void createToolBar();
    void createStatusBar();

    MindMapScene* m_scene;
    QGraphicsView* m_view;

    QAction* m_addNodeAction;
    QAction* m_addConnectionAction;
    QAction* m_dragAction;
    QAction* m_clearAction;
};

#endif // MAINWINDOW_H
