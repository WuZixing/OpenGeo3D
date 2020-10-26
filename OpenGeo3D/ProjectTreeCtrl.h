#pragma once

#include "wxWidgets.h"
#include <wx/treectrl.h>
#include <geo3dml/Map.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

class ProjectTreeCtrl : public wxTreeCtrl {
public:
	ProjectTreeCtrl(wxWindow* parent, const wxSize& size);
	virtual ~ProjectTreeCtrl();

	vtkRenderer* GetRenderer() const { return renderer_; }
	vtkTransform* GetTransform() const { return transform_; }

	void ProjectTreeCtrl::UpdateSubTreeOfMap(geo3dml::Map* g3dMap);

private:
	void OnStateImageClicked(wxTreeEvent& event);

private:;
	wxTreeItemId FindOrInsertMapItem(geo3dml::Map* g3dMap);
	void CheckItem(wxTreeItemId item, int checkState);

private:
	const int ItemState_Unchecked_, ItemState_Checked_;
	wxTreeItemId rootStructureModel_, rootOfGridMode_;
	
	vtkSmartPointer<vtkRenderer> renderer_;
	vtkSmartPointer<vtkTransform> transform_;
};
