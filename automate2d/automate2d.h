#ifndef AUTOMATE2D_H
#define AUTOMATE2D_H

#include <iostream>

class Etat2D {
  unsigned int m_largeur;
  unsigned int m_hauteur;
  bool* m_valeurs;

public:
  Etat2D(unsigned int largeur = 50, unsigned int hauteur = 50);
  Etat2D(const Etat2D& etat);
  Etat2D operator=(const Etat2D& etat);
  ~Etat2D();
  unsigned int getLargeur() const ;
  unsigned int getHauteur() const ;
  bool getCellule(unsigned int x, unsigned int y) const;
  void setCellule(unsigned int x, unsigned int y, bool valeur);
};
std::ostream & operator<<(std::ostream & f, const Etat2D & e);

class Automate2D {

  unsigned int m_nbMinVoisin;
  unsigned int m_nbMaxVoisin;
  unsigned int m_nbNaissance;

public:
  Automate2D(unsigned int nbMaxVoisin = 3, unsigned int nbMinVoisin = 2, unsigned int nbNaissance = 3);
  Automate2D(const Automate2D& automate) = default;
  ~Automate2D() = default;
  unsigned int getNbMinVoisin() const;
  unsigned int getNbMaxVoisin() const;
  unsigned int getNbNaissance() const;
  void setNbMinVoisin(unsigned int valeur);
  void setNbMaxVoisin(unsigned int valeur);
  void setNbNaissance(unsigned int valeur);
  void appliquerTransition(const Etat2D& dep, Etat2D& dest) const;
  bool updateCellule(const Etat2D e,int x, int y) const;
};

class Simulateur2D {
  const Automate2D& m_automate;
  const Etat2D* m_depart;
  Etat2D** m_etats;
  unsigned int m_nbMaxEtats;
  unsigned int m_rang;

public:
  Simulateur2D(const Automate2D& automate, unsigned int nbMaxEtats = 3);
  Simulateur2D(const Automate2D& automate, const Etat2D& depart, unsigned int nbMaxEtats = 3);
  void setDepart(const Etat2D& depart);
  void next();
  void run(unsigned int nbRun);
  unsigned int getRang() const;
  unsigned int getNbMaxEtats() const;
  Etat2D& getDernierEtat() const;

};

#endif // AUTOMATE2D_H
