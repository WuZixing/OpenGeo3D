#include "ProjectTreeCtrl.h"
#include <g3dvtk/ObjectFactory.h>
#include "Text.h"

ProjectTreeCtrl::ProjectTreeCtrl(QWidget* parent) : QTreeWidget(parent) {
	setColumnCount(1);
	setHeaderHidden(true);

	rootOfStructureModel_ = new QTreeWidgetItem(this);
	//rootOfStructureModel_->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	rootOfStructureModel_->setText(0, Text::nameOfStructureModel());
	rootOfStructureModel_->setCheckState(0, Qt::CheckState::Checked);
	addTopLevelItem(rootOfStructureModel_);
	QTreeWidgetItem* child = new QTreeWidgetItem(rootOfStructureModel_);
	//child->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	child->setText(0, "hahahah2");
	child->setCheckState(0, Qt::CheckState::Checked);

	g3dvtk::ObjectFactory g3dFactory;
	g3dProject_.reset(g3dFactory.NewProject());
	g3dProject_->SetID(geo3dml::Object::NewID());
	g3dProject_->SetName(Text::nameOfStructureModel().toUtf8().constData());

	renderer_ = vtkSmartPointer<vtkRenderer>::New();
	transform_ = vtkSmartPointer<vtkTransform>::New();
	transform_->Identity();
}

ProjectTreeCtrl::~ProjectTreeCtrl() {

}
