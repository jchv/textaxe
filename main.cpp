#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QDirIterator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication::setApplicationDisplayName("TextAxe");
    QApplication::setApplicationName("TextAxe");
    QApplication::setApplicationVersion("0.2");
    QApplication::setOrganizationName("jchadwick");
    QApplication::setOrganizationDomain("jchadwick.net");

    QApplication a(argc, argv);
    QApplication::setWindowIcon(QIcon(":/icons/icon.ico"));

    QDirIterator it(":/fonts/", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        auto fontName = it.next();
        if(QFontDatabase::addApplicationFont(fontName) == -1)
            qDebug() << "Error loading font " << fontName;
        else
            qDebug() << "Loaded bundled font" << fontName;
    }

    MainWindow w;
    w.show();

    return a.exec();
}
