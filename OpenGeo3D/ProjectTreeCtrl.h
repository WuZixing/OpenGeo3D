#pragma once

#include <QtWidgets/QTreeWidget>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <geo3dml/Project.h>

class ProjectTreeCtrl : public QTreeWidget {
	Q_OBJECT

public:
	ProjectTreeCtrl(QWidget* parent);
	virtual ~ProjectTreeCtrl();

private:
	std::unique_ptr<geo3dml::Project> g3dProject_;
	vtkSmartPointer<vtkRenderer> renderer_;
	vtkSmartPointer<vtkTransform> transform_;

	QTreeWidgetItem* rootOfStructureModel_;
};
