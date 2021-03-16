#pragma once

#include <QtWidgets/QTreeWidget>
#include <QtTreePropertyBrowser>
#include <QtVariantPropertyManager>
#include <geo3dml/Project.h>
#include <g3dvtk/Actor.h>

class RenderOptionPage : public QtTreePropertyBrowser {
	Q_OBJECT

public:
	RenderOptionPage(QWidget* parent);
	virtual ~RenderOptionPage();

	void setCurrentItem(QTreeWidgetItem* item);

private slots:
	void OnOptionChanged(QtProperty* prop, const QVariant& value);

private:
	void setCurrentItemAsG3DProject(geo3dml::Project* g3dProject);
	void setCurrentItemAsG3DMap(geo3dml::Map* g3dMap);
	void setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer);
	void setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor);

	void connectNotification();
	void disconnectNotification();

private:
	QtVariantPropertyManager* propManager_;
	g3dvtk::Actor* g3dActor_;

	const QString attriReadOnly_ = QStringLiteral("readOnly");
};
