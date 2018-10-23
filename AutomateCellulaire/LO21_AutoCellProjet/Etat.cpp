#include "Etat.hpp"

Etat::Etat(unsigned int h, unsigned int l, long int MinNum, long int MaxNum) : dimHauteur(h), dimLargeur(l)
{
    if (MinNum > MaxNum)
        throw EtatException("Min>Max!!!!");
    Min = MinNum;
    Max = MaxNum;
    
    valeur = new long int*[dimHauteur];
    for (unsigned int i = 0; i < h; i++)
    {
        valeur[i] = new long int[dimLargeur];
    }
    for (unsigned int i = 0; i < h; i++)
    {
        for (unsigned int j = 0; j < l; j++)
        {
            valeur[i][j] = 0;

        }
    }
}

Etat& Etat::operator=(const Etat& e)
{
    if (this != &e) {
        Max = e.getMax();
        Min = e.getMin();
        if ((dimLargeur != e.dimLargeur) || (dimHauteur != e.dimHauteur)) {
            long int** newvaleur = new long int*[e.dimHauteur];
            for (unsigned int i = 0; i < e.dimHauteur; i++) {
                newvaleur[i] = new long int[e.dimLargeur];
                for (unsigned int j = 0; j < e.dimLargeur; j++)
                    newvaleur[i][j] = e.valeur[i][j];
            }
            long int** old = valeur;
            valeur = newvaleur;
            for (unsigned int i = 0; i < dimHauteur; i++) delete[] old[i];
            delete[] old;
            dimHauteur = e.dimHauteur;
            dimLargeur = e.dimLargeur;
        }
        else for (unsigned int i = 0; i < e.dimHauteur; i++) {
            for (unsigned int j = 0; j < e.dimLargeur; j++)
                valeur[i][j] = e.valeur[i][j];
        }
    }
    return *this;
}

Etat::Etat(const Etat& e) : dimLargeur(e.dimLargeur),dimHauteur(e.dimHauteur), valeur(new long int*[e.dimHauteur]), Min(e.Min),Max(e.Max)
{
    for (unsigned int i = 0; i < dimHauteur; i++) {
        valeur[i] = new long int[dimLargeur];
        for (unsigned int j = 0; j < dimLargeur; j++)
            valeur[i][j] = e.valeur[i][j];
    }
}

Etat::~Etat()
{
    for (unsigned int i = 0; i < dimHauteur; i++) delete[] valeur[i];
    delete[] valeur;
}

void Etat::setCellule(unsigned int i, unsigned int j, long int val)
{
    if ((i >= dimHauteur) || (j >= dimLargeur))
        throw EtatException("Erreur Cellule");
    if (val > Max)
    {
        val = val % Max;
    }
    
    if (val < Min)
    {
        while (val < Min)
        {
            val += Max;
        }
    }
    valeur[i][j] = val;
}

long int Etat::getCellule(unsigned int i, unsigned int j) const
{
    if ((i >= dimHauteur) || (j >= dimLargeur))
        throw EtatException("Erreur Cellule");
    return valeur[i][j];
}

QJsonArray Etat::saveState()const
{
    QJsonArray returnArr;
    for(unsigned int i=0;i<dimHauteur;++i)
    {
        for(unsigned int j=0;j<dimLargeur;++j)
        {
            returnArr.append(QString::fromStdString(std::to_string(valeur[i][j])));
        }
    }
    return returnArr;
}



std::ostream& operator<<(std::ostream& f, const Etat& e)
{
    f << "\nMin : " << e.getMin() << " Max : " << e.getMax() << "\n";
    for (unsigned int i = 0; i < e.getHauteur(); i++)
    {
//        for (unsigned int j = 0; j < e.getLargeur(); j++)
//            if (e.getCellule(i, j))
//                f << e.getCellule(i, j) << " ";
//            else
//                f << "0 "; //ici c'est pour bien afficher
//        f << "\n";
        for (unsigned int j = 0; j < e.getLargeur(); j++)
            f << e.getCellule(i, j) << " ";
        f << "\n";
    }
    return f;
}




