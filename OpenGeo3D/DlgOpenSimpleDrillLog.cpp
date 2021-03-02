#include "DlgOpenSimpleDrillLog.h"
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>
#include "GroupOfSimpleDrillLogFiles.h"
#include "Text.h"

DlgOpenSimpleDrillLog::DlgOpenSimpleDrillLog(QWidget* parent) : QDialog(parent, Qt::WindowType::Dialog | Qt::WindowType::WindowCloseButtonHint) {
	setWindowTitle(Text::menuOpenDrillLog());
	initUI();
}

DlgOpenSimpleDrillLog::~DlgOpenSimpleDrillLog() {

}

void DlgOpenSimpleDrillLog::initUI() {
	GroupOfSimpleDrillLogFiles* group = new GroupOfSimpleDrillLogFiles(this);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok | QDialogButtonBox::StandardButton::Cancel, Qt::Orientation::Horizontal);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(group, 1, Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(buttonBox, 0, Qt::AlignmentFlag::AlignHCenter);
	setLayout(layout);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}
