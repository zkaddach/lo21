#include "autocell.h"

#include <QApplication>

int main(int argc, char*argv[]){


    QApplication app(argc, argv);
    Autocell fenetre;
    fenetre.show();

    return app.exec();
}
