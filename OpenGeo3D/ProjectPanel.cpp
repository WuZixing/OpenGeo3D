#include "ProjectPanel.h"
#include "ProjectMetaBook.h"
#include "ProjectTreeCtrl.h"
#include <QtWidgets/QTextEdit>

ProjectPanel::ProjectPanel(QWidget* parent) : QSplitter(Qt::Orientation::Vertical, parent) {
	ProjectTreeCtrl* projectTree_ = new ProjectTreeCtrl(this);
	ProjectMetaBook* projectMetaBook_ = new ProjectMetaBook(this);

	this->setStretchFactor(0, 1);
	this->setStretchFactor(1, 1);
}

ProjectPanel::~ProjectPanel() {

}

void ProjectPanel::AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap) {

}

void ProjectPanel::AppendG3DMap(geo3dml::Map* map) {

}

void ProjectPanel::UpdateVoxelGridModel() {

}

void ProjectPanel::ExpandStructureModelTree() {

}

void ProjectPanel::ExpandGridModelNodeTree() {

}
