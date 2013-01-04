#ifndef GLDISPYWIDGET_H
#define GLDISPYWIDGET_H

#include <QtOpenGL>
#include <QGLWidget>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

class glDispyWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit glDispyWidget(QWidget *parent = 0);
    ~glDispyWidget();

    //void paintEvent(QPaintEvent *);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    QTimer* timer;
    QTime*  timeValue;
signals:
    
public slots:
    void redraw();

private:
    QGLFramebufferObject* render_fbo;
    Display *xDisplay;
    int     screenNumber;
    Window  rootWindow;
    int xDisplayWidth;
    int xDisplayHeight;
    bool useShm;
    XShmSegmentInfo shminfo;
    XImage* img;
    GLuint texture;
};

#endif // GLDISPYWIDGET_H
