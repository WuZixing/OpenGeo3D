#pragma once

#include <QVTKOpenGLNativeWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>

class RenderWidget : public QVTKOpenGLNativeWidget {
	Q_OBJECT

public:
	RenderWidget(QWidget* parent, vtkRenderer* renderer);
	virtual ~RenderWidget();

	void render();
	void resetAndRender();

private:
	vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget_;
};
