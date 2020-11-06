#pragma once

#include "wxWidgets.h"
#include <wx/treectrl.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <geo3dml/Project.h>
#include <g3dgrid/GridCollection.h>
#include "G3DTreeItemData.h"

class ProjectTreeCtrl : public wxTreeCtrl {
public:
	ProjectTreeCtrl(wxWindow* parent, const wxSize& size);
	virtual ~ProjectTreeCtrl();

	vtkRenderer* GetRenderer() const { return renderer_; }
	vtkTransform* GetTransform() const { return transform_; }

	void AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void AppendG3DMap(geo3dml::Map* map);
	void AppendG3DGrid(g3dgrid::Grid* grid);

	void ExpandStructureModelTree();
	void ExpandGridModelNodeTree();

private:
	void OnStateImageClicked(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);
	void OnItemMenu(wxTreeEvent& event);
	void OnEditVoxelGrid(wxCommandEvent& event);

private:;
	geo3dml::Map* GetDefaultMap();
	wxTreeItemId FindOrInsertMapItem(geo3dml::Map* g3dMap);
	wxTreeItemId FindOrInsertVoxelGridItem(g3dgrid::VoxelGrid* g3dVoxelGrid);
	void UpdateSubTreeOfMap(geo3dml::Map* g3dMap);
	void AppendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap);
	void UpdateSubTreeOfGrid(g3dgrid::Grid* g3dGrid);
	void CheckItem(wxTreeItemId item, int checkState);

	void ShowMenuOnStructureModelItem(G3DTreeItemData* itemData, const wxPoint& pos);
	void ShowMenuOnGridModelItem(G3DTreeItemData* itemData, const wxPoint& pos);
	void ShowMenuOnActorItem(G3DTreeItemData* itemData, const wxPoint& pos);
	void ShowMenuOnVoxelGridItem(G3DTreeItemData* itemData, const wxPoint& pos);

private:
	const int ItemState_Unchecked_, ItemState_Checked_;
	wxTreeItemId rootStructureModel_, rootOfGridModel_;
	
	std::unique_ptr<geo3dml::Project> g3dProject_;
	std::unique_ptr<g3dgrid::GridCollection> g3dGrids_;
	vtkSmartPointer<vtkRenderer> renderer_;
	vtkSmartPointer<vtkTransform> transform_;
};
