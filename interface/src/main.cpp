#include <QApplication> // Use QApplication, não QCoreApplication
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // QApplication é a classe de aplicação para GUI
    QApplication a(argc, argv);

    // Cria a janela principal e a exibe
    MainWindow w;
    w.show();

    // Inicia o loop de eventos da aplicação
    return a.exec();
}
