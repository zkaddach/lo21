#ifndef AutoCell1D_H
#define AutoCell1D_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QIntValidator>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QTableWidget>
#include <QHeaderView>

#include <QPushButton>

#include "../autocell.h"

class Autocell;

class AutoCell1D : public QWidget{
    Q_OBJECT
    QSpinBox* num; // numéro
    QLineEdit* numeroBit[8]; // un QLineEdit par bit
    QLabel* numl;
    QLabel* numeroBitl[8];
    QVBoxLayout* numc;
    QVBoxLayout* bitc[8];
    QHBoxLayout* numeroc;
    QIntValidator* zeroOneValidator;
    //Q2
    QTableWidget *depart;
    unsigned int dimension = 25;
    QVBoxLayout *couche;
    //Q3
    QPushButton *simu;
    QTableWidget *result = new QTableWidget(25,25,this);
    Autocell *m_mere;
public:
     AutoCell1D(Autocell *mere = nullptr, QWidget* parent = nullptr);
private slots:
    void synchronizeNumToNumBit(int i);
    void synchronizeNumBitToNum(const QString& s);
    void cellActivated(const QModelIndex& s);
    void launchSimulation(const bool&);
};



#endif // AutoCell1D_H
