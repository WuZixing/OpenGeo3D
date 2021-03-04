#include "GroupOfSimpleDrillLogFiles.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "Text.h"

GroupOfSimpleDrillLogFiles::GroupOfSimpleDrillLogFiles(QWidget* parent) : QGroupBox(Text::labelOfSimpleDrillLogGroup(), parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	QFontMetrics fontMetrics(font());
	const int widthOfNo = fontMetrics.size(Qt::TextSingleLine, QStringLiteral("1234")).width();
	const int widthOfDrillNo = fontMetrics.size(Qt::TextSingleLine, QStringLiteral("ABCD-0123456789")).width();
	const int widthOfCoord = fontMetrics.size(Qt::TextSingleLine, QStringLiteral("0123456789.123456")).width();
	const int widthOfFilePath = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 60;
	const int widthOfFieldName = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 20;
	const int widthOfFieldType = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 20;

	QWidget* widget = new QWidget(this);
	QHBoxLayout* hBox = new QHBoxLayout(widget);
	hBox->addWidget(new QLabel(Text::labelOfDrillPositionFilePath(), widget));
	positionFilePath_ = new QLineEdit(widget);
	positionFilePath_->setMinimumWidth(widthOfFilePath);
	positionFilePath_->setReadOnly(true);
	hBox->addWidget(positionFilePath_, 1);
	QPushButton* btn = new QPushButton(Text::labelOfOpen(), widget);
	connect(btn, &QPushButton::clicked, this, &GroupOfSimpleDrillLogFiles::openDrillPosition);
	hBox->addWidget(btn);
	widget->setLayout(hBox);
	layout->addWidget(widget);

	QStringList headers;
	headers << Text::labelOfNo() << Text::labelOfDrillNo() << Text::labelOfX() << Text::labelOfY() << Text::labelOfZ() << Text::labelOfDrillDepth();
	drillList_ = new QTableWidget(this);
	drillList_->setColumnCount(headers.size());
	drillList_->setHorizontalHeaderLabels(headers);
	drillList_->setColumnWidth(0, widthOfNo);
	drillList_->setColumnWidth(1, widthOfDrillNo);
	drillList_->setColumnWidth(2, widthOfCoord);
	drillList_->setColumnWidth(3, widthOfCoord);
	drillList_->setColumnWidth(4, widthOfCoord);
	drillList_->setColumnWidth(5, widthOfCoord / 2);
	headers.clear();
	layout->addWidget(drillList_, 1);

	widget = new QWidget(this);
	hBox = new QHBoxLayout(widget);
	hBox->addWidget(new QLabel(Text::labelOfDrillLog(), widget));
	btn = new QPushButton(Text::labelOfAppend(), widget);
	connect(btn, &QPushButton::clicked, this, &GroupOfSimpleDrillLogFiles::appendLogs);
	hBox->addWidget(btn);
	btn = new QPushButton(Text::labelOfClear(), widget);
	connect(btn, &QPushButton::clicked, this, &GroupOfSimpleDrillLogFiles::clearLogs);
	hBox->addWidget(btn);
	hBox->addStretch(1);
	hBox->addWidget(new QLabel(Text::tipOfSpecifyFieldValueType(), widget));
	widget->setLayout(hBox);
	layout->addWidget(widget);

	widget = new QWidget(this);
	hBox = new QHBoxLayout(widget);
	headers << Text::labelOfNo() << Text::labelOfDrillNo() << Text::labelOfFilePath();
	logFileList_ = new QTableWidget(widget);
	logFileList_->setColumnCount(headers.size());
	logFileList_->setHorizontalHeaderLabels(headers);
	logFileList_->setColumnWidth(0, widthOfNo);
	logFileList_->setColumnWidth(1, widthOfDrillNo);
	logFileList_->setColumnWidth(2, widthOfFilePath);
	headers.clear();
	hBox->addWidget(logFileList_, 1);
	headers << Text::labelOfNo() << Text::labelOfField() << Text::labelOfFieldValueType();
	logFieldList_ = new QTableWidget(widget);
	logFieldList_->setColumnCount(3);
	logFieldList_->setHorizontalHeaderLabels(headers);
	logFieldList_->setColumnWidth(0, widthOfNo);
	logFieldList_->setColumnWidth(1, widthOfFieldName);
	logFieldList_->setColumnWidth(1, widthOfFieldType);
	headers.clear();
	hBox->addWidget(logFieldList_, 1);
	widget->setLayout(hBox);
	layout->addWidget(widget, 1);
	setLayout(layout);
}

GroupOfSimpleDrillLogFiles::~GroupOfSimpleDrillLogFiles() {

}

void GroupOfSimpleDrillLogFiles::openDrillPosition() {

}

void GroupOfSimpleDrillLogFiles::appendLogs() {

}

void GroupOfSimpleDrillLogFiles::clearLogs() {

}
