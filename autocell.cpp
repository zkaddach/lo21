#include "autocell.h"



Autocell::Autocell(QWidget* parent) : QWidget(parent) {

    m_automateLayout = new QHBoxLayout;
    //Choice of Automate
    m_menuAutomate = new QComboBox(this);
    m_menuAutomate->addItem("Automate 1D");
    m_menuAutomate->addItem("Automate 2D");
    m_automateLayout->addWidget(m_menuAutomate);

    //Reset Button
    m_reset = new QPushButton("Reset");
    m_automateLayout->addWidget(m_reset);

     //The mother Layout
    m_layout = new QVBoxLayout;
    m_layout->addLayout(m_automateLayout);

    chargerAutomate("Automate 1D");
    //Configuration depending on choice
    connect(m_menuAutomate, SIGNAL(currentTextChanged(QString)),this, SLOT(chargerAutomate(QString)));
    connect(m_reset, SIGNAL(clicked(bool)),this, SLOT(reset(bool)));


    this->setLayout(m_layout);
}

void Autocell::reset(const bool&){
    //Configuration depending on choice
    if(m_menuAutomate->currentText() == "Automate 1D"){
             if(f_autocell1d != nullptr)
                 delete f_autocell1d;
             f_autocell1d = new Fenetre<AutoCell1D>(new AutoCell1D(this));
    }
    else if(m_menuAutomate->currentText() == "Automate 2D"){
        if(f_autocell2d != nullptr)
            delete f_autocell2d;
        f_autocell2d = new Fenetre<AutoCell2D>(new AutoCell2D(this));
    }
}

void Autocell::chargerAutomate(const QString& s){
    if(m_sonLayout != nullptr)
        cleanLayout(m_sonLayout);
    m_sonLayout = new QVBoxLayout;
    if(s == "Automate 1D"){
        f_autocell1d = new Fenetre<AutoCell1D>(new AutoCell1D(this));
    }
    else if(s == "Automate 2D"){
        f_autocell2d = new Fenetre<AutoCell2D>(new AutoCell2D(this));
    }
    m_layout->addLayout(m_sonLayout);
}

void Autocell::cleanLayout(QLayout *layout){

    QLayoutItem * item;
    QLayout * sublayout;
    QWidget * widget;
    //Go over all items in the Layout and erase them one by one
    while ((item = layout->takeAt(0))) {
        if ((sublayout = item->layout()) != 0) {cleanLayout(sublayout);}
        else if ((widget = item->widget()) != 0) {widget->hide(); delete widget;}
        else {delete item;}
    }
        // if we wanted to delete the layout : delete layout;
}
