#ifndef _AUTOMATE_H
#define _AUTOMATE_H
#include <string>
#include <iostream>

std::string NumToNumBit(short unsigned int num) ;
short unsigned int NumBitToNum(const std::string& num);

class AutomateException {
public:
    AutomateException(const std::string& message):info(message) {}
    std::string getInfo() const { return info; }
private:
    std::string info;
};

class Etat {
// Remarque : privé par défaut pour class (public pour struct)
private:
    // Nombre de cellules
    unsigned int dimension;
    // Etat associé à chaque cellule (false=>0, true=>1)
    bool * valeur;
public:
    Etat(unsigned int n=0);
    // Allocation dynamique de mémoire via opérateur new dans le constructeur => La classe nécessite un destructeur pour
    // réaliser la libération de la mémoire
    ~Etat() { delete[] valeur;}
    // Les versions générées par le compilateur du constructeur par recopie et de l'opérateur d'affectation réalise une
    // initialisation/affectation attribut par attribut
    // Cela provoquerait le partage du même tableau valeur entre 2 objets Etat
    Etat(const Etat & e);
    Etat & operator=(const Etat & e);
    // Méthode non inline car ajout de contrôles pour gestion des exceptions
    void setCellule(unsigned int i, bool val);
    bool getCellule(unsigned int i) const;
    // Méthode inline (définition au moment de la déclaration)
    unsigned int getDimension() const { return dimension; }

};
std::ostream & operator<<(std::ostream & f, const Etat & e);

// La classe automate ne disposant pas de constructeur sans argument, il n'est pas possible de définir un tableau
// d'objets Automate ou d'utiliser l'opérateur new[] pour allouer dynamiquement un tableau d'objects Automate. 
// Remarque : A un stade avancé du C++, cette opération est possible en utilisant l'opérateur new[]() de bas niveau ou
// encore un objet allocator (séparer allocation mémoire et construction d'un objet)
// Il est tout à fait possible de créer des tableaux de pointeurs d'objets Automate. En effet, un pointeur n'a pas
// besoin d'être initialisé
// Automate * tab[10];
// Automate ** tabdyn = new Automate*[10];

class Automate {
private:
    // Numéro définissant le comportement de l'automate
    unsigned short int numero; 
    // Même info sous forme de string
    std::string numeroBit;
    // AutomateManager est désormais responsable de la gestion des automate
    // Passage en privé de toutes les opérations de manipulation
    Automate(unsigned short int num);
    Automate(const std::string & num);
    // default : C++11 Utilisation de la version générée par le compilateur
    ~Automate() = default;
    Automate(const Automate & a) = default;
    // Déclaration d'amitié pour donner à la classe AutomateManager les droits d'accès
    friend class AutomateManager;
public:
    unsigned short int getNumero() const { return numero; }
    const std::string & getNumeroBit() const { return numeroBit; }
    void appliquerTransition(const Etat & dep, Etat & dest) const;
    // Le comportement du constructeur par copie/opérateur d'affectation générés par le compilateur est satisfaisant.
    // Pas besoin de les réécrire
    // De même, pas d'opération d'allocation dynamique de mémoire réalisé donc pas besoin de surcharger le destructeur
};

std::ostream & operator<<(std::ostream & f, const Automate & t);

class Simulateur {
private:
    const Automate & automate;
    // Etat de départ de l'automate
    const Etat * depart = nullptr;
    Etat ** etats = nullptr;
    // Nombre d'états successifs mémorisés (buffer) 
    unsigned int nbMaxEtats;
    // Rang du dernier état mémorisé (état initial = rang 0)
    // Revient à ajouter rang(0) dans tous les constructeurs où rang n'est pas initialisé
    unsigned int rang = 0;
    // Ajout d'une méthode privée chargée d'allouer la mémoire pour un automate dans etats
    // c est l'indice du tableau depart pour lequel on souhaite allouer la mémoire
    void build(unsigned int c);
    // Pour interdire la duplication, il suffit de passer le constructeur de recopie et l'opérateur d'affectation en privé
    // Il n'est pas nécessaire de définir ces méthodes
    Simulateur(const Simulateur & s) = delete;
    Simulateur & operator=(const Simulateur & s) = delete;
public:
    // Il faudra appeler setEtatDepart
    Simulateur(const Automate & a, unsigned int buffer = 2);
    // On renseigne en plus l'état de départ
    Simulateur(const Automate & a, const Etat & dep, unsigned int buffer = 2);
    void setEtatDepart(const Etat & e);
    // Génère les n prochains états
    void run(unsigned int nbSteps);
    // Génère le prochain état
    void next();
    // Renvoie le dernier état
    const Etat & dernier() const;
    unsigned int getRangDernier() const { return rang; }
    // Revenir à l'état de départ
    void reset();
    // Allocation dynamique => Libération mémoire à réaliser, définition du destructeur
    ~Simulateur();
    //friend class Iterator; Pas nécessaire car Iterator est une classe interne à Simulateur => droit d'accès comme tout
    //membre de la classe Simulateur
    class Iterator {
      // Déclaration d'amitié pour donner accès au constructeur privé d'Iterator à la classe Simulateur (utilisé dans
      // getIterator)
      friend class Simulateur;
      // Accès au simulateur depuis lequel on veut parcourir les états
      Simulateur* sim = nullptr;
      // Indice de l'état courant
      int i = 0;
      // La classe simulateur pourra utiliser ce constructeur
      Iterator(Simulateur* s):sim(s), i(s->rang){}
    public:
      // Constructeur public
      // permet de faire Simulateur::Iterator i; => Appel au constructeur sans param
      Iterator() {}
      // Indique si tous les éléments ont été parcourus
      bool isDone() const {
        // i==-1 => La liste circulaire n'a pas encore été remplie et tous les éléments ont été parcourus
        // i == sim->rang - sim->nbMaxEtats => La liste circulaire est remplie et tous les éléments ont été parcourus
        return i == -1 ||
          i == sim->rang - sim->nbMaxEtats; 
      }
      // Passage à l'élément suivant
      void next() {
        // Si tous les éléments ont été parcourus => Exception
        if (isDone())
          throw AutomateException("error, next on an iterator which is done");
        // Passage à l'élément suivant
        i--;
        // Liste circulaire
        // Si le dernier etat affiche était à l'indice 0, il faut éventuellement passer à l'élément situé à l'indice
        // sim->nbMaxEtats - 1 (revient à la fin du tableau)
        if (i == -1 && sim->rang >= sim->nbMaxEtats) i = sim->nbMaxEtats - 1;
      }
      // Accès à l'état courant
      Etat& current() const {
        if (isDone())
          throw AutomateException("error, indirection on an iterator which is done");
        return *sim->etats[i%sim->nbMaxEtats];
      }
    };
    // Iterator vers la classe simulateur
    // On a accès à ce constructeur via la déclaration d'amitié
    Iterator getIterator() {
      // this renvoie un pointeur vers l'objet courant (de type Simulateur)
      return Iterator(this);
    }
    class ConstIterator {
      friend class Simulateur;
      const Simulateur* sim = nullptr;
      int i = 0;
      ConstIterator(const Simulateur* s) :sim(s), i(s->rang) {}
      public:
      ConstIterator() {}
      bool isDone() const {
        return i == -1 ||
          i== sim->rang - sim->nbMaxEtats;
      }
      void next() {
        if (isDone())
          throw AutomateException("error, next on an iterator which is done");
        i--;
        if (i == -1 && sim->rang >= sim->nbMaxEtats) i = sim->nbMaxEtats - 1;
      }
      const Etat& current() const {
        if (isDone())
          throw AutomateException("error, indirection on an iterator which is done");
        return *sim->etats[i%sim->nbMaxEtats];
      }
    };
    // IMPORTANT
    // 2 méthodes ne peuvent pas différer uniquement par le type retournée => Présence du "const" pour les différencier
    // On ne pourrait pas avoir :
    // ConstIterator getIterator();
    // Iterator getIterator();
    // => INTERDIT
    // L'astuce est de mettre une méthode const et l'autre non pour les différencier
    // Le compilateur utilisera la const si l'objet simulateur est const (cas dans afficherEtats), et l'autre sinon (cas
    // dans afficherEtModifierEtats)
    ConstIterator getIterator() const{
      return ConstIterator(this);
    }
    // 2e solution : Respecter l'interface utilisée par les conteneurs stl
    class iterator {
      friend class Simulateur;
      Simulateur* sim = nullptr;
      int i = 0;
      iterator(Simulateur* s) :sim(s), i(s->rang) {}
      iterator(Simulateur* s, int dep) :sim(s), i(dep) {}
      public:
      iterator() {}
      // Operateur unaire préfixe
      // Permet le passage à l'élément suivant
      iterator& operator++() {
        i--;
        if (i == -1 && sim->rang >= sim->nbMaxEtats) i = sim->nbMaxEtats - 1;
        return *this;
      }
      // Permet l'accès à l'élément courant
      Etat& operator *() const {
        return *sim->etats[i%sim->nbMaxEtats];
      }
      // Comparaison de 2 iterator : Permet de savoir si tous les éléments ont été parcourus
      bool operator!=(iterator it) const { return sim != it.sim || i != it.i; }
    };
    // Iterator vers premier élément
    iterator begin() { return iterator(this); }
    // Iterator fin
    iterator end() { if (rang < nbMaxEtats) return iterator(this, -1); else return
      iterator(this, rang - nbMaxEtats); }
    class const_iterator {
      friend class Simulateur;
      const Simulateur* sim = nullptr;
      int i = 0;
      const_iterator(const Simulateur* s) :sim(s), i(s->rang) {}
      const_iterator(const Simulateur* s, int dep) :sim(s), i(dep) {}public:
        const_iterator() {}
      const_iterator& operator++() {
        i--;
        if (i == -1 && sim->rang >= sim->nbMaxEtats) i = sim->nbMaxEtats - 1;
        return * this;
      }
      const Etat& operator *() const {
        return *sim->etats[i%sim->nbMaxEtats];
      }
      bool operator!=(const_iterator it) const { return sim != it.sim || i != it.i; }
    };
    const_iterator begin() const { return const_iterator(this); }
    const_iterator end() const { if (rang < nbMaxEtats) return const_iterator(this,
        -1); else return const_iterator(this, rang - nbMaxEtats); }
    const_iterator cbegin() const { return const_iterator(this); }
    const_iterator cend() const { if (rang < nbMaxEtats) return const_iterator(this,
        -1); else return const_iterator(this, rang - nbMaxEtats); }

};

class AutomateManager {
  // 256 automates possibles 
  Automate * automates[256];
  // Pour empêcher un utilisateur de créer des instances de AutomateManager, le constructeur/destructeur doivent être privés
  AutomateManager();
  ~AutomateManager();
  // Constructeur de recopie/opérateur d'affection déclarés mais non définis => Le compilateur ne va pas les générer
  // delete (facultatif, C++11) => Explicite la demande de non génération au compilateur
  AutomateManager(const AutomateManager & a) = delete;
  AutomateManager& operator=(const AutomateManager & a) = delete;
  // Instance uniquement de la classe AutomateManager
  static AutomateManager * instance;
public:
  const Automate & getAutomate(unsigned short int num);
  // Accesseur à l'unique instance de la classe 
  static AutomateManager & getAutomateManager();
  // Mémoire allouée dynamiquement à l'unique instance
  // Il faut fournir une méthode pour liébrer la mémoire
  static void libererAutomateManager();
};

// Solution 2 : SingletonPattern
// Dans cette solution, on va créer une variable static dans la méthode getAutomateManager
// De cette manière, l'opération de libération mémoire sera réalisée automatiquement à la fin du programme
// Désavantage : L'utilisateur n'a pas la possibilité de détruire le singleton et d'en recréer un comme c'était le cas
// avec la première solution
/*
class AutomateManager {
  Automate* automates[256];
  AutomateManager();
  ~AutomateManager();
  AutomateManager(const AutomateManager& a) = delete;
  AutomateManager& operator=(const AutomateManager& a) = delete;
  public:
  const Automate& getAutomate(unsigned short int num);
  const Automate& getAutomate(const std::string& num);
  static AutomateManager& getAutomateManager();
};
*/

// Solution 3 : SingletonPattern
// Dans cette solution, on encapsule instance dans Handler
// Le destructeur de handler (static) sera appelé automaiquement et libèrera la mémoire allouée à l'instance unique
// (présence du delete dans le destructeur)
// L'utilisateur a aussi la possibilité de détruire le singleton et d'en recréer un via la méthode
// libererAutomateManager
/*
class AutomateManager {
  // .... //
  struct Handler {
    AutomateManager* instance;
    Handler():instance(nullptr) {}
    ~Handler() { delete instance; }
  };
  static Handler handler;
public:
  static AutomateManager& getAutomateManager();
  static void libererAutomateManager();
};
*/

#endif
