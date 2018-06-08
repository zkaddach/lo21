#include "autocell1d.h"
#include "automate1d.h"

AutoCell1D::AutoCell1D(Autocell *mere, QWidget* parent) : QWidget(parent), m_mere(mere) {
    //Initialization of the "Numéro" SpinBox
    numl = new QLabel("Numéro");
    num = new QSpinBox;
    num->setRange(0,256);
    num->setValue(0);
    //Initialization of the "Numéro" VLayout
    numc = new QVBoxLayout;
    numc->addWidget(numl);
    numc->addWidget(num);
    //Initialization of the "Configuration" HLayout
    numeroc = new QHBoxLayout;
    numeroc->addLayout(numc);
    //Initialization of the "Binary" Labels
    numeroBitl[0] = new QLabel("111");
    numeroBitl[1] = new QLabel("110");
    numeroBitl[2] = new QLabel("101");
    numeroBitl[3] = new QLabel("100");
    numeroBitl[4] = new QLabel("011");
    numeroBitl[5] = new QLabel("010");
    numeroBitl[6] = new QLabel("001");
    numeroBitl[7] = new QLabel("000");
    //Initialization of the 0,1 validator
    zeroOneValidator = new QIntValidator;
    zeroOneValidator->setRange(0,1);
    //Initialization of the Binary inputs
    for(int i = 0; i < 8 ; i++){
        //Initialization of each input
        numeroBit[i] = new QLineEdit;
        numeroBit[i]->setValidator(zeroOneValidator);
        numeroBit[i]->setText("0");
        numeroBit[i]->setFixedWidth(20);
        //Initializatoin of each "Binary VLayout" with input and label
        bitc[i] = new QVBoxLayout;
        bitc[i]->addWidget(numeroBitl[i]);
        bitc[i]->addWidget(numeroBit[i]);
        //Update to the "Configuration" Hlayout
        numeroc->addLayout(bitc[i]);
        //Connection of each Binary input to the "Numéro" SpinBox
        connect(numeroBit[i], SIGNAL(textChanged(QString)),this,SLOT(synchronizeNumBitToNum(QString)));
    }
    //Connection of SpinBox with the Binary inputs
    connect(num, SIGNAL(valueChanged(int)),this,SLOT(synchronizeNumToNumBit(int)));

    //Q2 :
    //Initialization of the grid "depart"
    depart = new QTableWidget(1,25,this);
    unsigned int largeur = 25;
    depart->setFixedSize(dimension*largeur, largeur);
    depart->horizontalHeader()->setVisible(false);
    depart->verticalHeader()->setVisible(false);
    depart->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    depart->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Initialization of each item of the grid "depart"
    for(unsigned int i = 0; i < dimension; i++) {
            depart->setColumnWidth(i, largeur);
            depart->setItem(0, i, new QTableWidgetItem(""));
            depart->item(0, i)->setBackgroundColor("white");
            depart->item(0, i)->setTextColor("white");
            depart->item(0, i)->setText("0");
        }
    //On doubleClicked SIGNAL the cell is Activated
    connect(depart, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(cellActivated(QModelIndex)));
    //Initialization of the "Final" Layout
    couche = m_mere->getLayout();
    couche->addLayout(numeroc);
    couche->addWidget(depart);
    //Initializaition of the button Simulation !
    simu = new QPushButton("Simulation");
    couche->addWidget(simu);
    connect(simu, SIGNAL(clicked(bool)), this, SLOT(launchSimulation(bool)));
    //Update of the layouts to the Window


}
//Update the Binary inputs when the SpinBox "Numéro" is modified
void AutoCell1D::synchronizeNumToNumBit(int i) {

    std::string numBit = NumToNumBit(i);
    for(int j = 0; j < 8; j++){
        numeroBit[j]->setText(QString(numBit[j]));
    }
}
//Update the SpinBox "Numéro" when the Binary inputs is modified
void AutoCell1D::synchronizeNumBitToNum(const QString& s){

    if(s == ""){
        return;
    }
    std::string bit("");
    for(int i = 0; i < 8; i++){
        bit += numeroBit[i]->text().toStdString();
    }
    num->setValue(NumBitToNum(bit));
}
//Activate the cell
void AutoCell1D::cellActivated(const QModelIndex& s){
    if(depart->item(0, s.column())->text() == "0"){
        depart->item(0, s.column())->setBackgroundColor("black");
        depart->item(0, s.column())->setTextColor("black");
        depart->item(0, s.column())->setText("1");
    }
    else if(depart->item(0, s.column())->text() == "1"){
        depart->item(0, s.column())->setBackgroundColor("white");
        depart->item(0, s.column())->setTextColor("white");
        depart->item(0, s.column())->setText("0");
    }

}
//Launch the simulation of 25 tests
void AutoCell1D::launchSimulation(const bool&){
    //Set up of the initial state
    Etat e(dimension);
    for(int i = 0; i < dimension; i++){
        if(depart->item(0, i)->text() == "0")
            e.setCellule(i, false);
        else
            e.setCellule(i, true);
    }
    //Set up of the Automate
    const Automate &a = AutomateManager::getAutomateManager().getAutomate(num->value());
    //Set up of the Simulator
    Simulateur s(a,e,25);
    //Configuration of the Result grid
    result->setFixedSize(dimension*25, 20*dimension);
    result->horizontalHeader()->setVisible(false);
    result->verticalHeader()->setVisible(false);
    result->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    result->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //Initialization of each item on the grid
    for(int j = 0; j < dimension;j++){
        //Increment the next simulation step
        s.next();
        for(unsigned int i = 0; i < dimension; i++) {
                result->setColumnWidth(i, 25);
                result->setItem(j, i, new QTableWidgetItem(""));
                if(s.dernier().getCellule(i) == false){
                    result->item(j, i)->setBackgroundColor("white");
                    result->item(j, i)->setTextColor("white");
                    result->item(j, i)->setText("0");
                }
                else{
                    result->item(j, i)->setBackgroundColor("black");
                    result->item(j, i)->setTextColor("black");
                    result->item(j, i)->setText("1");
                }

        }
    }


    couche->addWidget(result);
}
