#include "AppFrame.h"
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtGui/QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include "OpenApp.h"

int main(int argc, char* argv[]) {
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    OpenApp app(argc, argv);

    QString path = QLibraryInfo::location(QLibraryInfo::LibraryLocation::TranslationsPath);
    QLocale locale(QLocale::Language::Chinese, QLocale::Country::China);
    QTranslator qtTranslator, appTranslator;
    bool status = qtTranslator.load(locale, QStringLiteral("qt_"), QString(), path);
    if (status) {
        status = app.installTranslator(&qtTranslator);
    }
    /*
    status = appTranslator.load(locale, QStringLiteral("OpenApp_"), QString(), path);
    if (status) {
        status = app.installTranslator(&appTranslator);
    }
    */
    
    app.showWindow();
    return app.exec();
}