#pragma once

#include "wxWidgets.h"
#include <wx/splitter.h>
#include "G3DTreeItemData.h"
#include "ProjectItemMetaBook.h"
#include "ProjectTreeCtrl.h"

class ProjectPanel : public wxSplitterWindow {
public:
	ProjectPanel(wxWindow* parent, const wxSize& size);
	virtual ~ProjectPanel();

	vtkRenderer* GetRenderer() const;
	vtkTransform* GetTransform() const;
	geo3dml::Project* GetG3DProject() const;

	void AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void AppendG3DMap(geo3dml::Map* map);
	void AppendG3DGrid(g3dgrid::Grid* grid);

	void ExpandStructureModelTree();
	void ExpandGridModelNodeTree();

	void CloseAllModels();
	void CloseStructureModels();
	void CloseGridModels();

private:
	void OnChildrenNotify(wxNotifyEvent& event);

private:
	ProjectTreeCtrl* projectTree_;
	ProjectItemMetaBook* projectMetaBook_;

	wxDECLARE_EVENT_TABLE();
};