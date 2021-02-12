#include "ProjectPanel.h"
#include <QtWidgets/QTextEdit>

ProjectPanel::ProjectPanel(QWidget* parent) : QSplitter(Qt::Orientation::Vertical, parent) {
	projectTreeCtrl_ = new ProjectTreeCtrl(this);
	projectMetaBook_ = new ProjectMetaBook(this);

	setStretchFactor(0, 1);
	setStretchFactor(1, 1);

	connect(projectTreeCtrl_, &ProjectTreeCtrl::itemSelectionChanged, this, &ProjectPanel::onTreeItemSelectionChanged);
}

ProjectPanel::~ProjectPanel() {

}

vtkRenderer* ProjectPanel::getRenderer() const {
	return projectTreeCtrl_->getRenderer();
}

vtkTransform* ProjectPanel::getTransform() const {
	return projectTreeCtrl_->getTransform();
}

void ProjectPanel::appendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {
	projectTreeCtrl_->appendG3DModel(model, appendToDefaultMap);
}

void ProjectPanel::appendG3DMap(geo3dml::Map* map) {
	projectTreeCtrl_->appendG3DMap(map);
}

void ProjectPanel::updateVoxelGridModel() {
	projectTreeCtrl_->updateVoxelGridModel();
}

void ProjectPanel::expandStructureModelTree() {
	projectTreeCtrl_->expandStructureModelTree();
}

void ProjectPanel::expandGridModelNodeTree() {
	projectTreeCtrl_->expandGridModelNodeTree();
}

void ProjectPanel::closeAllModels() {
	projectTreeCtrl_->closeAllModels();
}

void ProjectPanel::closeStructureModel() {
	projectTreeCtrl_->closeStructureModel();
}

void ProjectPanel::closeVoxelGridModel() {
	projectTreeCtrl_->closeVoxelGridModel();
}

void ProjectPanel::onTreeItemSelectionChanged() {
	QTreeWidgetItem* item = projectTreeCtrl_->currentItem();
	void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
	geo3dml::Object* g3dObj = static_cast<geo3dml::Object*>(ptr);
}
