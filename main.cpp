#include "application.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Application icon;
    icon.show();

    app.setQuitOnLastWindowClosed(false);
    return app.exec();
}
