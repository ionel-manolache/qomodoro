#include "systrayicon.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SysTrayIcon icon;
    icon.show();

    return app.exec();
}
