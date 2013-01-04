#ifndef DISPYMAIN_H
#define DISPYMAIN_H

#include <QtGui/QMainWindow>
#include <X11/Xlib.h>

class DispyMain : public QMainWindow
{
    Q_OBJECT
    
public:
    DispyMain(QWidget *parent = 0);
    ~DispyMain();
};

#endif // DISPYMAIN_H
