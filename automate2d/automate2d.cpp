#include "automate2d.h"

//---------------------------------------------Classe Etat 2D----------------------------------------------------------

Etat2D::Etat2D(unsigned int largeur, unsigned int hauteur): m_largeur(largeur), m_hauteur(hauteur){
    m_valeurs = new bool[m_hauteur*m_largeur];
    for (int i = 0; i < m_hauteur; i++){
        for(int j = 0; j < m_largeur; j++){
            m_valeurs[i*(m_largeur-1) + j] = false;
        }
    }
}

Etat2D::Etat2D(const Etat2D& etat) : m_largeur(etat.getLargeur()), m_hauteur(etat.getHauteur()){
    m_valeurs = new bool[m_hauteur*m_largeur];
    for (int i = 0; i < m_hauteur; i++){
        for(int j = 0; j < m_largeur; j++){
            m_valeurs[i*(m_largeur-1) + j] = etat.getCellule(j, i);
        }
    }
}

Etat2D Etat2D::operator=(const Etat2D& etat){
    m_largeur = etat.getLargeur();
    m_hauteur = etat.getHauteur();
    bool *old = m_valeurs;
    m_valeurs = new bool[m_hauteur*m_largeur];
    for (int i = 0; i < m_hauteur; i++){
        for(int j = 0; j < m_largeur; j++){
            m_valeurs[i*(m_largeur-1) + j] = etat.getCellule(j, i);
        }
    }
    delete[] old;
    return *this;
}

Etat2D::~Etat2D(){
    delete[] m_valeurs;
}

unsigned int Etat2D::getLargeur() const {return m_largeur;}

unsigned int Etat2D::getHauteur() const {return m_hauteur;}

bool Etat2D::getCellule(unsigned int x, unsigned int y) const {return m_valeurs[y*(m_largeur-1) + x];}

void Etat2D::setCellule(unsigned int x, unsigned int y, bool valeur){m_valeurs[y*(m_largeur-1) + x] = valeur;}

std::ostream & operator<<(std::ostream & f, const Etat2D & e) {
    for (int i = 0; i < e.getHauteur(); i++){
        for(int j = 0; j < e.getLargeur(); j++){
            if(e.getCellule(j, i)) {
                f << "X";
            } else {
                f << "-";
            }
        }
        f << std::endl;
    }
    return f;
}



//---------------------------------------------Classe Automate 2D----------------------------------------------------------

Automate2D::Automate2D(unsigned int nbMaxVoisin, unsigned int nbMinVoisin, unsigned int nbNaissance) : m_nbMaxVoisin(nbMaxVoisin), m_nbMinVoisin(nbMinVoisin), m_nbNaissance(nbNaissance) {}

unsigned int Automate2D::getNbMinVoisin() const {return m_nbMinVoisin;}

unsigned int Automate2D::getNbMaxVoisin() const {return m_nbMaxVoisin;}

unsigned int Automate2D::getNbNaissance() const {return m_nbNaissance;}

void Automate2D::setNbMinVoisin(unsigned int valeur){m_nbMinVoisin = valeur;}

void Automate2D::setNbMaxVoisin(unsigned int valeur){m_nbMaxVoisin = valeur;}

void Automate2D::setNbNaissance(unsigned int valeur){m_nbMinVoisin = valeur;}

void Automate2D::appliquerTransition(const Etat2D& dep, Etat2D& dest) const{

    if(dep.getHauteur() != dest.getHauteur() || dep.getLargeur() != dest.getLargeur()) dest = dep;

    for (int i = 0; i < dep.getHauteur(); i++){
        for(int j = 0; j < dep.getLargeur(); j++){
            if(updateCellule(dep, j,i)){
                dest.setCellule(j, i, !dep.getCellule(j,i));
            }
            else
               dest.setCellule(j, i, dep.getCellule(j,i));

        }
    }
}

bool Automate2D::updateCellule(const Etat2D e,  int x,  int y) const{
    unsigned int counter = 0;
     int l = 0;
     int c = 0;

    for(int i = (y-1); i <= (y+1); i++){
        for(int j = (x-1) ; j <= (x+1); j++){

            l = i;
            c = j;
            if(i < 0) {l = e.getHauteur()-1;}
            else if(i > e.getHauteur()-1) {l = 0;}
            if(j < 0) {c = e.getLargeur()-1;}
            else if(j > e.getLargeur()-1){c = 0;}

            if(e.getCellule(c, l)){
                counter++;
            }
        }
    }
    if(e.getCellule(x,y)){
        counter-=1;
        if(counter > m_nbMaxVoisin || counter < m_nbMinVoisin)
            return true;
        else
            return false;
    }
    else{
        if(counter == m_nbNaissance)
            return true;
        else
            return false;
    }
}


//---------------------------------------------Classe Simulateur 2D----------------------------------------------------------

Simulateur2D::Simulateur2D(const Automate2D& automate, unsigned int nbMaxEtats) : m_automate(automate), m_nbMaxEtats(nbMaxEtats), m_rang(0){
    m_etats = new Etat2D*[nbMaxEtats];
    for(unsigned int i = 0; i < nbMaxEtats; i++){
        m_etats[i] = nullptr;
    }
}

Simulateur2D::Simulateur2D(const Automate2D& automate, const Etat2D& depart, unsigned int nbMaxEtats) : m_automate(automate), m_depart(&depart), m_nbMaxEtats(nbMaxEtats), m_rang(0) {
    m_etats = new Etat2D*[nbMaxEtats];
    for(unsigned int i = 0; i < nbMaxEtats; i++){
        m_etats[i] = nullptr;
    }
    m_etats[0] = new Etat2D(depart);
}

void Simulateur2D::setDepart(const Etat2D& depart){
    if(m_depart == nullptr)
        m_depart = new Etat2D(depart);
    m_depart = &depart;
    m_etats[0] = new Etat2D(depart);
}

void Simulateur2D::next(){
    if(m_rang < m_nbMaxEtats){
       m_etats[m_rang+1] = new Etat2D;
       m_automate.appliquerTransition(*m_etats[m_rang], *m_etats[m_rang+1]);
    }
    else{
        Etat2D *old = m_etats[m_rang%m_nbMaxEtats +1];
        m_etats[m_rang%m_nbMaxEtats +1] = new Etat2D;
        m_automate.appliquerTransition(*m_etats[m_rang%m_nbMaxEtats +1], *m_etats[m_rang%m_nbMaxEtats]);
        delete[] old;
    }
    m_rang++;
}

void Simulateur2D::run(unsigned int nbRun){
    for(unsigned int i = 0; i < nbRun; i++)
        next();
}

unsigned int Simulateur2D::getRang() const{return m_rang;}

unsigned int Simulateur2D::getNbMaxEtats() const{return m_nbMaxEtats;}

Etat2D& Simulateur2D::getDernierEtat() const {return *m_etats[m_rang%m_nbMaxEtats];}
