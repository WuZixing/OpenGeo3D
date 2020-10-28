#pragma once

#include "wxWidgets.h"
#include <wx/propgrid/propgrid.h>
#include <geo3dml/Project.h>
#include <g3dvtk/Actor.h>
#include "G3DTreeItemData.h"

class RenderOptionPage : public wxPropertyGrid {
public:
	RenderOptionPage(wxWindow* parent);
	virtual ~RenderOptionPage();

	void BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType);

private:
	void OnRenderOptionChanged(wxPropertyGridEvent& event);

private:
	void BindToStructureModels(geo3dml::Project* g3dProject);
	void BindToG3DMap(geo3dml::Map* g3dMap);
	void BindToG3DLayer(geo3dml::Layer* g3dLayer);
	void BindToG3DActor(geo3dml::Actor* g3dActor);

private:
	g3dvtk::Actor* g3dActor_;

	const wxString EntryName_ObjectColor_ = wxS("object_color");
	const wxString EntryName_ObjectTransparency_ = wxS("object_transparency");
	const wxString EntryName_ObjectRepresentation_ = wxS("object_representation");
	const wxString EntryName_ObjectDrawEdge_ = wxS("object_draw_edge");
};
