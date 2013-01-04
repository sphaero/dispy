#include "gldispywidget.h"
#include <QPainter>
#include <QRect>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

glDispyWidget::glDispyWidget(QWidget *parent) :
    QGLWidget(parent)
{
    //initialise X display
    xDisplay = XOpenDisplay("localhost:0.0");
    screenNumber = DefaultScreen(xDisplay);
    rootWindow = RootWindow(xDisplay, screenNumber);

    xDisplayWidth = XDisplayWidth(xDisplay, screenNumber);
    xDisplayHeight = XDisplayHeight(xDisplay, screenNumber);
    useShm = XShmQueryExtension(xDisplay);

    if (useShm) {
        img = XShmCreateImage(xDisplay, DefaultVisual(xDisplay, screenNumber),
                                  DefaultDepth(xDisplay, screenNumber),
                                  ZPixmap, NULL, &shminfo,
                                  xDisplayWidth, xDisplayHeight );
        if(img == NULL) {
            qDebug() << "can't get shm image";
            return;
        }
        shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line*img->height,
                                 IPC_CREAT | 0777 );
        shminfo.shmaddr = img->data = (char *)shmat(shminfo.shmid, 0,0);
        shminfo.readOnly = false;
        XShmAttach(xDisplay, &shminfo);
    }

    //make this widget current GL context
    makeCurrent();
    //render_fbo = new QGLFramebufferObject(512, 512);
    //render_fbo->bind();
    initializeGL();
    //glColor3f(1,1,0);
    //glBegin(GL_POLYGON);
    //glVertex2f(0,0);
    //glVertex2f(100,500);
    //glVertex2f(500,100);
    //glEnd();
    //render_fbo->release();
    timer = new QTimer();
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(redraw()));
    timer->start(100);
}

glDispyWidget::~glDispyWidget() {
    //detach from shared mem
    XShmDetach(xDisplay, &shminfo);
    shmdt(shminfo.shmaddr);
    shmctl(shminfo.shmid, IPC_RMID, NULL);
    XDestroyImage(img);
    XCloseDisplay(xDisplay);
    //delete render_fbo;
}

void glDispyWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
    //prepare opengl texture
    glGenTextures(1, &texture);
}

void glDispyWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, w, h, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void glDispyWidget::redraw() {

    this->makeCurrent();
    //qDebug() << "bnoe";
    paintGL();
    updateGL();
}

void glDispyWidget::paintGL() {


    //http://stackoverflow.com/questions/6512543/obtaining-full-desktop-screenshot-from-the-gpu
    //http://forum.openframeworks.cc/index.php?topic=3017.0
    if(useShm) {
        XShmGetImage(xDisplay, rootWindow, img, 0, 0, AllPlanes);
    }
    else {
        //XImage *img = XGetImage(display,root,0,0,400,400,XAllPlanes(),ZPixmap);
        img = XGetImage(xDisplay,rootWindow,0,0,xDisplayWidth,xDisplayHeight,XAllPlanes(),ZPixmap);
    }
    if (img == NULL) {
       qDebug() << "error getting display data";
       return;
    }
    qDebug() << img->width;
    /*glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    glVertex2f(0,0);
    glVertex2f(100,500);
    glVertex2f(500,100);
    glEnd();
    */
    //see http://www.opengl.org/wiki/Programming_OpenGL_in_Linux:_Creating_a_texture_from_a_Pixmap
    //generate a texture from the Ximage
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, xDisplayWidth,xDisplayHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)(&(img->data[0])));
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_CULL_FACE);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(0.f, 0.f, 0.f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(width(), 0.f, 0.f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(width(), height()-2, 0);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(0.f, height(), 0.f);
    glEnd();
    //render_fbo->toImage().save("test.png");
}
