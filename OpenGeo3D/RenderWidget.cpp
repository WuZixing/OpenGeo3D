#include "RenderWidget.h"
#include <vtkAxesActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkRenderer.h>

RenderWidget::RenderWidget(QWidget* parent) : QVTKOpenGLNativeWidget(parent) {
	vtkNew<vtkGenericOpenGLRenderWindow> window;
	setRenderWindow(window);
	vtkRenderWindowInteractor* interactor = window->GetInteractor();
	if (interactor != nullptr) {
		vtkInteractorStyleSwitch* styleSwith = vtkInteractorStyleSwitch::SafeDownCast(interactor->GetInteractorStyle());
		if (styleSwith != nullptr) {
			styleSwith->SetCurrentStyleToTrackballCamera();
		}
	}

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	window->AddRenderer(renderer_);

	axesWidget_ = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkNew<vtkAxesActor> axes;
	axesWidget_->SetOrientationMarker(axes);
	axesWidget_->SetInteractor(interactor);
	axesWidget_->EnabledOn();
	axesWidget_->InteractiveOff();
}

RenderWidget::~RenderWidget() {

}
