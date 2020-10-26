#pragma once

#include "wxWidgets.h"
#include <wx/aui/auibook.h>
#include <wx/splitter.h>
#include <geo3dml/Project.h>
#include "ProjectTreeCtrl.h"

class ProjectPanel : public wxSplitterWindow {
public:
	ProjectPanel(wxWindow* parent, const wxSize& size);
	virtual ~ProjectPanel();

	vtkRenderer* GetRenderer() const;
	vtkTransform* GetTransform() const;

	void AppendG3DModel(geo3dml::Model* model, bool appendToDefaultMap);
	void AppendG3DMap(geo3dml::Map* map);

private:
	geo3dml::Map* GetDefaultMap();

	void AppendFeatureClassToMap(geo3dml::FeatureClass* g3dFC, geo3dml::Map* g3dMap);

private:
	ProjectTreeCtrl* projectTree_;
	wxAuiNotebook* dataPropNotebook_;

	geo3dml::Project* g3dProject_;


};