#include "GroupOfSimpleDrillLogFiles.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <ogrsf_frmts.h>
#include <geo3dml/Field.h>
#include "BusyCursor.h"
#include "CRSSet.h"
#include "ComboBoxItemDelegate.h"
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
	logFieldList_->setMaximumWidth(width);
	headers.clear();
	headers << QString::fromUtf8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Integer).c_str())
		<< QString::fromUtf8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Double).c_str())
		<< QString::fromUtf8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Boolean).c_str())
		<< QString::fromUtf8(geo3dml::Field::ValueTypeToName(geo3dml::Field::ValueType::Text).c_str());
	ComboBoxItemDelegate* itemDelegate = new ComboBoxItemDelegate(this);
	itemDelegate->setValues(headers);
	logFieldList_->setItemDelegateForColumn(1, itemDelegate);
	hBox->addWidget(logFieldList_, 1);
	widget->setLayout(hBox);
	layout->addWidget(widget, 1);

	QGridLayout* gridLayout = new QGridLayout(this);
	QCheckBox* checkBox = new QCheckBox(Text::labelOfSavingDrillPositionToFile(), this);
	checkBox->setChecked(false);
	connect(checkBox, &QCheckBox::stateChanged, this, &GroupOfSimpleDrillLogFiles::shpFileChecked);
	gridLayout->addWidget(checkBox, 0, 0);
	shpFilePath_ = new QLineEdit(this);
	shpFilePath_->setReadOnly(true);
	shpFilePath_->setEnabled(false);
	gridLayout->addWidget(shpFilePath_, 0, 1);
	btnSavePositionToShp_ = new QPushButton(Text::labelOfSave(), this);
	btnSavePositionToShp_->setEnabled(false);
	connect(btnSavePositionToShp_, &QPushButton::clicked, this, &GroupOfSimpleDrillLogFiles::saveDrillPositionToFile);
	gridLayout->addWidget(btnSavePositionToShp_, 0, 2);
	gridLayout->addWidget(new QLabel(Text::labelOfCRS(), this), 1, 0, Qt::AlignmentFlag::AlignRight);
	crsList_ = new QComboBox(this);
	QStringList crsCodes = CRSSet::availableCRSNames();
	crsList_->addItems(crsCodes);
	crsList_->setCurrentIndex(0);
	crsList_->setEnabled(false);
	gridLayout->addWidget(crsList_, 1, 1);
	layout->addLayout(gridLayout);

	setLayout(layout);
}

GroupOfSimpleDrillLogFiles::~GroupOfSimpleDrillLogFiles() {

}

void GroupOfSimpleDrillLogFiles::openDrillPosition() {
	QString filePath = QFileDialog::getOpenFileName(this, QString(), QDir::currentPath(), Text::filterOfDrillPositionFile());
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
			item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
			drillList_->setItem(row, 0, item);
			for (int c = 0; c < colCount; ++c) {
				item = new QTableWidgetItem(tokens[c]);
				item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
				drillList_->setItem(row, c, item);
			}
		}
	}
}

void GroupOfSimpleDrillLogFiles::appendLogs() {
	QStringList filePaths = QFileDialog::getOpenFileNames(this, QString(), QDir::currentPath(), Text::filterOfDrillLogFile());
	if (filePaths.isEmpty()) {
		return;
	}
	auto* ptr = logFieldList_->itemDelegateForColumn(1);
	BusyCursor waiting;
	QRegularExpression regExp("[,\\t]");
	QStringList::const_iterator citor = filePaths.cbegin();
	while (citor != filePaths.cend()) {
		QString filePath = *citor;
		QFileInfo fileInfo(filePath);
		QDir::setCurrent(fileInfo.absolutePath());
		QString baseName = fileInfo.baseName();
		int row = logFileList_->rowCount();
		logFileList_->insertRow(row);
		QTableWidgetItem* item = new QTableWidgetItem(baseName);
		logFileList_->setItem(row, 0, item);
		item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
		item = new QTableWidgetItem(filePath);
		item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
		logFileList_->setItem(row, 1, item);
		// read field names from the header line.
		QFile file(filePath);
		if (file.open(QIODevice::OpenModeFlag::ReadOnly | QIODevice::OpenModeFlag::Text)) {
			QTextStream textStream(&file);
			QString header = textStream.readLine();
			QStringList tokens = header.split(regExp);
			for (int c = 1; c < tokens.size(); ++c) {	// the first column is depth.
				QString fieldName = tokens[c];
				if (!fieldSet_.contains(fieldName)) {
					fieldSet_.insert(fieldName);
					row = logFieldList_->rowCount();
					logFieldList_->insertRow(row);
					item = new QTableWidgetItem(fieldName);
					item->setFlags(item->flags() ^ Qt::ItemFlag::ItemIsEditable);
					logFieldList_->setItem(row, 0, item);
				}
			}
		}
		++citor;
	}
}

void GroupOfSimpleDrillLogFiles::clearLogs() {
	QMessageBox::StandardButton btn = QMessageBox::question(this, QString(), Text::confirmToClearFileList(),
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		return;
	}
	BusyCursor waiting;
	logFileList_->clearContents();
	logFileList_->setRowCount(0);
	logFieldList_->clearContents();
	logFieldList_->setRowCount(0);
	fieldSet_.clear();
}

bool GroupOfSimpleDrillLogFiles::validate() {
	if (drillList_->rowCount() < 1) {
		QMessageBox::warning(this, QString(), Text::tipOfEmptyDrillPosition());
		drillList_->setFocus();
		return false;
	}
	if (logFileList_->rowCount() < 1) {
		QMessageBox::warning(this, QString(), Text::tipOfEmptyDrillLogFiles());
		logFileList_->setFocus();
		return false;
	}
	int fieldCount = logFieldList_->rowCount();
	for (int r = 0; r < fieldCount; ++r) {
		QString typeName;
		QTableWidgetItem* item = logFieldList_->item(r, 1);
		if (item != nullptr) {
			typeName = item->text();
		}
		if (typeName.isEmpty()) {
			QMessageBox::warning(this, QString(), Text::tipOfUnknownFieldValueType());
			logFieldList_->selectRow(r);
			logFieldList_->setFocus();
			return false;
		}
	}
	if (btnSavePositionToShp_->isEnabled()) {
		if (shpFilePath_->text().isEmpty()) {
			QMessageBox::warning(this, QString(), Text::tipOfEmptyDrillPositionShpFilePath());
			btnSavePositionToShp_->setFocus();
			return false;
		}
		if (crsList_->currentText().isEmpty()) {
			QMessageBox::warning(this, QString(), Text::tipOfEmptyCRS());
			crsList_->setFocus();
			return false;
		}
	}

	return true;
}

DrillPositionMap GroupOfSimpleDrillLogFiles::getDrillPositions() const {
	DrillPositionMap pos;
	int count = drillList_->rowCount();
	for (int i = 0; i < count; ++i) {
		QString drillNo = drillList_->item(i, 0)->text();
		double x = drillList_->item(i, 1)->text().toDouble();
		double y = drillList_->item(i, 2)->text().toDouble();
		double z = drillList_->item(i, 3)->text().toDouble();
		pos[drillNo] = geo3dml::Point3D(x, y, z);
	}
	return pos;
}

DrillLogFileMap GroupOfSimpleDrillLogFiles::getDrillLogFiles() const {
	DrillLogFileMap logFiles;
	int count = logFileList_->rowCount();
	for (int i = 0; i < count; ++i) {
		QString drillNo = logFileList_->item(i, 0)->text();
		QString filePath = logFileList_->item(i, 1)->text();
		logFiles[drillNo] = filePath;
	}
	return logFiles;
}

DrillLogFieldMap GroupOfSimpleDrillLogFiles::getDrillLogFields() const {
	DrillLogFieldMap fields;
	int count = logFieldList_->rowCount();
	for (int i = 0; i < count; ++i) {
		std::string name = logFieldList_->item(i, 0)->text().toUtf8().constData();
		std::string typeName = logFieldList_->item(i, 1)->text().toUtf8().constData();
		geo3dml::Field field;
		field.Name(name).Label(name).DataType(geo3dml::Field::NameToValueType(typeName));
		fields[name] = field;
	}
	return fields;
}

void GroupOfSimpleDrillLogFiles::shpFileChecked(int state) {
	bool status = (state == Qt::CheckState::Checked);
	shpFilePath_->setEnabled(status);
	btnSavePositionToShp_->setEnabled(status);
	crsList_->setEnabled(status);
}

void GroupOfSimpleDrillLogFiles::saveDrillPositionToFile() {
	QString filePath = QFileDialog::getSaveFileName(this, QString(), QDir::currentPath(), Text::filterOfShpFile());
	if (filePath.isEmpty()) {
		return;
	}
	QFileInfo fileInfo(filePath);
	QDir::setCurrent(fileInfo.absolutePath());
	shpFilePath_->setText(filePath);
}

bool GroupOfSimpleDrillLogFiles::isSavingPositionToSHPEnabled() const {
	return btnSavePositionToShp_->isEnabled();
}

bool GroupOfSimpleDrillLogFiles::savePositionToSHP() const {
	QString filePath = shpFilePath_->text();
	if (filePath.isEmpty()) {
		return false;
	}
	QString baseName = QFileInfo(filePath).baseName();

	GDALAllRegister();
	GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("ESRI Shapefile");
	if (poDriver == nullptr) {
		return false;
	}
	GDALDataset* poDS = poDriver->Create(filePath.toUtf8().constData(), 0, 0, 0, GDALDataType::GDT_Unknown, nullptr);
	if (poDS == nullptr) {
		return false;
	}
	OGRSpatialReference srs;
	QString crsName = crsList_->currentText();
	if (!CRSSet::loadCRS(srs, crsName)) {
		srs.Clear();
	}
	OGRLayer* poLayer = poDS->CreateLayer(baseName.toUtf8().constData(), &srs, OGRwkbGeometryType::wkbPointZM);
	if (poLayer == nullptr) {
		GDALClose(poDS);
		return false;
	}
	OGRFieldDefn oField("DrillNo", OGRFieldType::OFTString);
	oField.SetWidth(32);
	if (poLayer->CreateField(&oField) != OGRERR_NONE) {
		GDALClose(poDS);
		return false;
	}
	DrillPositionMap posMap = getDrillPositions();
	for (auto citor = posMap.cbegin(); citor != posMap.cend(); ++citor) {
		OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		if (poFeature == nullptr) {
			continue;
		}
		poFeature->SetField(oField.GetNameRef(), citor->first.toUtf8().constData());
		OGRPoint pt(citor->second.x, citor->second.y, citor->second.z);
		poFeature->SetGeometry(&pt);
		poLayer->CreateFeature(poFeature);
		OGRFeature::DestroyFeature(poFeature);
	}

	GDALClose(poDS);
	return true;
}
