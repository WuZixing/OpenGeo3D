#pragma once

#include <QtWidgets/QSplitter>
#include "ProjectMetaBook.h"
#include "ProjectTreeCtrl.h"

class ProjectPanel : public QSplitter {
	Q_OBJECT

public:
	ProjectPanel(QWidget* parent);
	virtual ~ProjectPanel();

	vtkRenderer* getRenderer() const;
	vtkTransform* getTransform() const;

	void appendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void appendG3DMap(geo3dml::Map* map);
	void updateVoxelGridModel();

	void expandStructureModelTree();
	void expandGridModelNodeTree();

	void closeAllModels();
	void closeStructureModel();
	void closeVoxelGridModel();

private:
	void onTreeItemSelectionChanged();

private:
	ProjectTreeCtrl* projectTreeCtrl_;
	ProjectMetaBook* projectMetaBook_;
};
