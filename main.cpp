#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include "JobProducer.h"
#include "Logger.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QApplication::setApplicationName("BatchProcessor");
    QApplication::setOrganizationName("CellphyLab");
    QApplication::setOrganizationDomain("CellphyLab.com");
    qInstallMessageHandler(&Logger::qtMessageHandler);

    Logger::instance()->log(
                QString("%1 %2 Version %3 Launched")
                  .arg(QApplication::organizationName())
                  .arg(QApplication::applicationName())
                  .arg(QApplication::applicationVersion()));

    app.setWindowIcon( QIcon(":/icons/appicon.png") );

    const QString settingsFile = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)).absoluteFilePath("settings.ini");
    QSettings * m_settings = new QSettings(settingsFile, QSettings::IniFormat, qApp);

    qmlRegisterUncreatableType<JobModel>("BatchProcessor", 1, 0, "JobModel", "QML should not initialize this Class");

    QFontDatabase fontDatabase;
        if (fontDatabase.addApplicationFont(":/fonts/fontello.ttf") == -1)
            qDebug() << "Failed to load fontello.ttf";


    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QQuickView view;

    view.engine()->rootContext()->setContextProperty("producer", JobProducer::instance());
    view.engine()->rootContext()->setContextProperty("settingPath", m_settings->fileName());

    view.setSource(url);
    view.setMinimumSize(QSize(640, 480));
    view.setTitle("Batch Process");
    view.show();

    return app.exec();
}
