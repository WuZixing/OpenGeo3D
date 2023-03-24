// UTF-8编码
#include "DlgOpenSimpleDrillLog.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include <g3dvtk/ObjectFactory.h>
#include "DrillLogFile.h"
#include "Text.h"
#include <QLabel>
#include <QFileDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QMessageBox>

DlgOpenSimpleDrillLog::DlgOpenSimpleDrillLog(QWidget* parent) : QDialog(parent, Qt::WindowType::Dialog | Qt::WindowType::WindowCloseButtonHint) {
	setWindowTitle(Text::menuOpenDrillLog());
	initUI();
}

DlgOpenSimpleDrillLog::~DlgOpenSimpleDrillLog() {

}

void DlgOpenSimpleDrillLog::initUI() {
	group_ = nullptr; // new GroupOfSimpleDrillLogFiles(this);
	QFontMetrics fontMetrics(font());
	const int widthOfDrillNo = fontMetrics.size(Qt::TextSingleLine, QStringLiteral("ABCD-0123456789")).width();
	const int widthOfCoord = fontMetrics.size(Qt::TextSingleLine, QStringLiteral("0123456789.123456")).width();
	const int widthOfFilePath = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 60;
	const int widthOfFieldName = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 20;
	const int widthOfFieldType = fontMetrics.size(Qt::TextSingleLine, QStringLiteral(" ")).width() * 20;

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, Qt::Orientation::Horizontal);

	QVBoxLayout* layout = new QVBoxLayout(this);
	// layout->addWidget(group_, 1, Qt::AlignmentFlag::AlignLeft);
	QWidget* widget = new QWidget(this);
	QHBoxLayout* hBox = new QHBoxLayout(widget);
	hBox->addWidget(new QLabel(QStringLiteral("网格文件："), widget));
	dataFilePath_ = new QLineEdit(widget);
	dataFilePath_->setMinimumWidth(widthOfFilePath);
	dataFilePath_->setReadOnly(true);
	hBox->addWidget(dataFilePath_, 1);
	QPushButton* btn = new QPushButton(Text::labelOfOpen(), widget);
	connect(btn, &QPushButton::clicked, this, &DlgOpenSimpleDrillLog::openDataFile);
	hBox->addWidget(btn);
	widget->setLayout(hBox);
	layout->addWidget(widget);

	QGroupBox* groupBox = new QGroupBox(QStringLiteral("网格参数"), this);
	QDoubleValidator* doubleValidator = new QDoubleValidator(groupBox);
	QIntValidator* intValidator = new QIntValidator(groupBox);
	QGridLayout* gridLayout = new QGridLayout(groupBox);
	gridLayout->addWidget(new QLabel(QStringLiteral("X"), groupBox), 0, 1);
	gridLayout->addWidget(new QLabel(QStringLiteral("Y"), groupBox), 0, 2);
	gridLayout->addWidget(new QLabel(QStringLiteral("Z"), groupBox), 0, 3);
	gridLayout->addWidget(new QLabel(QStringLiteral("网格数目"), groupBox), 1, 0);
	editDimX_ = new QLineEdit("64", groupBox);
	editDimX_->setValidator(intValidator);
	gridLayout->addWidget(editDimX_, 1, 1);
	editDimY_ = new QLineEdit("64", groupBox);
	editDimY_->setValidator(intValidator);
	gridLayout->addWidget(editDimY_, 1, 2);
	editDimZ_ = new QLineEdit("160", groupBox);
	editDimZ_->setValidator(intValidator);
	gridLayout->addWidget(editDimZ_, 1, 3);
	groupBox->setLayout(gridLayout);
	gridLayout->addWidget(new QLabel(QStringLiteral("单个网格大小"), groupBox), 2, 0);
	editStepX_ = new QLineEdit("2.0", groupBox);
	editStepX_->setValidator(doubleValidator);
	gridLayout->addWidget(editStepX_, 2, 1);
	editStepY_ = new QLineEdit("2.0", groupBox);
	editStepY_->setValidator(doubleValidator);
	gridLayout->addWidget(editStepY_, 2, 2);
	editStepZ_ = new QLineEdit("0.25", groupBox);
	editStepZ_->setValidator(doubleValidator);
	gridLayout->addWidget(editStepZ_, 2, 3);
	gridLayout->addWidget(new QLabel(QStringLiteral("无效属性值"), groupBox), 3, 0);
	editNoData_ = new QLineEdit("-99", groupBox);
	editNoData_->setValidator(intValidator);
	gridLayout->addWidget(editNoData_, 3, 1);
	layout->addWidget(groupBox);


	layout->addWidget(buttonBox, 0, Qt::AlignmentFlag::AlignHCenter);
	setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DlgOpenSimpleDrillLog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &DlgOpenSimpleDrillLog::reject);
}

void DlgOpenSimpleDrillLog::accept() {
	if (dataFilePath_->text().isEmpty()) {
		QMessageBox::warning(this, QString(), QStringLiteral("没有指定数据文件路径。"));
		dataFilePath_->setFocus();
		return;
	}
	int dimX = 0, dimY = 0, dimZ = 0;
	if (!getGridDim(dimX, dimY, dimZ)) {
		QMessageBox::warning(this, QString(), QStringLiteral("没有指定有效的网格数目参数。"));
		editDimX_->setFocus();
		return;
	}
	double stepX = 0, stepY = 0, stepZ = 0;
	if (!getGridStep(stepX, stepY, stepZ)) {
		QMessageBox::warning(this, QString(), QStringLiteral("没有指定有效的网格大小参数。"));
		editStepX_->setFocus();
		return;
	}
	if (!getNoDataValue(dimX)) {
		QMessageBox::warning(this, QString(), QStringLiteral("没有指定有效的无效值参数。"));
		editNoData_->setFocus();
		return;
	}
	QDialog::accept();
}

void DlgOpenSimpleDrillLog::openDataFile() {
	QString filePath = QFileDialog::getOpenFileName(this, QString(), QString(), QStringLiteral("网格数据(*.*)"));
	if (filePath.isEmpty()) {
		return;
	}
	dataFilePath_->setText(filePath);
}

QString DlgOpenSimpleDrillLog::getDataFilePath() const {
	return dataFilePath_->text();
}

bool DlgOpenSimpleDrillLog::getGridDim(int& dimX, int& dimY, int& dimZ) const {
	bool isOk = false;
	dimX = editDimX_->text().toInt(&isOk);
	if (!isOk) {
		return false;
	}
	dimY = editDimY_->text().toInt(&isOk);
	if (!isOk) {
		return false;
	}
	dimZ = editDimZ_->text().toInt(&isOk);
	if (!isOk) {
		return false;
	}
	return true;
}

bool DlgOpenSimpleDrillLog::getGridStep(double& stepX, double& stepY, double& stepZ) const {
	bool isOk = false;
	stepX = editStepX_->text().toDouble(&isOk);
	if (!isOk) {
		return false;
	}
	stepY = editStepY_->text().toDouble(&isOk);
	if (!isOk) {
		return false;
	}
	stepZ = editStepZ_->text().toDouble(&isOk);
	if (!isOk) {
		return false;
	}
	return true;
}

bool DlgOpenSimpleDrillLog::getNoDataValue(int& value) const {
	bool isOk = false;
	value = editNoData_->text().toInt(&isOk);
	return isOk;
}
