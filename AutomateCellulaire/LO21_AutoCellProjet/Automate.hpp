#ifndef Automate_hpp
#define Automate_hpp
#include <stdio.h>
#include <iostream>
#include "Etat.hpp"

/*********** AUTOMATE EXCEPTION **********/
/**
*  @class AutomateException
*/
class AutomateException
{
public:
    AutomateException(const std::string& message):info(message){}
    std::string getInfo() const { return info; }
private:
    std::string info;
};

/*********** AUTOMATE **********/
/**
*  @class Automate
*  @brief classe abstraite des automates
*  @details comporte la fonction commune appliquerTransition
*  afin de respecter principe de substitution, le destructeur est virtuel
*  on enlève le possibilité de construire un automate par l'utilisateur directement, par duplication ou par affectation. Et on laisse la classe AutomateManager contrôler la fabrication et la suppression en utilisant le design parttern Singleton
*/
class Automate
{
public:
    virtual void appliquerTransition(const Etat& dep, Etat& dest) const = 0;
};

/*********** AUTOMATE 1D **********/
/**
*  @class AutomateUnDimension
*  @brief Une classe des automates 1 dimension. Comporte une règle et ses accesseurs.
*/
class AutomateUnDimension : public Automate
{
public:
    unsigned short int getNumero(void) const { return numero; }
    const std::string& getNumeroBit(void) const { return numeroBit; }
    
    virtual void appliquerTransition(const Etat& dep, Etat& dest) const;
    
private :
//règle sous forme décimale
    unsigned short int numero;
//règle sous forme binaire
    std::string numeroBit;
    
    AutomateUnDimension();
    AutomateUnDimension(unsigned short int num);
    AutomateUnDimension(const std::string& numBit);
    AutomateUnDimension(const AutomateUnDimension& a) = default;
    AutomateUnDimension& operator=(const AutomateUnDimension& a) = default;
    ~AutomateUnDimension() = default;
    friend class AutomateManager;

    static short unsigned int NumBitToNum(const std::string& num);
    static std::string NumToNumBit(short unsigned int num);
};

std::ostream& operator<<(std::ostream& f, const AutomateUnDimension& A);

/*********** AUTOMATE 2D **********/
/**
*  @class AutomateDeuxDimension
*  @brief Une classe des automates 2 dimensions comportant 2 règles: une règle pour les cellules vivantes à l'examination, et une pour les cellules mortes à l'examination
*/
class AutomateDeuxDimension : public Automate
{
public:
    unsigned short int getNumCellVivant() const { return numCellVivant; }
    unsigned short int getNumCellMorte() const { return numCellMorte; }
    const std::string& getNumeroBitVivant() const {
        return numBitCellVivant; }
    const std::string& getNumeroBitMorte() const { return numBitCellMorte; }
    
    virtual void appliquerTransition(const Etat& dep, Etat& dest) const;

private:
//règle sous forme décimale
    unsigned short int numCellVivant;
    unsigned short int numCellMorte;
//règle sous forme binaire
    std::string numBitCellVivant;
    std::string numBitCellMorte;
    
    AutomateDeuxDimension(unsigned short int n, unsigned short int m);
    AutomateDeuxDimension(const std::string& BitVivant,const std::string& BitMorte);
    AutomateDeuxDimension(const AutomateDeuxDimension& a) = default;
    AutomateDeuxDimension& operator=(const AutomateDeuxDimension& a) = default;
    ~AutomateDeuxDimension() = default;
    friend class AutomateManager;
    
    static short unsigned int NumBitToNum(const std::string& num);
    static std::string NumToNumBit(short unsigned int num);
};

std::ostream& operator<<(std::ostream& f, const AutomateDeuxDimension& A);

/*********** AUTOMATE PLUSIEURS ETATS **********/
/**
*  @class AutomatePlusieursEtats
*  @brief Une classe des automates une dimension mais à plusieurs états, notre propre automate. Il comporte une règle et comporte 99 états possible (de 0 à 98)
*/
class AutomatePlusieursEtats : public Automate
{
public:

    unsigned short int getNumero(void) const { return numero; }
    const std::string& getNumeroBit(void) const { return numeroBit; }
    void appliquerTransition(const Etat& dep, Etat& dest) const;
    
private:
    short unsigned int numero;
    std::string numeroBit;
    
    AutomatePlusieursEtats();
    AutomatePlusieursEtats(short unsigned int n);
    AutomatePlusieursEtats(std::string nB);
    AutomatePlusieursEtats(const AutomatePlusieursEtats& a) = default;
    AutomatePlusieursEtats& operator=(const AutomatePlusieursEtats& a) = default;
    ~AutomatePlusieursEtats() = default;
    friend class AutomateManager;
    
    static short unsigned int NumBitToNumPE(const std::string& num);
    static std::string NumToNumBitPE(short unsigned int num);
};

std::ostream& operator<<(std::ostream& f, const AutomatePlusieursEtats& A);

#endif /* Automate_hpp */
