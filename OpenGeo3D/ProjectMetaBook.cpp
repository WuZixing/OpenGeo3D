#include "ProjectMetaBook.h"
#include "MetadataPage.h"

ProjectMetaBook::ProjectMetaBook(QWidget* parent) : QTabWidget(parent) {
	MetadataPage* metadataPage_ = new MetadataPage(this);

}

ProjectMetaBook::~ProjectMetaBook() {

}
