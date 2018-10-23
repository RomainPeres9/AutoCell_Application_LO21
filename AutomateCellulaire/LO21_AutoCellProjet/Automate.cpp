#include "Automate.hpp"

/*********** AUTOMATE 1D **********/

AutomateUnDimension::AutomateUnDimension(): numero(30), numeroBit(NumToNumBit(30)){}

AutomateUnDimension::AutomateUnDimension(short unsigned int n)
{
    if( n >= 0 && n < 256 )
    {
        numero = n;
        numeroBit = NumToNumBit(n);
    }
    else
    {
        throw AutomateException("num not in [0..256]");
    }
}

short unsigned int AutomateUnDimension::NumBitToNum(const std::string& num)
{
    if (num.size() != 8)
        throw AutomateException("Numero d’automate indefini");
    int puissance = 1;
    
    short unsigned int numero = 0;
    for (int i = 7; i >= 0; i--)
    {
        if (num[i] == '1')
            numero += puissance;
        else if (num[i] != '0')
            throw AutomateException("Numero d’automate indefini");
        puissance *= 2;
    }
    return numero;
}

std::string AutomateUnDimension::NumToNumBit(short unsigned int num)
{
    std::string numeroBit;
    if (num >= 256)
        throw AutomateException("Numero d’automate indefini");
    unsigned short int p = 128;
    int i = 7;
    while (i >= 0)
    {
        if (num >= p)
        {
            numeroBit.push_back('1');
            num -= p;
        }
        else
        {
            numeroBit.push_back('0');
        }
        i--;
        p = p / 2;
    }
    return numeroBit;
}

void AutomateUnDimension::appliquerTransition(const Etat& dep, Etat& dest) const
{
    if (dep.getHauteur() != 1) {
        throw AutomateException("Ce n'est pas un dimension");
        return;
    }
    //Ici je pense que peut-etre c'est mieux d'creer 2 class d'Etats,Etat 1 dimension et Etat 2 dimension
    if (dep.getLargeur() != dest.getLargeur()) dest = dep;
    for (unsigned int i = 0; i < dep.getLargeur(); i++) {
        long conf = 0;
        if (i > 0) conf += dep.getCellule(0, i - 1) * 4;
        conf += dep.getCellule(0, i) * 2;
        if (i < dep.getLargeur() - 1) conf += dep.getCellule(0, i + 1);
        dest.setCellule(0, i, numeroBit[7 - conf] - '0');
    }
}

std::ostream& operator<<(std::ostream& f, const AutomateUnDimension& A)
{
    f << A.getNumero() << " : " << A.getNumeroBit() << "\n";
    return f;
}

/*********** AUTOMATE 2D **********/

AutomateDeuxDimension::AutomateDeuxDimension(unsigned short int n, unsigned short int m) :numCellVivant(n), numCellMorte(m), numBitCellVivant(NumToNumBit(n)), numBitCellMorte(NumToNumBit(m)) {}

AutomateDeuxDimension::AutomateDeuxDimension(const std::string& BitVivant, const std::string& BitMorte) : numCellVivant(NumBitToNum(BitVivant)), numCellMorte(NumBitToNum(BitMorte)), numBitCellVivant(BitVivant), numBitCellMorte(BitMorte) {}

short unsigned int AutomateDeuxDimension::NumBitToNum(const std::string& num)
{
    if (num.size() != 9)
        throw AutomateException("Numero d'automate indefini");
    int puissance = 1;
    short unsigned int numero = 0;
    for (int i = 8; i >= 0; i--)
    {
        if (num[i] == '1')
            numero += puissance;
        else if (num[i] != '0')
            throw AutomateException("Numero d'automate indefini");
        puissance *= 2;
    }
    return numero;
}

std::string AutomateDeuxDimension::NumToNumBit(short unsigned int num)
{
    std::string numeroBit;
    if (num >= 512)
        throw AutomateException("Numero d'automate indefini");
    unsigned short int p = 256;
    int i = 8;
    while (i >= 0)
    {
        if (num >= p)
        {
            numeroBit.push_back('1');
            num -= p;
        }
        else
        {
            numeroBit.push_back('0');
        }
        i--;
        p = p / 2;
    }
    return numeroBit;
}


void AutomateDeuxDimension::appliquerTransition(const Etat& dep, Etat& dest) const
{
    if (dep.getHauteur() < 2)
    {
        throw AutomateException("Ce n'est pas deux dimension");
        return;
    }
    //Ici je pense que peut-etre c'est mieux d'creer 2 class d'Etats,Etat 1 dimension et Etat 2 dimension
    if ((dep.getLargeur() != dest.getLargeur()) || (dep.getHauteur() != dest.getHauteur())) dest = dep;
    for (unsigned int i = 0; i < dep.getHauteur(); i++)
    {
        for (unsigned int j = 0; j < dep.getLargeur(); j++)
        {
            unsigned short int conf = 0;
            if (i > 0)
                conf += dep.getCellule(i - 1, j) ;
            if (i < dep.getHauteur() - 1)
                conf += dep.getCellule(i + 1, j);
            if (j > 0)
                conf += dep.getCellule(i , j-1);
            if (j < dep.getLargeur() - 1)
                conf += dep.getCellule(i, j + 1);
            if (i > 0 && j > 0)
                conf += dep.getCellule(i - 1, j - 1);
            if ((i < dep.getHauteur() - 1) && (j < dep.getLargeur() - 1))
                conf += dep.getCellule(i + 1, j + 1);
            if (i > 0 && (j < dep.getLargeur() - 1))
                conf += dep.getCellule(i - 1, j + 1);
            if ((i < dep.getHauteur() - 1) && j > 0)
                conf += dep.getCellule(i + 1, j - 1);
            if (dep.getCellule(i, j) == 1)
                dest.setCellule(i, j, numBitCellVivant[8 - conf] - '0');
            else if (dep.getCellule(i, j) == 0)
                dest.setCellule(i, j, numBitCellMorte[8 - conf] - '0');
        }
    }
}

std::ostream& operator<<(std::ostream& f, const AutomateDeuxDimension& A) {
    f << A.getNumCellVivant() << " : " << A.getNumeroBitVivant() << "\n" << A.getNumCellMorte() << " : " << A.getNumeroBitMorte() << "\n";
    return f;
}

/*********** AUTOMATE PLUSIEURS ETATS **********/

AutomatePlusieursEtats::AutomatePlusieursEtats() : numero(30), numeroBit(NumToNumBitPE(30)){}

AutomatePlusieursEtats::AutomatePlusieursEtats(short unsigned int n)
{
    if( n >= 0 && n < 256 )
    {
        numero = n;
        numeroBit = NumToNumBitPE(n);
    }
    else
    {
        throw AutomateException("num not in [0..256]");
    }
}

short unsigned int AutomatePlusieursEtats::NumBitToNumPE(const std::string& num)
{
    if (num.size() != 8)
        throw AutomateException("Numero d’automate indefini");
    int puissance = 1;
    
    short unsigned int numero = 0;
    for (int i = 7; i >= 0; i--)
    {
        if (num[i] == '1')
            numero += puissance;
        else if (num[i] != '0')
            throw AutomateException("Numero d’automate indefini");
        puissance *= 2;
    }
    return numero;
}

std::string AutomatePlusieursEtats::NumToNumBitPE(short unsigned int num)
{
    std::string numeroBit;
    if (num >= 256)
        throw AutomateException("Numero d’automate indefini");
    unsigned short int p = 128;
    int i = 7;
    while (i >= 0)
    {
        if (num >= p)
        {
            numeroBit.push_back('1');
            num -= p;
        }
        else
        {
            numeroBit.push_back('0');
        }
        i--;
        p = p / 2;
    }
    return numeroBit;
}


void AutomatePlusieursEtats::appliquerTransition(const Etat& dep, Etat& dest) const
{
    if(dep.getLargeur() != dest.getLargeur())
            dest = dep; //on peut le faire grace a l'operateur d'affectation implemente pour la classe Etat2

        for(unsigned int counter(0); counter < dep.getLargeur(); counter++)
        {
            long int value;
            if(counter == 0)
            {
                if( ( dep.getCellule(0, dep.getLargeur()-1)%2 != 0 ) && (dep.getCellule(0, 0)%2 != 0 ) && ( dep.getCellule(0,1)%2 != 0 ) ) // I I I
                {
                    if(numeroBit[7] == '1')//On additionne le voisinnage à la case concernée
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else//On soustrait le voisinnage à la case concernée
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 == 0 ) && ( dep.getCellule(0, 0)%2 != 0 ) && ( dep.getCellule(0, 1)%2 != 0 ) ) // P I I
                {
                    if(numeroBit[6] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 != 0 ) && ( dep.getCellule(0, 0)%2 == 0 ) && ( dep.getCellule(0, 1)%2 != 0 ) ) // I P I
                {
                    if(numeroBit[5] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 != 0 ) && ( dep.getCellule(0, 0)%2 != 0 ) && ( dep.getCellule(0, 1)%2 == 0 ) ) // I I P
                {
                    if(numeroBit[4] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 == 0 ) && ( dep.getCellule(0, 0)%2 == 0 ) && ( dep.getCellule(0, 1)%2 != 0 ) ) // P P I
                {
                    if(numeroBit[3] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 == 0 ) && ( dep.getCellule(0, 0)%2 != 0 ) && ( dep.getCellule(0, 1)%2 == 0 ) ) // P I P
                {
                    if(numeroBit[2] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else if( ( dep.getCellule(0, dep.getLargeur()-1)%2 != 0 ) && ( dep.getCellule(0, 0)%2 == 0 ) && ( dep.getCellule(0, 1)%2 == 0 ) ) // I P P
                {
                    if(numeroBit[1] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
                else  // P P P
                {
                    if(numeroBit[0] == '1')
                    {
                        value = dep.getCellule(0, 0) + ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                    else
                    {
                        value = dep.getCellule(0, 0) - ( dep.getCellule(0, dep.getLargeur()-1) + dep.getCellule(0, 1) );
                    }
                }
            }
            else
            {
                if( (dep.getCellule(0, counter-1)%2 != 0) && (dep.getCellule(0, counter)%2 != 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 != 0) ) // I I I
                {
                    if(numeroBit[7] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 == 0) && (dep.getCellule(0, counter)%2 != 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 != 0) ) // P I I
                {
                    if(numeroBit[6] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 != 0) && (dep.getCellule(0, counter)%2 == 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 != 0) ) // I P I
                {
                    if(numeroBit[5] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 != 0) && (dep.getCellule(0, counter)%2 != 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 == 0) ) // I I P
                {
                    if(numeroBit[4] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 == 0) && (dep.getCellule(0, counter)%2 == 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 != 0) ) // P P I
                {
                    if(numeroBit[3] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 == 0) && (dep.getCellule(0, counter)%2 != 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 == 0) ) // P I P
                {
                    if(numeroBit[2] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else if( (dep.getCellule(0, counter-1)%2 != 0) && (dep.getCellule(0, counter)%2 == 0) && (dep.getCellule(0, (counter+1)%(dep.getLargeur()))%2 == 0) ) // I P P
                {
                    if(numeroBit[1] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
                else  // P P P
                {
                    if(numeroBit[0] == '1')
                    {
                        value = dep.getCellule(0, counter) + ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                    else
                    {
                        value = dep.getCellule(0, counter) - ( dep.getCellule(0, counter-1) + dep.getCellule(0, (counter+1)%(dep.getLargeur())) );
                    }
                }
            }
            dest.setCellule(0, counter, value);
        }
}

std::ostream& operator<<(std::ostream& f, const AutomatePlusieursEtats& A)
{
    f << A.getNumero() << " : " << A.getNumeroBit() << "\n";
    return f;
}






