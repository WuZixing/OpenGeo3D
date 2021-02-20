#include "ProjectMetaBook.h"
#include "Text.h"

ProjectMetaBook::ProjectMetaBook(QWidget* parent) : QTabWidget(parent) {
	setTabPosition(QTabWidget::TabPosition::South);
	metadataPage_ = new MetadataPage(this);
	addTab(metadataPage_, Text::titleOfMetadataPage());
}

ProjectMetaBook::~ProjectMetaBook() {

}

void ProjectMetaBook::setCurrentItem(QTreeWidgetItem* item) {
	metadataPage_->setCurrentItem(item);
}
