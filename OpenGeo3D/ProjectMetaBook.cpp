#include "ProjectMetaBook.h"
#include "Text.h"

ProjectMetaBook::ProjectMetaBook(QWidget* parent) : QTabWidget(parent) {
	setTabPosition(QTabWidget::TabPosition::South);
	metadataPage_ = new MetadataPage(this);
	addTab(metadataPage_, Text::titleOfMetadataPage());
	renderOptionPage_ = new RenderOptionPage(this);
	addTab(renderOptionPage_, Text::titleOfRenderOptionPage());
}

ProjectMetaBook::~ProjectMetaBook() {

}

void ProjectMetaBook::setCurrentItem(QTreeWidgetItem* item) {
	metadataPage_->setCurrentItem(item);
	renderOptionPage_->setCurrentItem(item);
}
