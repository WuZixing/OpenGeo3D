#include "AppFrame.h"
#include <QtWidgets/QApplication>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtGui/QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>

int main(int argc, char* argv[]) {
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

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
    
    AppFrame frame;
    frame.showMaximized();

    return app.exec();
}