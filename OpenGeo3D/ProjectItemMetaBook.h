#pragma once

#include "wxWidgets.h"
#include <wx/aui/auibook.h>
#include "MetadataPage.h"
#include "RenderOptionPage.h"

class ProjectItemMetaBook : public wxAuiNotebook {
public:
	ProjectItemMetaBook(wxWindow* parent);
	virtual ~ProjectItemMetaBook();

	void BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType);

private:
	MetadataPage* metadataPage_;
	RenderOptionPage* renderOptionPage_;
};
