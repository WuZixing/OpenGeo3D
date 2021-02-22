#include "RenderOptionPage.h"

RenderOptionPage::RenderOptionPage(QWidget* parent) : QtTreePropertyBrowser(parent) {
	setRootIsDecorated(false);
	setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
	setAlternatingRowColors(false);
	setHeaderVisible(false);
}

RenderOptionPage::~RenderOptionPage() {

}

void RenderOptionPage::setCurrentItem(QTreeWidgetItem* item) {

}
