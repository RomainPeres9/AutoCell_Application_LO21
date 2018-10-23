#ifndef Simulateur_hpp
#define Simulateur_hpp
#include <QJsonObject>
#include <stdio.h>
#include "Automate.hpp"

/**
*  @class Simulateur
*  @brief Une classe simulant l'exécution d'automates cellulaires de tous les types
*  @details on enlève le possibilité de construire un simulateur par duplication ou par affectation.
*  On permettra d'appliquer un objet Automate sur plusieurs générations à partir d'un objet Etat représentant une grille de départ. Grâce à un buffer un objet Simulateur est capable de garder un nombre donné des derniers états générés.
*/
class Simulateur
{
private:
    /// l'Automate que l'on va utiliser dans cette simulation
    const Automate& automate;
    /// le buffer gardant un nombre donné des derniers états générés
    Etat** etats = nullptr;
    /// la grille de départ
    const Etat* depart = nullptr;
    /// le capacité maximum du buffer
    unsigned int nbMaxEtats;
    /// Représente le rang du dernier état généré
    unsigned int rang = 0;
    
    Simulateur(const Simulateur& s) = delete;
    Simulateur& operator=(const Simulateur& s) = delete;
    /// créer un état vide à la position c dans le buffer etats
    void build(unsigned int c);
    
public:
    ///constructeur sans état départ
    Simulateur(const Automate& a, unsigned int buffer = 2);
    ///constructeur avec état départ
    Simulateur(const Automate& a, const Etat& dep, unsigned int buffer = 2);
    ///modifier état départ avec un état représenté par e.
    void setEtatDepart(const Etat& e);
    ///génère les nbSteps prochains états
    void run(unsigned int nbSteps);
    ///génère les nbSteps prochains états
    void next();
    /// @return dernier état généré
    const Etat& dernier() const;
    /// @return Le rang du dernier état généré
    unsigned int getRangDernier() const { return rang; }
    /// relance le simulateur à l'état de départ
    void reset();
    ///@brief destructeur avec désallocation de mémoire de etats et etat de départ.
    ///@details La création et la suppression de automate est contrôlé par AutomateManager
    ~Simulateur();
    friend class Iterator;
    friend class ConstIterator;
    
    /*********** ITERARTOR **********/
    /**
    *  @class Iterator
    *  @brief design pattern iterator
    *  @details on peut modifier le simulateur contrôler par cet itérateur
    *  il permet de parcourir les derniers états générés depuis le dernier généré jusqu'au plus ancien encore présent dans le buffer
    */
    class Iterator
    {
    private:
        friend class Simulateur;
        ///Un attribut qui contient le simulateur qu'on va contrôler avec cet itérateur
        Simulateur* sim = nullptr;
        ///Un attribut entier qui représente le rang du dernier état qu'on a parcouru
        int i = 0;
        ///@brief Constructeur de Iterator
        ///@details	Il est caché à utilisateur pour ne pas créer un itérateur directement avec n'importe quel simulateur, c'est toujours le simulateur qui crée son itérateur
        Iterator(Simulateur* s) :sim(s), i(s->rang) {}
        
    public:
        Iterator() {}
        
        bool isDone() const
        {
            return sim == nullptr || (i == -1 && sim->rang<sim->nbMaxEtats) || i == sim->rang - sim->nbMaxEtats;
        }

        ///@brief aller à l'état suivant dans le buffer
        ///@details si on a déjà fini le parcours ou l'iterateur ne fait pas le lien avec son simulateur, il déclenche une exception
        void next()
        {
            if (isDone())
                throw AutomateException("error, next on an iterator which is done");
            i--;
            if (i == -1 && sim->rang >= sim->nbMaxEtats)
                i = sim->nbMaxEtats - 1;
        }
        ///retourner l'état courant
        Etat& current() const
        {
            if (isDone())
                throw AutomateException("error, indirection on an iterator which is done");
            return *sim->etats[i%sim->nbMaxEtats];
        }
    };
    
    Iterator getIterator()
    {
        return Iterator(this);
    }
    
    /*********** CONSTITERATOR **********/
    /**
    *  @class ConstIterator
    *  @brief design parttern const iterator
    *  @details on ne peut pas modifier le simulateur contrôler par cet itérateur
    *  il permets de parcourir les derniers états générés depuis le dernier généré jusqu'au plus ancien encore présent dans le buffer
    */
       class ConstIterator
       {
       private:
           friend class Simulateur;
           ///Un attribut qui contient le simulateur qu'on va contrôler avec cet itérateur
           const Simulateur* sim = nullptr;
           ///Un attribut entier qui représente le rang du dernier état qu'on a parcouru
           int i = 0;
           ///@brief Constructeur de Iterator
           ///@details	Il est caché à utilisateur pour ne pas créer un itérateur directement avec n'importe quel simulateur, c'est toujours le simulateur qui crée son itérateur
           ConstIterator(const Simulateur* s) :sim(s), i(s->rang) {}

       public:
           ConstIterator() {}
           bool isDone() const
           {
               return sim == nullptr || (i == -1 && sim->rang<sim->nbMaxEtats) || i == sim->rang - sim->nbMaxEtats;
           }
           ///@brief aller à l'état suivant dans le buffer
           ///@details si on a déjà fini le parcours ou l'iterateur ne fait pas le lien avec son simulateur, il déclenche une exception
           void next()
           {
               if (isDone())
                   throw AutomateException("error, next on an iterator which is done");
               i--;
               if (i == -1 && sim->rang >= sim->nbMaxEtats)
                   i = sim->nbMaxEtats - 1;
           }
           ///retourner l'état courant
           const Etat& current() const
           {
               if (isDone())
                   throw AutomateException("error, indirection on an iterator which is done");
               return *sim->etats[i%sim->nbMaxEtats];
           }
       };

       ConstIterator getIterator() const
       {
           return ConstIterator(this);
       }

       ConstIterator getConstIterator() const
       {
           return ConstIterator(this);
       }
    


    ///Sauvegarde dans l'objet json passé en paramètre la configuration de l'automate du simulateur
    void saveConfiguration(QJsonObject& obj) const;
    ///@return un objet json contenant tout le contenu du simulateur pour pouvoir le récupérer plutard
    QJsonObject saveAll()const;
};








#endif /* Simulateur_hpp */
