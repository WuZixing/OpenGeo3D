#include "DlgAbout.h"
#include <QtCore/QDate>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include "Text.h"

DlgAbout::DlgAbout(QWidget* parent) : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint) {
	setWindowTitle(Text::titleOfDlgAbout());
	initUI();
}

DlgAbout::~DlgAbout() {

}

void DlgAbout::initUI() {
	QLabel* appInfo = new QLabel(Text::appName() + "  " + Text::appVersion());
	QString copyRightString = QString::fromUtf8("\u00A92020");
	QDate dt = QDate::currentDate();
	if (dt.year() > 2020) {
		copyRightString += QStringLiteral("-") + dt.toString("yyyy");
	}
	QLabel* appCopyright = new QLabel(copyRightString);
	QLabel* appDescription = new QLabel(Text::appDescription());
	QLabel* appWebSite = new QLabel(QStringLiteral("<a href=") + Text::appWebSite() + QStringLiteral(">") + Text::appWebSite() + QStringLiteral("</a>"));
	appWebSite->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
	appWebSite->setOpenExternalLinks(true);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok, Qt::Orientation::Horizontal);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->addWidget(appInfo, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(appCopyright, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(appDescription, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(appWebSite, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(buttonBox, 0, Qt::AlignmentFlag::AlignHCenter);

	setLayout(vLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}
