#include "CRSSet.h"
#include <QtCore/QRegularExpression>

CRSSet::CRSSet() {

}

QStringList CRSSet::availableCRSNames() {
	QStringList names;
	names << QStringLiteral("Unknown") 
		// << QStringLiteral("EPSG:4479 (China Geodetic Coordinate System 2000)") 
		<< QStringLiteral("EPSG:4490 (China Geodetic Coordinate System 2000)")
		<< QStringLiteral("EPSG:4326 (WGS 84)");
	return names;
}

int CRSSet::EPSGCodeOfCRS(const QString& crsName) {
	QRegularExpression regExp(QStringLiteral("^EPSG:(?<code>\\d+)"));
	QRegularExpressionMatch match = regExp.match(crsName);
	if (match.hasMatch()) {
		bool status = false;
		int code = match.captured(1).toInt(&status);
		if (status) {
			return code;
		}
	}
	return -1;
}

bool CRSSet::loadCRS(OGRSpatialReference& crs, const QString& crsName) {
	int code = EPSGCodeOfCRS(crsName);
	if (code >= 0) {
		return loadCRS(crs, code);
	} else {
		return false;
	}
}

bool CRSSet::loadCRS(OGRSpatialReference& crs, int EPSGCode) {
	switch (EPSGCode) {
	case 4326:
		return crs.SetWellKnownGeogCS("WGS84") == OGRERR_NONE;
	case 4490: {
		OGRErr err = crs.importFromWkt(
			"GEOGCS[\"China Geodetic Coordinate System 2000\", \
			  DATUM[\"China_2000\", \
			    SPHEROID[\"CGCS2000\", 6378137, 298.257222101, \
			      AUTHORITY[\"EPSG\", \"1024\"]], \
			    AUTHORITY[\"EPSG\", \"1043\"]], \
			  PRIMEM[\"Greenwich\", 0, \
			    AUTHORITY[\"EPSG\", \"8901\"]], \
			  UNIT[\"degree\", 0.0174532925199433, \
			    AUTHORITY[\"EPSG\", \"9122\"]], \
			  AUTHORITY[\"EPSG\", \"4490\"]]"
		);
		return err == OGRERR_NONE;
	}
	case 4479: {
		SRS_PT_TRANSVERSE_MERCATOR;
		OGRErr err = crs.SetProjCS(
			"GEOCCS[\"China Geodetic Coordinate System 2000\", \
			  DATUM[\"China_2000\", \
			    SPHEROID[\"CGCS2000\", 6378137, 298.257222101, \
			      AUTHORITY[\"EPSG\", \"1024\"]], \
			    AUTHORITY[\"EPSG\", \"1043\"]], \
			  PRIMEM[\"Greenwich\", 0, \
			    AUTHORITY[\"EPSG\", \"8901\"]], \
			  UNIT[\"metre\", 1, \
			    AUTHORITY[\"EPSG\", \"9001\"]], \
			  AXIS[\"Geocentric X\", OTHER], \
			  AXIS[\"Geocentric Y\", OTHER], \
			  AXIS[\"Geocentric Z\", NORTH], \
			  AUTHORITY[\"EPSG\", \"4479\"]]"
		);
		return err == OGRERR_NONE;
	}
	default:
		return crs.importFromEPSG(EPSGCode) != OGRERR_NONE;
	}
}
