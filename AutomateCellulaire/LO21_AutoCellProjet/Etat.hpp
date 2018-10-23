#ifndef Etat_hpp
#define Etat_hpp
#include <stdio.h>
#include <iostream>
#include <QJsonObject>
#include <QJsonArray>

/*********** ETAT EXCEPTION **********/
/**
*  @class EtatException
*/
class EtatException
{
public:
    EtatException(const std::string& message):info(message){}
    std::string getInfo() const { return info; }
private:
    std::string info;
};

/*********** ETAT **********/
/**
*  @class Etat
*  @brief Une classe des états comportant un tableau de valeur
*  @details Ici notre classe état est à une dimension ou deux, qui est rajusté pour convenir aux besoins de tous les automates que nous avons.
*  On fait l'hypothèse qu'après avoir créé une instance de classe état, on n'a pas le droit de changé son Min, son Max, son dimLargeur, son dimHauteur directement pour prévenir les erreurs possible.
*  la seul façon de changer son Min, son Max,son dimLargeur, son dimHauteur est de recréer une autre instance et faire la affectation ou duplication
*/
class Etat
{
public:
    /**
    *  @brief Constructeurs sans paramètres
    *  @details ici dimLargeur=0,dimHauteur=0,valeur=nullptr, Min=0,Max=1
    */
    Etat() :dimLargeur(0), dimHauteur(0), valeur(nullptr),Min(0),Max(1) {}
    /**
    *  @brief Constructeurs avec paramètres
    *  @details par défaut Min=0,Max=1.
    *  Il y a une vérification que Min<Max
    *  Au début toutes les cases du tableau valeur sont instanciés 0
    */
    Etat(unsigned int h, unsigned int l, long int MinNum=0, long int MaxNum=1);
    /**
    *  @brief Destructeur avec désallocation de mémoire
    */
    ~Etat();
    /**
    *  @brief Constructeur par duplication
    */
    Etat(const Etat& e);
    /**
    *  @brief Constructeur par affectation
    *  @details Si les dimensions de ces 2 états sont les mêmes, on ne fait pas de désallocation et après allocation pourr gagner du temps d'exécution
    */
    Etat& operator=(const Etat& e);
    /**
    *  @brief Configurer une case dans la matrice valeur
    *  @details Si i et j sont hors limite, déclenche une exception
    *  Si val est hors de l'intervalle, on fait un mécanisme de résidu de Max ou Min pour s'assurer que le nombre dans la case est bien contenu dans l'intervalle.
    *  Ce mécanisme est aussi pour réaliser le mécanisme de notre propre automate cellulaire: automate de plusieurs états
    */
    void setCellule(unsigned int i, unsigned int j, long int val);
    long int getCellule(unsigned int i, unsigned int j) const;
    unsigned int getLargeur() const { return dimLargeur; }
    unsigned int getHauteur() const { return dimHauteur; }
    long int getMin() const { return Min; }
    long int getMax() const { return Max; }
    ///@brief retourne l'état sous forme d'un tableau Json
    QJsonArray saveState()const;
    
private:
    ///@brief Largeur d'une ligne (si 1 dimension, nb cellules)
    unsigned int dimLargeur;
    ///@brief Hauteur de l'état, @details si automate 1 dimension dimHauteur=1.
    unsigned int dimHauteur;
    ///@brief Un tableau d'2 dimension de long int, @details long int ici pour faire notre propre automate cellulaire.
    long int** valeur;
    ///@details min pour vérifier la valeur minimum dans le tableau valeur, si Automate 1 dimension ou 2 dimension binaire: Min=0.
    long int Min;
    ///@details Max pour vérifier la valeur minimum dans le tableau valeur, si Automate 1 dimension ou 2 dimension normale Max=1.
    long int Max;
};

std::ostream& operator<<(std::ostream& f, const Etat& e);


#endif /* Etat_hpp */
