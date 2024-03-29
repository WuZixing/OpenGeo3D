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
	void openGeoJSON();
	void closeStructureModel();
	void about();
	void quit();
	void fullView();
	void changeBackgroundColor();
	void scaleZUp();
	void scaleZDown();
	void customizedZScale();
	void resetZScale();
	void toggleProjectPanel();
	void windowMenuAboutToShow();
	void saveToGeo3DML();
	void closeAllModels();

protected:
    virtual bool event(QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
	void setupMenu();
	void setupWidgets();

	QString selectAFile(const QString& dialogCaption, const QString& fileFilters);

private:
	QDockWidget* dockWidget_;
	ProjectPanel* projectPanel_;
	RenderWidget* renderWidget_;
	QAction* menuProjectPanel_;
};
