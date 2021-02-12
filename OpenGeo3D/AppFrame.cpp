#include "AppFrame.h"
#include <QtGui/QCloseEvent>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QVBoxLayout>
#include <g3dvtk/ObjectFactory.h>
#include <g3dxml/XMLReader.h>
#include "icon.xpm"
#include "BusyCursor.h"
#include "Events.h"
#include "DlgAbout.h"
#include "ProjectPanel.h"
#include "RenderWidget.h"
#include "Text.h"

AppFrame::AppFrame(QWidget* parent) : QMainWindow(parent) {
	setWindowIcon(QIcon(QPixmap(xpm_icon)));
	setupMenu();
	setupWidgets();
}

AppFrame::~AppFrame() {

}

void AppFrame::about() {
	DlgAbout about(this);
	about.exec();
}

void AppFrame::setupMenu() {
	QMenu* menu = menuBar()->addMenu(Text::menuFile());
	QMenu* subMenu = menu->addMenu(Text::menuStructureModel());
	subMenu->addAction(Text::menuOpenGeo3DML(), this, &AppFrame::openGeo3DML);
	subMenu->addAction(Text::menuOpenDrillLog(), this, &AppFrame::openDrillLog);
	subMenu->addSeparator();
	subMenu->addAction(Text::menuCloseStructureModel(), this, &AppFrame::closeStructureModel);
	menu->addSeparator();
	menu->addAction(Text::menuQuit(), this, &AppFrame::quit);
	menu = menuBar()->addMenu(Text::menuHelp());
	menu->addAction(Text::menuAbout(), this, &AppFrame::about);
}

void AppFrame::setupWidgets() {
	QDockWidget* dockWidget = new QDockWidget(this);
	projectPanel_ = new ProjectPanel(dockWidget);
	dockWidget->setWidget(projectPanel_);
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, dockWidget);
	renderWidget_ = new RenderWidget(this, projectPanel_->getRenderer());
	setCentralWidget(renderWidget_);
}

void AppFrame::closeEvent(QCloseEvent* event) {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, Text::titleOfConfirmingQuit(), Text::confirmToQuit(), 
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		event->ignore();
	} else {
		QMainWindow::closeEvent(event);
	}
}

void AppFrame::openGeo3DML() {
	QString filePath = QFileDialog::getOpenFileName(this, Text::menuOpenGeo3DML(), QString(), Text::filterOfGeo3DMLFiles());
	if (filePath.isEmpty()) {
		return;
	}

	geo3dml::Object* g3dObject = nullptr;
	g3dvtk::ObjectFactory g3dVtkFactory;
	g3dxml::XMLReader xmlReader(&g3dVtkFactory);
	BusyCursor::BeginWaiting();
	g3dObject = xmlReader.LoadXMLFile(filePath.toUtf8().constData());
	BusyCursor::EndWaiting();
	if (g3dObject == NULL) {
		QMessageBox::critical(this, this->windowTitle(), QString::fromUtf8(xmlReader.Error().c_str()));
	} else {
		BusyCursor waiting;
		geo3dml::Model* model = dynamic_cast<geo3dml::Model*>(g3dObject);
		if (model != NULL) {
			projectPanel_->appendG3DModel(model, true);
		} else {
			geo3dml::Project* project = dynamic_cast<geo3dml::Project*>(g3dObject);
			if (project != NULL) {
				project->BindFeatureClassesToLayers(&g3dVtkFactory);
				int numOfMaps = project->GetMapCount();
				while (project->GetModelCount() > 0) {
					geo3dml::Model* model = project->RemoveModelAt(0);
					projectPanel_->appendG3DModel(model, numOfMaps == 0);
				}
				while (project->GetMapCount() > 0) {
					geo3dml::Map* map = project->RemoveMapAt(0);
					projectPanel_->appendG3DMap(map);
				}
			}
			delete g3dObject;
		}
		projectPanel_->expandStructureModelTree();
		Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
	}
}

void AppFrame::openDrillLog() {

}

void AppFrame::closeStructureModel() {
	QMessageBox::StandardButton btn = QMessageBox::warning(this, Text::menuCloseStructureModel(), Text::confirmToCloseStructureModel(), 
		QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::No, QMessageBox::StandardButton::No);
	if (btn != QMessageBox::StandardButton::Yes) {
		return;
	}
	BusyCursor waiting;
	projectPanel_->closeStructureModel();
	Events::PostEvent(Events::Type::ResetAndUpdateScene, this);
}

void AppFrame::quit() {
	close();
}

bool AppFrame::event(QEvent* event) {
	switch ((int)(event->type())) {
	case Events::Type::UpdateScene: {
		BusyCursor waiting;
		renderWidget_->render();
		return true;
	}
	case Events::Type::ResetAndUpdateScene: {
		BusyCursor waiting;
		renderWidget_->resetAndRender();
		return true;
	}
	default:
		break;
	}
	return QMainWindow::event(event);
}
