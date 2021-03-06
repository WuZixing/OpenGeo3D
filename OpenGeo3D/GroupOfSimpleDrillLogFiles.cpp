#include "GroupOfSimpleDrillLogFiles.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "BusyCursor.h"
#include "Text.h"

GroupOfSimpleDrillLogFiles::GroupOfSimpleDrillLogFiles(QWidget* parent) : QGroupBox(Text::labelOfSimpleDrillLogGroup(), parent) {
	QVBoxLayout* layout = new QVBoxLayout(this);
	QFontMetrics fontMetrics(font());
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
	headers << Text::labelOfDrillNo() << Text::labelOfX() << Text::labelOfY() << Text::labelOfZ() << Text::labelOfDrillDepth();
	drillList_ = new QTableWidget(this);
	drillList_->setColumnCount(headers.size());
	drillList_->setHorizontalHeaderLabels(headers);
	drillList_->setColumnWidth(0, widthOfDrillNo);
	drillList_->setColumnWidth(1, widthOfCoord);
	drillList_->setColumnWidth(2, widthOfCoord);
	drillList_->setColumnWidth(3, widthOfCoord);
	drillList_->setColumnWidth(4, widthOfCoord / 2);
	int width = 0;
	for (int m = 0; m < drillList_->columnCount(); ++m) {
		width += drillList_->columnWidth(m);
	}
	drillList_->setMinimumWidth(width / 10 * 9);
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
	headers << Text::labelOfDrillNo() << Text::labelOfFilePath();
	logFileList_ = new QTableWidget(widget);
	logFileList_->setColumnCount(headers.size());
	logFileList_->setHorizontalHeaderLabels(headers);
	logFileList_->setColumnWidth(0, widthOfDrillNo);
	logFileList_->setColumnWidth(1, widthOfFilePath);
	width = 0;
	for (int m = 0; m < logFileList_->columnCount(); ++m) {
		width += logFileList_->columnWidth(m);
	}
	logFileList_->setMaximumWidth(width / 10 * 9);
	headers.clear();
	hBox->addWidget(logFileList_, 1);
	headers << Text::labelOfField() << Text::labelOfFieldValueType();
	logFieldList_ = new QTableWidget(widget);
	logFieldList_->setColumnCount(headers.size());
	logFieldList_->setHorizontalHeaderLabels(headers);
	logFieldList_->setColumnWidth(0, widthOfFieldName);
	logFieldList_->setColumnWidth(1, widthOfFieldType);
	width = 0;
	for (int m = 0; m < logFieldList_->columnCount(); ++m) {
		width += logFieldList_->columnWidth(m);
	}
	logFieldList_->setMaximumWidth(width / 10 * 9);
	headers.clear();
	hBox->addWidget(logFieldList_, 1);
	widget->setLayout(hBox);
	layout->addWidget(widget, 1);
	setLayout(layout);
}

GroupOfSimpleDrillLogFiles::~GroupOfSimpleDrillLogFiles() {

}

void GroupOfSimpleDrillLogFiles::openDrillPosition() {
	QString filePath = QFileDialog::getOpenFileName(this, QString(), QDir::currentPath(), Text::filterOfDrillPositionFiles());
	if (filePath.isEmpty()) {
		return;
	}
	QDir dir(filePath);
	dir.cdUp();
	QDir::setCurrent(dir.path());
	QFile file(filePath);
	if (!file.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text)) {
		QMessageBox::warning(this, QString(), Text::errorOfOpenFile(filePath, file.errorString()));
		return;
	}
	BusyCursor waiting;
	positionFilePath_->setText(filePath);
	drillList_->clearContents();
	drillList_->setRowCount(0);
	int colCount = drillList_->columnCount();
	QRegularExpression regExp("[,\\t]");
	QTextStream textStream(&file);
	QString line = textStream.readLine();	// ignore header line
	while (!textStream.atEnd()) {
		line = textStream.readLine();
		QStringList tokens = line.split(regExp);
		if (tokens.size() == colCount) {
			int row = drillList_->rowCount();
			drillList_->insertRow(row);
			QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
			drillList_->setItem(row, 0, item);
			for (int c = 0; c < colCount; ++c) {
				item = new QTableWidgetItem(tokens[c]);
				drillList_->setItem(row, c, item);
			}
		}
	}
}

void GroupOfSimpleDrillLogFiles::appendLogs() {

}

void GroupOfSimpleDrillLogFiles::clearLogs() {

}
