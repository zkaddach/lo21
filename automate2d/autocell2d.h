#ifndef AUTOCELL2D_H
#define AUTOCELL2D_H

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
#include <QTime>
#include <QCoreApplication>
#include <QComboBox>

#include "autocell.h"

class Autocell;
class AutoCell2D;

class Grille : public QWidget {
  Q_OBJECT
    unsigned int m_nbLigne ;
    unsigned int m_nbColone ;
    unsigned int m_tailleCase ;
    QTableWidget *depart;
    QTime attente;
    const AutoCell2D& mere;

signals:
    void gridChanged(const bool& );
public:
   Grille(const AutoCell2D& fen, QWidget *parent = nullptr, unsigned int nbLigne = 10, unsigned int nbColone = 10, unsigned int tailleCase = 25);
private slots:
   void cellActivated(const QModelIndex& s);
   void launchSimulation( const bool&);

   friend class AutoCell2D;
};

class AutoCell2D : public QWidget {
    Q_OBJECT
    QSpinBox*  m_nbMinVoisin;
    QSpinBox* m_nbMaxVoisin;
    QSpinBox* m_nbNaissance;
    QSpinBox* m_timeSteps;
    QLabel* m_lMinVoisin;
    QLabel* m_lMaxVoisin;
    QLabel* m_lNaissance;
    QLabel* m_lTimeSteps;
    QVBoxLayout* m_automateConfig[4];
    QHBoxLayout* m_coucheConfig;

    QPushButton *simu;
    QPushButton *m_stopSimu;


    QVBoxLayout* m_layout;

    QTime attente;

    Grille *g;
    bool m_playSimu;
    Autocell *m_mere;

public:
    AutoCell2D(Autocell *mere = nullptr, QWidget *parent = nullptr);
    bool getPlaySimu() {return m_playSimu;}
    unsigned int getNbMinVoisin() const {return m_nbMinVoisin->value();}
    unsigned int getNbMaxVoisin() const {return m_nbMaxVoisin->value();}
    unsigned int getNbNaissance() const {return m_nbNaissance->value();}
private slots:
    void pauseSimulation(const bool&);
    void startSimulation(const bool&);
};




#endif // AUTOCELL2D_H
