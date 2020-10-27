#pragma once

#include "wxWidgets.h"
#include <wx/propgrid/propgrid.h>
#include <geo3dml/Project.h>
#include "G3DTreeItemData.h"

class RenderOptionPage : public wxPropertyGrid {
public:
	RenderOptionPage(wxWindow* parent);
	virtual ~RenderOptionPage();

	void BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType);

private:
	void BindToStructureModels(geo3dml::Project* g3dProject);
	void BindToG3DMap(geo3dml::Map* g3dMap);
	void BindToG3DLayer(geo3dml::Layer* g3dLayer);
	void BindToG3DActor(geo3dml::Actor* g3dActor);
};
