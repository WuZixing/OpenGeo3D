#pragma once

#include "wxWidgets.h"
#include <wx/treebase.h>
#include <geo3dml/Geo3DML.h>

class G3DTreeItemData : public wxTreeItemData {
public:
	enum ItemType {
		Unknown,
		G3D_StructureModel,
		G3D_GridModel,
		G3D_Map,
		G3D_Layer,
		G3D_Actor
	};

public:
	G3DTreeItemData(geo3dml::Object* g3dObject, ItemType t);
	virtual ~G3DTreeItemData();

	ItemType GetItemType() const { return itemType_; }
	geo3dml::Object* GetG3DObject() const { return g3dObject_; };

private:
	ItemType itemType_;
	geo3dml::Object* g3dObject_;
};
