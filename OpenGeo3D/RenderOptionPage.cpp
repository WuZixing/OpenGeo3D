#include "RenderOptionPage.h"
#include <vtkProperty.h>
#include <geo3dml/Annotation.h>
#include <geo3dml/CornerPointGrid.h>
#include <geo3dml/LineString.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/Point.h>
#include <geo3dml/TIN.h>
#include <geo3dml/UniformGrid.h>
#include <geo3dml/GTPVolume.h>
#include "Events.h"
#include "Text.h"

RenderOptionPage::RenderOptionPage(QWidget* parent) : QtTreePropertyBrowser(parent) {
	setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
	setAlternatingRowColors(false);
	setHeaderVisible(false);
	g3dActor_ = nullptr;
	propManager_ = new QtVariantPropertyManager(this);
	QtVariantEditorFactory* propFactory = new QtVariantEditorFactory(this);
	setFactoryForManager(propManager_, propFactory);
}

RenderOptionPage::~RenderOptionPage() {

}

void RenderOptionPage::setCurrentItem(QTreeWidgetItem* item) {
	clear();
	propManager_->clear();
	if (item == nullptr) {
		return;
	}
	void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
	geo3dml::Object* g3dObject = static_cast<geo3dml::Object*>(ptr);
	if (g3dObject == nullptr) {
		return;
	}

	disconnectNotification();

	geo3dml::Project* g3dProject = dynamic_cast<geo3dml::Project*>(g3dObject);
	if (g3dProject != nullptr) {
		setCurrentItemAsG3DProject(g3dProject);
	} else {
		geo3dml::Map* g3dMap = dynamic_cast<geo3dml::Map*>(g3dObject);
		if (g3dMap != nullptr) {
			setCurrentItemAsG3DMap(g3dMap);
		} else {
			geo3dml::Layer* g3dLayer = dynamic_cast<geo3dml::Layer*>(g3dObject);
			if (g3dLayer != nullptr) {
				setCurrentItemAsG3DLayer(g3dLayer);
			} else {
				geo3dml::Actor* g3dActor = dynamic_cast<geo3dml::Actor*>(g3dObject);
				if (g3dActor != nullptr) {
					setCurrentItemAsG3DActor(g3dActor);
				}
			}
		}
	}

	update();

	connectNotification();
}

void RenderOptionPage::setCurrentItemAsG3DProject(geo3dml::Project* g3dProject) {

}

void RenderOptionPage::setCurrentItemAsG3DMap(geo3dml::Map* g3dMap) {

}

void RenderOptionPage::setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer) {

}

void RenderOptionPage::setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor) {
	geo3dml::Geometry* g3dGeometry = g3dActor->GetBindingGeometry();
	if (g3dGeometry == nullptr) {
		return;
	}

	QString geoClassName = Text::nameOfClassUnknown();
	geo3dml::TIN* tin = nullptr;
	geo3dml::UniformGrid* uniformGrid = nullptr;
	geo3dml::CornerPointGrid* cornerGrid = nullptr;
	geo3dml::Point* point = nullptr;
	geo3dml::LineString* lineString = nullptr;
	geo3dml::Annotation* annotation = nullptr;
	geo3dml::MultiPoint* mPoint = nullptr;
	geo3dml::GTPVolume* gtpGrid = nullptr;
	tin = dynamic_cast<geo3dml::TIN*>(g3dGeometry);
	if (tin != nullptr) {
		geoClassName = Text::nameOfClassG3DTIN();
	} else {
		uniformGrid = dynamic_cast<geo3dml::UniformGrid*>(g3dGeometry);
		if (uniformGrid != nullptr) {
			geoClassName = Text::nameOfClassG3DUniformGrid();
		} else {
			cornerGrid = dynamic_cast<geo3dml::CornerPointGrid*>(g3dGeometry);
			if (cornerGrid != nullptr) {
				geoClassName = Text::nameOfClassG3DCornerGrid();
			} else {
				point = dynamic_cast<geo3dml::Point*>(g3dGeometry);
				if (point != nullptr) {
					geoClassName = Text::nameOfClassG3DPoint();
				} else {
					lineString = dynamic_cast<geo3dml::LineString*>(g3dGeometry);
					if (lineString != nullptr) {
						geoClassName = Text::nameOfClassG3DLineString();
					} else {
						annotation = dynamic_cast<geo3dml::Annotation*>(g3dGeometry);
						if (annotation != nullptr) {
							geoClassName = Text::nameOfClassG3DAnnotation();
						} else {
							mPoint = dynamic_cast<geo3dml::MultiPoint*>(g3dGeometry);
							if (mPoint != nullptr) {
								geoClassName = Text::nameOfClassG3DMPoint();
							} else {
								gtpGrid = dynamic_cast<geo3dml::GTPVolume*>(g3dGeometry);
								if (gtpGrid != nullptr) {
									geoClassName = Text::nameOfClassG3DGTPVolume();
								}
							}
						}
					}
				}
			}
		}
	}

	// basic info
	QtVariantProperty* propBasicInfo = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfBasicMetaInfo());
	addProperty(propBasicInfo);
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfClassName());
	propItem->setValue(geoClassName);
	propItem->setEnabled(false);
	propBasicInfo->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfLOD());
	propItem->setValue(g3dGeometry->GetLODLevel());
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);
	// render option
	g3dActor_ = dynamic_cast<g3dvtk::Actor*>(g3dActor);
	if (g3dActor_ == nullptr || !g3dActor_->GetVTKProp()->IsA("vtkActor")) {
		return;
	}
	vtkProperty* renderProp = ((vtkActor*)g3dActor_->GetVTKProp())->GetProperty();
	QtVariantProperty* propRenderOption = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfRenderOption());
	double r = 0, g = 0, b = 0;
	renderProp->GetColor(r, g, b);
	QColor color = QColor::fromRgbF(r, g, b, renderProp->GetOpacity());
	QtVariantProperty* propColor = propManager_->addProperty(QMetaType::Type::QColor, Text::labelOfColor());
	propColor->setValue(color);
	propRenderOption->addSubProperty(propColor);
	QStringList repModes;
	repModes << Text::nameOfRepresentationModePoints() << Text::nameOfRepresentationModeWireframe() << Text::nameOfRepresentationModeSurface();
	propItem = propManager_->addProperty(QtVariantPropertyManager::enumTypeId(), Text::labelOfRepresentationMode());
	propItem->setAttribute(QLatin1String("enumNames"), repModes);
	propItem->setValue(renderProp->GetRepresentation());
	propRenderOption->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::Bool, Text::labelOfDrawEdge());
	propItem->setValue(renderProp->GetEdgeVisibility());
	propRenderOption->addSubProperty(propItem);

	QtBrowserItem* item = addProperty(propRenderOption);
	QList<QtBrowserItem*> children = item->children();
	for (QtBrowserItem* item : children) {
		if (item->property() == propColor) {
			setExpanded(item, false);
			break;
		}
	
	}
}

void RenderOptionPage::OnOptionChanged(QtProperty* prop, const QVariant& value) {
	vtkProperty* renderProp = nullptr;
	if (g3dActor_ != nullptr && g3dActor_->GetVTKProp()->IsA("vtkActor")) {
		renderProp = ((vtkActor*)g3dActor_->GetVTKProp())->GetProperty();
	}
	if (renderProp == nullptr) {
		return;
	}

	QString propName = prop->propertyName();
	if (propName == Text::labelOfColor()) {
		QColor color = value.value<QColor>();
		renderProp->SetColor(color.redF(), color.greenF(), color.blueF());
		renderProp->SetOpacity(color.alphaF());
		Events::PostEvent(Events::Type::UpdateScene);
	} else if (propName == Text::labelOfRepresentationMode()) {
		switch (value.toInt()) {
		case 0:
			renderProp->SetRepresentationToPoints();
			break;
		case 1:
			renderProp->SetRepresentationToWireframe();
			break;
		case 2:
			renderProp->SetRepresentationToSurface();
			break;
		default:
			break;
		}
		Events::PostEvent(Events::Type::UpdateScene);
	} else if (propName == Text::labelOfDrawEdge()) {
		renderProp->SetEdgeVisibility(value.toBool());
		Events::PostEvent(Events::Type::UpdateScene);
	}
}

void RenderOptionPage::connectNotification() {
	connect(propManager_, &QtVariantPropertyManager::valueChanged, this, &RenderOptionPage::OnOptionChanged);
}

void RenderOptionPage::disconnectNotification() {
	disconnect(propManager_, &QtVariantPropertyManager::valueChanged, this, &RenderOptionPage::OnOptionChanged);
}
