// this file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
// please see LICENSE.txt for more details and licensing issues
// copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include <QApplication>
#include <QMessageBox>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QStyleFactory>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
	app.setStyle(QStyleFactory::create("fusion"));
	/*
    QString sPath=QCoreApplication::applicationDirPath();

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("Astree_" + QLocale::system().name(),sPath);
    app.installTranslator(&myappTranslator);
	*/
    MainWindow w;
	w.resize(5000, 5000); // showMaximized ugly fix as in: https://stackoverflow.com/questions/27157312/qt-showmaximized-not-working-in-windows
    w.showMaximized();

    if (argc==2)
    {
        w.load_file(std::string(argv[1]));
    }

    return app.exec();
}
