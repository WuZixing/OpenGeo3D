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

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
	void onItemChanged(QTreeWidgetItem* item, int column);

private:
	geo3dml::Map* getDefaultMap();

	void updateSubTreeOfMap(geo3dml::Map* g3dMap);
	void appendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap);
	QTreeWidgetItem* findOrInsertMapItem(geo3dml::Map* g3dMap);

	void removeChildrenFromScene(QTreeWidgetItem* item);
	void resetStructureModel();
	void resetVoxelGridModel();

	void checkChildrenState(QTreeWidgetItem* item, Qt::CheckState state);

	void contextMenuOfStructureModel(const QPoint& position);

private:
	std::unique_ptr<geo3dml::Project> g3dProject_;
	vtkSmartPointer<vtkRenderer> renderer_;
	vtkSmartPointer<vtkTransform> transform_;

	QTreeWidgetItem* rootOfStructureModel_;
};
