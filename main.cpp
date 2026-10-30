#include "calculator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Calculator w; // 将 MainWindow 替换为自定义的 Calculator
    w.show();
    return a.exec();
}
