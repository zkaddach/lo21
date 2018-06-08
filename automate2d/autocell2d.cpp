#include "autocell2d.h"
#include "automate2d.h"



AutoCell2D::AutoCell2D(Autocell *mere, QWidget *parent ) : m_mere(mere), QWidget(parent) {

    m_layout = m_mere->getLayout();

    //Initialization of the labels for the automate config (SpinBox)
    m_lMinVoisin = new QLabel("Min");
    m_nbMinVoisin = new QSpinBox;
    m_nbMinVoisin->setValue(2);
    m_lMaxVoisin = new QLabel("Max");
    m_nbMaxVoisin = new QSpinBox;
    m_nbMaxVoisin->setValue(3);
    m_lNaissance = new QLabel("Naissance");
    m_nbNaissance = new QSpinBox;
    m_nbNaissance->setValue(3);
    //Configuration of the layouts associated
    m_automateConfig[0] = new QVBoxLayout;
    m_automateConfig[0]->addWidget(m_lMinVoisin);
    m_automateConfig[0]->addWidget(m_nbMinVoisin);
    m_automateConfig[1] = new QVBoxLayout;
    m_automateConfig[1]->addWidget(m_lMaxVoisin);
    m_automateConfig[1]->addWidget(m_nbMaxVoisin);
    m_automateConfig[2] = new QVBoxLayout;
    m_automateConfig[2]->addWidget(m_lNaissance);
    m_automateConfig[2]->addWidget(m_nbNaissance);

    //Configuration of time steps duration :
    m_timeSteps = new QSpinBox;
    m_lTimeSteps = new QLabel("Steps");
    m_automateConfig[3] = new QVBoxLayout;
    m_automateConfig[3]->addWidget(m_lTimeSteps);
    m_automateConfig[3]->addWidget(m_timeSteps);

    //Initialization coucheConfig
    m_coucheConfig = new QHBoxLayout;
    for(int i = 0; i < 4; i++)
        m_coucheConfig->addLayout(m_automateConfig[i]);


    m_layout->addLayout(m_coucheConfig);

    //Adding the Grid
    g = new Grille(*this);
    m_layout->addWidget(g->depart);
    //On doubleClicked SIGNAL the cell is Activated
    connect(g->depart, SIGNAL(doubleClicked(QModelIndex)), g, SLOT(cellActivated(QModelIndex)));

    //Initialize Play Simu to false
    m_playSimu = false;

    //Adding button Start Simulation
    simu = new QPushButton("Start Simulation");
    m_layout->addWidget(simu);
    connect(simu, SIGNAL(clicked(bool)), this, SLOT(startSimulation(bool)));

    //Adding button Pause Simulation
    m_stopSimu = new QPushButton("Pause");
    m_layout->addWidget(m_stopSimu);
    connect(m_stopSimu, SIGNAL(clicked(bool)), this, SLOT(pauseSimulation(bool)));



}



void Grille::launchSimulation( const bool&){
        //Set Start State
        Etat2D e(m_nbColone, m_nbLigne);
        for(unsigned int j = 0; j < m_nbLigne; j++){
            for(unsigned int i = 0; i < m_nbColone; i++) {
                if(depart->item(j, i)->text() == "0")
                    e.setCellule(i,j, false);
                else
                    e.setCellule(i, j, true);
            }
        }
        //Define Automate and Simualteur
        Automate2D a(mere.getNbMaxVoisin(), mere.getNbMinVoisin(), mere.getNbNaissance());
        Simulateur2D s(a, e, 25); //Nb de ETAT Stocké

        //Launch next step of the simulation
        s.next();

        //Update the grid state
        for(unsigned int j = 0; j < m_nbLigne; j++){
            for(unsigned int i = 0; i < m_nbColone; i++) {
                if(s.getDernierEtat().getCellule(i,j) == false){
                    depart->item(j, i)->setBackgroundColor("white");
                    depart->item(j, i)->setTextColor("white");
                    depart->item(j, i)->setText("0");
                }
                else{
                    depart->item(j, i)->setBackgroundColor("black");
                    depart->item(j, i)->setTextColor("black");
                    depart->item(j, i)->setText("1");
                }
            }
        }
        QCoreApplication::processEvents();
}

//Pause the simulation
void AutoCell2D::pauseSimulation(const bool&){
     if(m_playSimu == true){
         m_playSimu = false;
         m_stopSimu->setText("Unpause");
     }
     else{
         m_playSimu = true;
         m_stopSimu->setText("Pause");
         startSimulation(true);
     }
}


//Launch Simulation
void AutoCell2D::startSimulation(const bool&){
   m_playSimu = true;

   while(m_playSimu){
       g->launchSimulation(true);
       QCoreApplication::processEvents();
       attente = QTime::currentTime().addSecs(1);
       while(attente > QTime::currentTime()) {QCoreApplication::processEvents();}
   }
}


//Activate the cell
void Grille::cellActivated(const QModelIndex& s){
    if(depart->item(s.row(), s.column())->text() == "0"){
        depart->item(s.row(), s.column())->setBackgroundColor("black");
        depart->item(s.row(), s.column())->setTextColor("black");
        depart->item(s.row(), s.column())->setText("1");
    }
    else if(depart->item(s.row(), s.column())->text() == "1"){
        depart->item(s.row(), s.column())->setBackgroundColor("white");
        depart->item(s.row(), s.column())->setTextColor("white");
        depart->item(s.row(), s.column())->setText("0");
    }

}

Grille::Grille(const AutoCell2D& fen, QWidget *parent, unsigned int nbLigne , unsigned int nbColone , unsigned int tailleCase) : QWidget(parent), mere(fen), m_nbLigne(nbLigne), m_nbColone(nbColone), m_tailleCase(tailleCase){
    //Initialization of the grid
    depart = new QTableWidget(m_nbLigne,m_nbColone,this);
    depart->setFixedSize(m_tailleCase * m_nbColone, m_tailleCase * m_nbLigne);
    depart->horizontalHeader()->setVisible(false);
    depart->verticalHeader()->setVisible(false);
    depart->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    depart->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for(unsigned int j = 0; j < m_nbLigne; j++){
        for(unsigned int i = 0; i < m_nbColone; i++) {
                    depart->setColumnWidth(i, m_tailleCase);
                    depart->setRowHeight(j,m_tailleCase);
                    depart->setItem(j, i, new QTableWidgetItem(""));
                    depart->item(j, i)->setBackgroundColor("white");
                    depart->item(j, i)->setTextColor("white");
                    depart->item(j, i)->setText("0");
        }
    }

}
