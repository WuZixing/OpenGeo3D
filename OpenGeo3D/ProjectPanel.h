#pragma once

#include <QtWidgets/QSplitter>
#include <geo3dml/Project.h>

class ProjectPanel : public QSplitter {
	Q_OBJECT

public:
	ProjectPanel(QWidget* parent);
	virtual ~ProjectPanel();

public:
	void AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void AppendG3DMap(geo3dml::Map* map);
	void UpdateVoxelGridModel();

	void ExpandStructureModelTree();
	void ExpandGridModelNodeTree();

};
