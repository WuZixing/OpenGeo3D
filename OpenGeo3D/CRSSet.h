#pragma once

#include <QtCore/QStringList>
#include <ogr_spatialref.h>

class CRSSet {
public:
	static QStringList availableCRSNames();
	static int EPSGCodeOfCRS(const QString& crsName);
	static bool loadCRS(OGRSpatialReference& crs, const QString& crsName);
	static bool loadCRS(OGRSpatialReference& crs, int EPSGCode);

private:
	CRSSet();
};
