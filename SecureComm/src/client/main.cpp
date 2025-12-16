#include <QApplication>
#include <QFile>
#include <QDebug>
#include "../ui/ChatWindow.h"

using namespace std;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("安全通信工具");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SecureComm");

    qDebug() << "========================================";
    qDebug() << "安全通信工具启动";
    qDebug() << "Qt版本:" << QT_VERSION_STR;
    qDebug() << "========================================";

    QFile styleFile(":/styles.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        qDebug() << "样式表加载成功";
    }
    else {
        qWarning() << "无法加载样式表文件";
    }

    ChatWindow mainWindow;
    mainWindow.setWindowTitle("安全通信工具 v1.0.0 - 加密聊天测试");
    mainWindow.resize(1000, 700);
    mainWindow.show();

    qDebug() << "主窗口已显示，进入事件循环...";

    return app.exec();
}