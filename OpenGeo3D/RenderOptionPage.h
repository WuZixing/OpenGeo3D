#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtTreePropertyBrowser>
#include <QtVariantPropertyManager>

class RenderOptionPage : public QtTreePropertyBrowser {
	Q_OBJECT

public:
	RenderOptionPage(QWidget* parent);
	virtual ~RenderOptionPage();

	void setCurrentItem(QTreeWidgetItem* item);

};
