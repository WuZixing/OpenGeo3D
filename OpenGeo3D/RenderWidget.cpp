#include "RenderWidget.h"
#include <vtkAxesActor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleSwitch.h>
#include <vtkRendererCollection.h>

RenderWidget::RenderWidget(QWidget* parent, vtkRenderer* renderer) : QVTKOpenGLNativeWidget(parent) {
	vtkNew<vtkGenericOpenGLRenderWindow> window;
	setRenderWindow(window);
	vtkRenderWindowInteractor* interactor = window->GetInteractor();
	if (interactor != nullptr) {
		vtkInteractorStyleSwitch* styleSwith = vtkInteractorStyleSwitch::SafeDownCast(interactor->GetInteractorStyle());
		if (styleSwith != nullptr) {
			styleSwith->SetCurrentStyleToTrackballCamera();
		}
	}

	window->AddRenderer(renderer);

	axesWidget_ = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	vtkNew<vtkAxesActor> axes;
	axesWidget_->SetOrientationMarker(axes);
	axesWidget_->SetInteractor(interactor);
	axesWidget_->EnabledOn();
	axesWidget_->InteractiveOff();
}

RenderWidget::~RenderWidget() {

}

void RenderWidget::render() {
	renderWindow()->Render();
}

void RenderWidget::resetAndRender() {
	vtkRendererCollection* renderers = renderWindow()->GetRenderers();
	renderers->GetFirstRenderer()->ResetCamera();
	renderWindow()->Render();
}
