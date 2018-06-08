#ifndef AUTOCELL_H
#define AUTOCELL_H

#include "automate2d/autocell2d.h"
#include "automate1d/autocell1d.h"

class Autocell;
class AutoCell1D;
class AutoCell2D;

template <class Type>
class Fenetre {
    Type* m_fenetre;
    friend class Autocell;
public:
    explicit Fenetre(Type* fenetre) : m_fenetre(fenetre) {}
};

class Autocell : public QWidget {
    Q_OBJECT
    Fenetre<AutoCell1D>* f_autocell1d = nullptr;
    Fenetre<AutoCell2D>* f_autocell2d = nullptr;

    QHBoxLayout *m_automateLayout;
    QComboBox *m_menuAutomate;
    QPushButton *m_reset;
    QVBoxLayout *m_sonLayout = nullptr;
    QVBoxLayout* m_layout = nullptr;

    void cleanLayout(QLayout *layout);
public:
    explicit Autocell(QWidget* parent = nullptr);
    QVBoxLayout* getLayout() const{return m_sonLayout;}
private slots:
    void reset(const bool&);
    void chargerAutomate(const QString& s);
};

#endif // AUTOCELL_H
