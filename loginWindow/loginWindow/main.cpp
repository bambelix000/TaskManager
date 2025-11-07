#include <QApplication>
#include "loginWindow.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    loginWindow w;
    w.show();
    return a.exec();
}
