// UTF-8编码
#include "DlgAbout.h"
#include <QtCore/QDate>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include "Text.h"
#include "generated/gitVersion.h"

DlgAbout::DlgAbout(QWidget* parent) : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint) {
	setWindowTitle(Text::titleOfDlgAbout());
	initUI();
}

DlgAbout::~DlgAbout() {

}

void DlgAbout::initUI() {
	QLabel* appInfo = new QLabel(Text::appName() + " " + Text::appVersion());
	QLabel* gitVersion = new QLabel(QStringLiteral("Git commit: ") + QString::fromStdString(OpenGeo3D::git_hash) + QStringLiteral("@") + QString::fromStdString(OpenGeo3D::git_branch));
	QString copyRightString = QString::fromUtf8("\u00A92020");
	QDate dt = QDate::currentDate();
	if (dt.year() > 2020) {
		copyRightString += QStringLiteral("-") + dt.toString("yyyy");
	}
	QLabel* appCopyright = new QLabel(copyRightString);
	QLabel* appDescription = new QLabel(Text::appDescription());
	QLabel* appWebSite = new QLabel(QStringLiteral("开源项目地址：<a href=") + Text::appWebSite() + QStringLiteral(">") + Text::appWebSite() + QStringLiteral("</a>"));
	appWebSite->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
	appWebSite->setOpenExternalLinks(true);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::StandardButton::Ok, Qt::Orientation::Horizontal);

	QVBoxLayout* vLayout = new QVBoxLayout(this);
	vLayout->addWidget(appInfo, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(gitVersion, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(appCopyright, 0, Qt::AlignmentFlag::AlignHCenter);
	vLayout->addWidget(appDescription, 0, Qt::AlignmentFlag::AlignLeft);
	vLayout->addWidget(appWebSite, 0, Qt::AlignmentFlag::AlignLeft);
	vLayout->addWidget(buttonBox, 0, Qt::AlignmentFlag::AlignHCenter);

	setLayout(vLayout);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}
