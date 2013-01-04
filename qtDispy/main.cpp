#include <QtGui/QApplication>
#include "gldispywidget.h"

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(framebufferobject);

    QApplication a(argc, argv);

    glDispyWidget w;
    w.resize(400,400);
    w.show();
    
    int ret = a.exec();
    qDebug() << ret;
    return ret;
}
