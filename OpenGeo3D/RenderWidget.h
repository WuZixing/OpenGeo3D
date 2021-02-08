#pragma once

#include <QVTKOpenGLNativeWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderer.h>

class RenderWidget : public QVTKOpenGLNativeWidget {
	Q_OBJECT

public:
	RenderWidget(QWidget* parent = nullptr);
	virtual ~RenderWidget();

private:
	vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget_;
	vtkSmartPointer<vtkRenderer> renderer_;
};
