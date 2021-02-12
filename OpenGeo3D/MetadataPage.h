#pragma once

#include <QtTreePropertyBrowser>

class MetadataPage : public QtTreePropertyBrowser {
	Q_OBJECT

public:
	MetadataPage(QWidget* parent);
	virtual ~MetadataPage();
};
