#include "automate1d.h"
#include <iostream>

// Allocation mémoire pour n booleen
Etat::Etat(unsigned int n) : dimension(n), valeur(new bool[n]) {
    // Initialisation de l'état à 0 par défaut
    for(unsigned int i=0; i<n; i++) {
        valeur[i] = false;
    } 
}

void Etat::setCellule(unsigned int i, bool val) {
    // Vérifie le respect de la dimension du tableau
    if(i>=dimension) {
        throw AutomateException("Erreur cellule");
    }
    valeur[i] = val;
}

bool Etat::getCellule(unsigned int i) const {
    if(i>=dimension) {
        throw AutomateException("Erreur cellule");
    }
    return valeur[i];
}

// Constructeur par recopie
// On alloue de la mémoire pour un nouveau tableau de bool
Etat::Etat(const Etat& e) : dimension(e.dimension), valeur(new bool[e.dimension]) {
    // Recopie les valeurs depuis le tableau source vers le tableau dest
    for(unsigned int i=0; i<dimension;i++) {
        valeur[i] = e.valeur[i];
    }
}

Etat & Etat::operator=(const Etat & e) {
    // Rien à faire si on essaie d'affecter un objet à lui-même
    if(this != &e) {
        // Si le tableau actuel n'est pas de la bonne dimension
        if(dimension != e.dimension) {
            dimension = e.dimension;
            // Sauvegarde du pointeur vers ancien tableau pour désallocation mémoire
            bool * old = valeur;
            // Allocation mémoire pour le tableau
            valeur = new bool[dimension];
            // Libération de la mémoire allouée à l'ancien tableau
            delete[] old;
        } 
        // Recopie des valeurs depuis e.valeur vers newvaleur
        for(unsigned int i=0; i<e.dimension;i++) {
            valeur[i] = e.valeur[i];
        }
    }
    return *this;
}


std::ostream & operator<<(std::ostream & f, const Etat & e) {
    for(unsigned int i=0; i<e.getDimension();i++) {
        if(e.getCellule(i)) {
            f << "X";
        } else {
            f << " ";
        }
    }
    return f;
}

short unsigned int NumBitToNum(const std::string& num) {
if (num.size() != 8) throw AutomateException("Numero d’automate indefini");
int puissance = 1;short unsigned int numero = 0;
for (int i = 7; i >= 0; i--) {
if (num[i] == '1') numero += puissance;
else if (num[i] != '0') throw AutomateException("Numero d’automate indefini");
puissance *= 2;
}
return numero;
}
std::string NumToNumBit(short unsigned int num) {
std::string numeroBit;
if (num > 256) throw AutomateException("Numero d’automate indefini");
unsigned short int p = 128;
int i = 7;
while (i >= 0) {
if (num >= p) { numeroBit.push_back('1'); num -= p; }
else { numeroBit.push_back('0'); }
i--;
p = p / 2;
}
return numeroBit;
}

Automate::Automate(unsigned short int num) : numero(num), numeroBit(NumToNumBit(num)) {
}

Automate::Automate(const std::string & num) : numero(NumBitToNum(num)), numeroBit(num) {}

void Automate::appliquerTransition(const Etat & dep, Etat & dest) const {
    // Redimensionne le  tableau de destination si nécessaire
    // On pourrait également choisir de lever une exception à la place
    if(dep.getDimension() != dest.getDimension()) dest = dep;
    // Pour chaque cellule
    for(unsigned int i=0; i<dep.getDimension(); i++) {
       // Identifie la conf (cellule gauche, centrale, droite)
       unsigned int conf = 0;
       // Si présence d'une cellule à gauche, +=valeur[gauche]*2^2
       if(i>0) conf+= dep.getCellule(i-1)*4; 
       // +=valeur[milieu]*2^1
       conf+=dep.getCellule(i)*2 ;
       // Si présence d'une cellule à droite, +=valeur[droite]*2^0
       if(i< dep.getDimension()-1) conf+= dep.getCellule(i+1);
       // On obtient :
       // 0 => conf 000 (etat de sortie accessible à numeroBit[7])
       // 1 => conf 001 (etat de sortie accessible à numeroBit[6])
       // ...
       // numeroBit[7-conf] nous renvoie l'état à l'instant t+1 sous forme dd'un char '0' ou '1'
       // numeroBot[7-conf]-'0' nous renvoie l'entier associé 0 ou 1
       dest.setCellule(i, numeroBit[7-conf]-'0'); 
    }
}
std::ostream & operator<<(std::ostream & f, const Automate & A) {
    f << A.getNumero() << " : " << A.getNumeroBit() << std::endl;
    return f;
}

Simulateur::Simulateur(const Automate & a, unsigned int buffer) : automate(a), nbMaxEtats(buffer) {
    etats = new Etat*[nbMaxEtats];
    for(unsigned int i=0; i< nbMaxEtats; i++) {
        etats[i] = nullptr;
    }
}
Simulateur::Simulateur(const Automate & a, const Etat & dep, unsigned int buffer) : automate(a), depart(&dep), nbMaxEtats(buffer) {
    etats = new Etat*[nbMaxEtats];
    // Init le premier état
    for(unsigned int i=0; i< nbMaxEtats; i++) {
        etats[i] = nullptr;
    }
    etats[0] = new Etat(dep);
}

void Simulateur::build(unsigned int c) {
    if(c >= nbMaxEtats) throw AutomateException("Erreur taille buffer");
    if(etats[c] == nullptr) etats[c] = new Etat;
}

void Simulateur::reset() {
    if(depart==nullptr) throw AutomateException("Etat de départ indéfini");
    // Alloue de la mémoire si nécessaire pour le premier état
    build(0);
    // Enregistre l'état initial dans le buffer
    *etats[0] = *depart;
     rang = 0;
}

void Simulateur::setEtatDepart(const Etat & e) {
    // Lorsque l'état de départ change, on reset tout
    depart = &e;
    reset();
}

void Simulateur::next() {
    if(depart==nullptr) throw AutomateException("Etat de départ indéfnini");
    rang++;
    // Alloue de la mémoire si nécessaire pour le nouvel état
    build(rang%nbMaxEtats);
    automate.appliquerTransition(*etats[(rang-1)%nbMaxEtats], *etats[rang%nbMaxEtats]);
}

void Simulateur::run(unsigned int nbSteps) {
    for(unsigned int i=0; i<nbSteps; i++) {
        next();
    }
}

const Etat & Simulateur::dernier() const {
    return *etats[rang%nbMaxEtats];
}

Simulateur::~Simulateur() {
    for(unsigned int i=0; i<nbMaxEtats; i++) delete etats[i];
    delete[] etats;
}


AutomateManager::AutomateManager() {
  for(unsigned int i=0; i<256; i++) automates[i] = nullptr;
}

AutomateManager::~AutomateManager() {
  for(unsigned int i=0; i<256; i++) delete automates[i];

}

AutomateManager * AutomateManager::instance = nullptr;

AutomateManager & AutomateManager::getAutomateManager() {
  if(!instance) instance = new AutomateManager;
  return *instance;
}

void AutomateManager::libererAutomateManager() {
  delete instance;
  instance = nullptr; // On pourra créer une nouvelle
}

const Automate & AutomateManager::getAutomate(unsigned short int num) {
  // Appel au constructeur de la classe automate
  // On a le droit avec la déclaration d'amitié effectuée
  if(!automates[num]) automates[num] = new Automate(num);
  return *automates[num];
}

// Solution 2 : SingletonPattern
/*AutomateManager& AutomateManager::getAutomateManager() {
  // L'instance est créée au premier appel de la méthode
  // Pendant toute la durée de vie du programme, c'est cette même instance qui sera renvoyée
  // Décharge le client de l'opération de libération mémoire
  // Empêche de supprimer puis recréer une instance
  static AutomateManager m;
  return m;
}*/




/*
 * Solution 3 : SingletonPattern
// La classe AutomateManager encapsule un Handler
// Le destructeur de Handler assure la libération mémoier
// On peut également utiliser libererAutomateManager pour réaliser cette opération si on souhaite recréer une instance
AutomateManager::Handler AutomateManager::handler = Handler();
AutomateManager& AutomateManager::getAutomateManager() {
  if (!handler.instance) handler.instance = new AutomateManager;
  return *handler.instance;
}
void AutomateManager::libererAutomateManager() {
  delete handler.instance;
  handler.instance = nullptr;
}
*/
