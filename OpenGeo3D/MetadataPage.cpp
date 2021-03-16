#include "MetadataPage.h"
#include <geo3dml/Annotation.h>
#include <geo3dml/CornerPointGrid.h>
#include <geo3dml/LineString.h>
#include <geo3dml/MultiPoint.h>
#include <geo3dml/Point.h>
#include <geo3dml/TIN.h>
#include <geo3dml/UniformGrid.h>
#include "Text.h"

MetadataPage::MetadataPage(QWidget* parent) : QtTreePropertyBrowser(parent) {
	g3dObject_ = nullptr;
	propManager_ = new QtVariantPropertyManager(this);
	QtVariantEditorFactory* propFactory = new QtVariantEditorFactory(this);
	setFactoryForManager(propManager_, propFactory);
	setResizeMode(QtTreePropertyBrowser::ResizeMode::Interactive);
	setAlternatingRowColors(false);
	setHeaderVisible(false);
}

MetadataPage::~MetadataPage() {

}

void MetadataPage::setCurrentItem(QTreeWidgetItem* item) {
	clear();
	propManager_->clear();
	if (item == nullptr) {
		return;
	}
	void* ptr = item->data(0, Qt::ItemDataRole::UserRole).value<void*>();
	g3dObject_ = static_cast<geo3dml::Object*>(ptr);
	if (g3dObject_ == nullptr) {
		return;
	}
	geo3dml::Project* g3dProject = dynamic_cast<geo3dml::Project*>(g3dObject_);
	if (g3dProject != nullptr) {
		setCurrentItemAsG3DProject(g3dProject);
	} else {
		geo3dml::Map* g3dMap = dynamic_cast<geo3dml::Map*>(g3dObject_);
		if (g3dMap != nullptr) {
			setCurrentItemAsG3DMap(g3dMap);
		} else {
			geo3dml::Layer* g3dLayer = dynamic_cast<geo3dml::Layer*>(g3dObject_);
			if (g3dLayer != nullptr) {
				setCurrentItemAsG3DLayer(g3dLayer);
			} else {
				geo3dml::Actor* g3dActor = dynamic_cast<geo3dml::Actor*>(g3dObject_);
				if (g3dActor != nullptr) {
					setCurrentItemAsG3DActor(g3dActor);
				}
			}
		}
	}

	update();
}

void MetadataPage::setCurrentItemAsG3DProject(geo3dml::Project* g3dProject) {
	setBasicMetaInfo(Text::labelOfStructureModel(), QString::fromUtf8(g3dProject->GetID().c_str()), g3dProject->GetMapCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dProject->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::setCurrentItemAsG3DMap(geo3dml::Map* g3dMap) {
	setBasicMetaInfo(Text::labelOf3DMap(), QString::fromUtf8(g3dMap->GetID().c_str()), g3dMap->GetLayerCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dMap->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
}

void MetadataPage::setCurrentItemAsG3DLayer(geo3dml::Layer* g3dLayer) {
	setBasicMetaInfo(Text::labelOfLayer(), QString::fromUtf8(g3dLayer->GetID().c_str()), g3dLayer->GetActorCount());
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dLayer->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
	// feature class
	geo3dml::FeatureClass* g3dFeatureClass = g3dLayer->GetBindingFeatureClass();
	QtVariantProperty* propFeatureClass = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfFeatureClass());
	QString strId, strName;
	if (g3dFeatureClass != nullptr) {
		strId = QString::fromUtf8(g3dFeatureClass->GetID().c_str());
		strName = QString::fromUtf8(g3dFeatureClass->GetName().c_str());
	} else {
		strId = QString::fromUtf8(g3dLayer->GetID().c_str());
	}
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(strId);
	propItem->setAttribute(attriReadOnly_, true);
	propFeatureClass->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfName());
	propItem->setValue(strName);
	propItem->setAttribute(attriReadOnly_, true);
	propFeatureClass->addSubProperty(propItem);
	// schema
	QtVariantProperty* propSchema = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfSchema());
	if (g3dFeatureClass != nullptr) {
		for (int i = 0; i < g3dFeatureClass->GetFieldCount(); ++i) {
			const geo3dml::Field& field = g3dFeatureClass->GetFieldAt(i);
			setFieldInfo(propSchema, field, i);
		}
	}
	propFeatureClass->addSubProperty(propSchema);
	QtBrowserItem* groupItem = addProperty(propFeatureClass);
	QList<QtBrowserItem*> children = groupItem->children();
	auto citor = children.cbegin();
	while (citor != children.cend()) {
		if ((*citor)->property() == propSchema) {
			QList<QtBrowserItem*> fieldItems = (*citor)->children();
			for (QtBrowserItem* item : fieldItems) {
				setExpanded(item, false);
			}
			break;
		}
		++citor;
	}
}

void MetadataPage::setCurrentItemAsG3DActor(geo3dml::Actor* g3dActor) {
	setBasicMetaInfo(Text::labelOfActor(), QString::fromUtf8(g3dActor->GetID().c_str()), 0);
	double minX = 0, minY = 0, minZ = 0, maxX = 0, maxY = 0, maxZ = 0;
	g3dActor->GetMinimumBoundingRectangle(minX, minY, minZ, maxX, maxY, maxZ);
	setMBRInfo(minX, minY, minZ, maxX, maxY, maxZ);
	// feature
	setFeatureInfo(g3dActor->GetBindingFeature());
	// geometry
	setGeometryInfo(g3dActor->GetBindingGeometry());
}

void MetadataPage::setBasicMetaInfo(const QString& datasetName, const QString& datasetId, int childrenNumber) {
	QtVariantProperty* propBasicInfo = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfBasicMetaInfo());
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfDataset());
	propItem->setValue(datasetName);
	propItem->setEnabled(false);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfSubNodeNumber());
	propItem->setValue(childrenNumber);
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(datasetId);
	propItem->setAttribute(attriReadOnly_, true);
	propBasicInfo->addSubProperty(propItem);

	addProperty(propBasicInfo);
}

void MetadataPage::setMBRInfo(double minX, double minY, double minZ, double maxX, double maxY, double maxZ) {
	QtVariantProperty* propAABB = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfAABB());
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMinPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(minX, 'f')).arg(QString::number(minY, 'f')).arg(QString::number(minZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);

	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfAABBMaxPoint());
	propItem->setValue(QStringLiteral("(%1, %2, %3)").arg(QString::number(maxX, 'f')).arg(QString::number(maxY, 'f')).arg(QString::number(maxZ, 'f')));
	propItem->setAttribute(attriReadOnly_, true);
	propAABB->addSubProperty(propItem);

	addProperty(propAABB);
}

void MetadataPage::setFieldInfo(QtVariantProperty* parentProp, const geo3dml::Field& field, int index) {
	QtVariantProperty* propField = propManager_->addProperty(QMetaType::Type::QString, QString::number(index + 1));
	propField->setValue(QString::fromUtf8(field.Name().c_str()));
	propField->setAttribute(attriReadOnly_, true);
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Label"));
	propItem->setValue(QString::fromUtf8(field.Label().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("DataType"));
	propItem->setValue(QString::fromUtf8(geo3dml::Field::ValueTypeToName(field.DataType()).c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("UOM"));
	propItem->setValue(QString::fromUtf8(field.Uom().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Definition"));
	propItem->setValue(QString::fromUtf8(field.Definition().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, QStringLiteral("Descriptoin"));
	propItem->setValue(QString::fromUtf8(field.Description().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propField->addSubProperty(propItem);
	propField->setEnabled(false);

	parentProp->addSubProperty(propField);
}

void MetadataPage::setFeatureInfo(geo3dml::Feature* g3dFeature) {
	QtVariantProperty* propFeature = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfFeature());
	addProperty(propFeature);
	if (g3dFeature == nullptr) {
		return;
	}

	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(QString::fromUtf8(g3dFeature->GetID().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propFeature->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfName());
	propItem->setValue(QString::fromUtf8(g3dFeature->GetName().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propFeature->addSubProperty(propItem);
	// fields
	std::vector<std::string>&& fields = g3dFeature->GetFieldNames();
	for (std::string fieldName : fields) {
		const geo3dml::FieldValue* fieldValue = g3dFeature->GetField(fieldName);
		if (fieldValue == nullptr) {
			propItem = propManager_->addProperty(QMetaType::Type::QString, QString::fromUtf8(fieldName.c_str()));
			propItem->setEnabled(false);
		} else {
			switch (fieldValue->ValueType()) {
			case geo3dml::Field::ValueType::Text: {
				propItem = propManager_->addProperty(QMetaType::Type::QString, QString::fromUtf8(fieldName.c_str()));
				propItem->setValue(QString::fromUtf8(fieldValue->GetString().c_str()));
				propItem->setAttribute(attriReadOnly_, true);
				break;
			}
			case geo3dml::Field::ValueType::Integer: {
				propItem = propManager_->addProperty(QMetaType::Type::Int, QString::fromUtf8(fieldName.c_str()));
				propItem->setValue(fieldValue->GetInt());
				propItem->setAttribute(attriReadOnly_, true);
				break;
			}
			case geo3dml::Field::ValueType::Double: {
				propItem = propManager_->addProperty(QMetaType::Type::Double, QString::fromUtf8(fieldName.c_str()));
				propItem->setValue(fieldValue->GetDouble());
				propItem->setAttribute(attriReadOnly_, true);
				break;
			}
			case geo3dml::Field::ValueType::Boolean: {
				propItem = propManager_->addProperty(QMetaType::Type::Bool, QString::fromUtf8(fieldName.c_str()));
				propItem->setValue(fieldValue->GetBool());
				propItem->setAttribute(attriReadOnly_, true);
				break;
			}
			default: {
				propItem = propManager_->addProperty(QMetaType::Type::QString, QString::fromUtf8(fieldName.c_str()));
				propItem->setValue(QString::fromUtf8(geo3dml::Field::ValueTypeToName(fieldValue->ValueType()).c_str()));
				propItem->setEnabled(false);
			}
			}
		}
		propFeature->addSubProperty(propItem);
	}
}

void MetadataPage::setGeometryInfo(geo3dml::Geometry* g3dGeometry) {
	QtVariantProperty* propGeometry = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfGeometry());
	addProperty(propGeometry);
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
							}
						}
					}
				}
			}
		}
	}

	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(QString::fromUtf8(g3dGeometry->GetID().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propGeometry->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfName());
	propItem->setValue(QString::fromUtf8(g3dGeometry->GetName().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propGeometry->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfLOD());
	propItem->setValue(g3dGeometry->GetLODLevel());
	propItem->setAttribute(attriReadOnly_, true);
	propGeometry->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfClassName());
	propItem->setValue(geoClassName);
	propItem->setAttribute(attriReadOnly_, true);
	propGeometry->addSubProperty(propItem);

	if (tin != nullptr) {
		propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfNumberOfVertices());
		propItem->setValue(tin->GetVertexCount());
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
		propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfNumberOfTriangles());
		propItem->setValue(tin->GetTriangleCount());
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	} else if (uniformGrid != nullptr) {
		double x = 0, y = 0, z = 0;
		uniformGrid->GetOrigin(x, y, z);
		QString str = QString::asprintf("(%.6f, %.6f, %.6f)", x, y, z);
		propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfGridOrigin());
		propItem->setValue(str);
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
		uniformGrid->GetSteps(x, y, z);
		str = QString::asprintf("(%.6f, %.6f, %.6f)", x, y, z);
		propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfGridCellSize());
		propItem->setValue(str);
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
		int i = 0, j = 0, k = 0;
		uniformGrid->GetDimensions(i, j, k);
		str = QString::asprintf("(%d, %d, %d)", i, j, k);
		propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfGridCellDimension());
		propItem->setValue(str);
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	} else if (cornerGrid != nullptr) {
		int i = 0, j = 0, k = 0;
		cornerGrid->GetDimensions(i, j, k);
		QString str = QString::asprintf("(%d, %d, %d)", i, j, k);
		propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfGridCellDimension());
		propItem->setValue(str);
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	} else if (lineString != nullptr) {
		propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfNumberOfVertices());
		propItem->setValue(lineString->GetVertexCount());
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	} else if (annotation != nullptr) {
		propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfNumberOfVertices());
		propItem->setValue(annotation->GetPointCount());
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	} else if (mPoint != nullptr) {
		propItem = propManager_->addProperty(QMetaType::Type::Int, Text::labelOfNumberOfVertices());
		propItem->setValue(mPoint->GetPointCount());
		propItem->setAttribute(attriReadOnly_, true);
		propGeometry->addSubProperty(propItem);
	}
	setShapePropertyOfGeometry(propGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Vertex));
	setShapePropertyOfGeometry(propGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Edge));
	setShapePropertyOfGeometry(propGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Face));
	setShapePropertyOfGeometry(propGeometry, g3dGeometry->GetProperty(geo3dml::ShapeProperty::SamplingTarget::Voxel));
}

void MetadataPage::setShapePropertyOfGeometry(QtVariantProperty* parentProp, geo3dml::ShapeProperty* g3dShapeProperty) {
	if (g3dShapeProperty == nullptr) {
		return;
	}
	QString targetComponent = QString::fromUtf8(geo3dml::ShapeProperty::SamplingTargetToName(g3dShapeProperty->TargetComponent()).c_str());
	QtVariantProperty* propGroup = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), targetComponent);
	parentProp->addSubProperty(propGroup);
	QtVariantProperty* propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfId());
	propItem->setValue(QString::fromUtf8(g3dShapeProperty->GetID().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propGroup->addSubProperty(propItem);
	propItem = propManager_->addProperty(QMetaType::Type::QString, Text::labelOfName());
	propItem->setValue(QString::fromUtf8(g3dShapeProperty->Name().c_str()));
	propItem->setAttribute(attriReadOnly_, true);
	propGroup->addSubProperty(propItem);
	// schema
	QtVariantProperty* propSchema = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(), Text::labelOfSchema());
	for (int i = 0; i < g3dShapeProperty->GetFieldCount(); ++i) {
		const geo3dml::Field& field = g3dShapeProperty->GetFieldAt(i);
		setFieldInfo(propSchema, field, i);
	}
	propGroup->addSubProperty(propSchema);
}
