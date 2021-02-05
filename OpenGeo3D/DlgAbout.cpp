#include "DlgAbout.h"
#include <QtCore/QDate>
#include <ui_DlgAbout.h>
#include "Text.h"

DlgAbout::DlgAbout(QWidget* parent) : QDialog(parent, Qt::Dialog | Qt::WindowCloseButtonHint) {
	ui_ = new Ui::DlgAbout();
	ui_->setupUi(this);

	ui_->appInfo->setText(Text::appName() + " " + Text::appVersion());
	QString copyRightString = QString::fromUtf8("\u00A92020");
	QDate dt = QDate::currentDate();
	if (dt.year() > 2020) {
		copyRightString += QStringLiteral("-") + dt.toString("yyyy");
	}
	ui_->appCopyright->setText(copyRightString);
	ui_->appDescription->setText(Text::appDescription());
	ui_->appWebSite->setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
	ui_->appWebSite->setOpenExternalLinks(true);
	ui_->appWebSite->setText(QStringLiteral("<a href=") + Text::appWebSite() + QStringLiteral(">") + Text::appWebSite() + QStringLiteral("</a>"));
	this->setWindowTitle(Text::titleOfDlgAbout());
}

DlgAbout::~DlgAbout() {
	delete ui_;
}

