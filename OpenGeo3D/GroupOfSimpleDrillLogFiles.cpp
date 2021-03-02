#include "GroupOfSimpleDrillLogFiles.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "Text.h"

GroupOfSimpleDrillLogFiles::GroupOfSimpleDrillLogFiles(QWidget* parent) : QGroupBox(Text::labelOfSimpleDrillLogGroup(), parent) {
	QWidget* widget = new QWidget(this);
	QHBoxLayout* layoutOfPositionFilePath = new QHBoxLayout(widget);
	layoutOfPositionFilePath->addWidget(new QLabel(Text::labelOfDrillPositionFilePath()));
	positionFilePath_ = new QLineEdit();
	layoutOfPositionFilePath->addWidget(positionFilePath_, 1);
	QPushButton* btnOpen = new QPushButton(Text::labelOfOpen());
	layoutOfPositionFilePath->addWidget(btnOpen);
	widget->setLayout(layoutOfPositionFilePath);

	drillList_ = new QTableWidget();
	drillList_->setColumnCount(6);
	QStringList headers;
	headers << QStringLiteral("#") << QStringLiteral("Drill No") << QStringLiteral("X") << QStringLiteral("Y") << QStringLiteral("Z") << QStringLiteral("Depth");
	drillList_->setHorizontalHeaderLabels(headers);
	drillList_->setColumnWidth(0, 30);
	drillList_->setColumnWidth(1, 60);
	drillList_->setColumnWidth(2, 100);
	drillList_->setColumnWidth(3, 100);
	drillList_->setColumnWidth(4, 100);
	drillList_->setColumnWidth(5, 100);
	//drillList_->setMinimumSize();

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(widget, 1);
	layout->addWidget(drillList_, 1, Qt::AlignmentFlag::AlignHCenter);
	setLayout(layout);
}

GroupOfSimpleDrillLogFiles::~GroupOfSimpleDrillLogFiles() {

}
