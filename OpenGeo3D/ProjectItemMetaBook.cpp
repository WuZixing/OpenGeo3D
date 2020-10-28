#include "ProjectItemMetaBook.h"
#include "Strings.h"

ProjectItemMetaBook::ProjectItemMetaBook(wxWindow* parent) : wxAuiNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_BOTTOM | wxBORDER_NONE) {
	metadataPage_ = new MetadataPage(this);
	renderOptionPage_ = new RenderOptionPage(this);
	AddPage(metadataPage_, Strings::TitleOfMetadata(), true);
	AddPage(renderOptionPage_, Strings::TitleOfRenderOption(), false);
}

ProjectItemMetaBook::~ProjectItemMetaBook() {

}

void ProjectItemMetaBook::BindToItem(geo3dml::Object* g3dObject, G3DTreeItemData::ItemType itemType) {
	metadataPage_->BindToItem(g3dObject, itemType);
	renderOptionPage_->BindToItem(g3dObject, itemType);
}