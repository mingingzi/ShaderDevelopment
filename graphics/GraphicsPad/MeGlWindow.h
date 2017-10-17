#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW
#include <QtOpenGL\qglwidget>
#include <Qt\qtimer.h>

class MeGlWindow : public QGLWidget
{
protected:
	void initializeGL();
	void paintGL();
	void mouseMoveEvent(QMouseEvent*);
	void keyPressEvent(QKeyEvent*);

	QTimer	*Mytimer;
public:
};

#endif
