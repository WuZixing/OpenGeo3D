#pragma once

#include <QMainWindow>
#include "ProjectPanel.h"
#include "RenderWidget.h"

class AppFrame : public QMainWindow {
	Q_OBJECT

public:
	AppFrame(QWidget* parent = nullptr);
	virtual ~AppFrame();

private slots:
	void openGeo3DML();
	void openDrillLog();
	void closeStructureModel();
	void about();
	void quit();

protected:
    virtual bool event(QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
	void setupMenu();
	void setupWidgets();

private:
	ProjectPanel* projectPanel_;
	RenderWidget* renderWidget_;
};
