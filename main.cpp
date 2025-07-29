#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 设置应用样式
    QApplication::setStyle("Fusion");

    // 创建主窗口
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
