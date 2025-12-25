// #include "EntryPoint.h"

#ifdef USE_QT

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

void initQt(int argc, char **argv)
{
    static QApplication app(argc, argv);
}

#endif