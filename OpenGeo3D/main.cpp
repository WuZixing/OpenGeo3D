#include "MainWindow.h"

#include <QSurfaceFormat>
#include <QTranslator>
#include <QVTKApplication.h>
#include <QVTKOpenGLNativeWidget.h>
#include "Strings.h"

int main(int argc, char *argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QVTKApplication app(argc, argv);
    app.setApplicationName(Strings::appName());
    app.setApplicationDisplayName(Strings::appName());
    app.setApplicationVersion(QString::fromUtf8(APP_VERSION));

    QTranslator translator;
    translator.load("translations/qt_zh_CN.qm");
    app.installTranslator(&translator);

    MainWindow w;
    w.showMaximized();
    
    return app.exec();
}
