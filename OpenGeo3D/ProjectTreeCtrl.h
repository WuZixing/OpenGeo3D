#pragma once

#include "wxWidgets.h"
#include <wx/treectrl.h>
#include <geo3dml/Project.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

class ProjectTreeCtrl : public wxTreeCtrl {
public:
	ProjectTreeCtrl(wxWindow* parent, const wxSize& size);
	virtual ~ProjectTreeCtrl();

	vtkRenderer* GetRenderer() const { return renderer_; }
	vtkTransform* GetTransform() const { return transform_; }

	void AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void AppendG3DMap(geo3dml::Map* map);

private:
	void OnStateImageClicked(wxTreeEvent& event);
	void OnItemSelected(wxTreeEvent& event);

private:;
	geo3dml::Map* GetDefaultMap();
	wxTreeItemId FindOrInsertMapItem(geo3dml::Map* g3dMap);
	void UpdateSubTreeOfMap(geo3dml::Map* g3dMap);
	void AppendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap);
	void CheckItem(wxTreeItemId item, int checkState);

private:
	const int ItemState_Unchecked_, ItemState_Checked_;
	wxTreeItemId rootStructureModel_, rootOfGridMode_;
	
	std::unique_ptr<geo3dml::Project> g3dProject_;
	vtkSmartPointer<vtkRenderer> renderer_;
	vtkSmartPointer<vtkTransform> transform_;
};
