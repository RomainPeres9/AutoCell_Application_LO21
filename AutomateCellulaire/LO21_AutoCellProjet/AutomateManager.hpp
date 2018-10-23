#ifndef AutomateManager_hpp
#define AutomateManager_hpp
#include <stdio.h>
#include "Automate.hpp"

/**
*  @class AutomateManager
*  @brief Une classe qui contrôle la fabrication et suppression des automates avec le design parttern Singleton
*  @details Les fonctions getAutomate crée et le retourne un automate s'il n'existe pas dans AutomateManager ou retourne l'automate s'il existe
*/
class AutomateManager
{
private:
    ///Un tableau de stocker tous les possibilités des règles sur 1 dimensions
    AutomateUnDimension* automatesUnDimension[256];
    ///Un tableau de stocker tous les possibilités des règles sur 2 dimensions
    AutomateDeuxDimension* automatesDeuxDimension[512*512];
    ///Un tableau de stocker tous les possibilités des règles de plusieurs états
    AutomatePlusieursEtats* automatesPlusieursEtats[256];
    ///@details On cache le constructeur de AutomateManager aux utilisateurs, on laisse la structure Handler qui contrôle la fabrication de AutomateManager
    AutomateManager();
    AutomateManager(const AutomateManager& a) = delete;
    AutomateManager& operator=(const AutomateManager& a) = delete;
    ///@details On cache le destructeur de AutomateManager aux utilisateurs, on laisse la structure Handler qui contrôle la suppression de AutomateManager
    ~AutomateManager();
    /**
    * @struct Handler
    * @brief une struct static dans AutomateManager	qui contrôle fabrication et suppression d'une seule instance de classe AutomateManager
    */
    struct Handler {
        AutomateManager* instance;
        Handler() :instance(nullptr) {}
        ~Handler() { delete instance; }
    };
    ///@details on créer un attribut statistique handler pour construire une seule instance de AutomateManager quand le processus est déclenché, et le enlever quand le processus est terminé
    static Handler handler;
    
public:
    ///créer ou obtenir un AutomateUndimension avec la règle qu'on a introduit en format décimal
    const AutomateUnDimension& getAutomateUnDimension(unsigned short int num);
    ///créer ou obtenir un AutomateUndimension avec la règle qu'on a introduit en format binaire
    const AutomateUnDimension& getAutomateUnDimension(const std::string& num);
    ///créer ou obtenir un AutomateUndimension avec la règle qu'on a introduit en format binaire
    const AutomateDeuxDimension& getAutomateDeuxDimension(unsigned short int nV, unsigned short int nM);
    ///créer ou obtenir un AutomateDeuxdimension avec la règle qu'on a introduit en format binaire
    const AutomateDeuxDimension& getAutomateDeuxDimension(const std::string& BitVivant, const std::string& BitMorte);
    ///créer ou obtenir un AutomatePlusieursEtats avec la règle qu'on a introduit en format décimal
    const AutomatePlusieursEtats& getAutomatePlusieursEtats(unsigned short int num);
    ///créer ou obtenir un AutomatePlusieursEtats avec la règle qu'on a introduit en format décimal
    const AutomatePlusieursEtats& getAutomatePlusieursEtats(const std::string& num);
    ///créer un AutomateManager s'il n'existe pas dans struct Handler, et le retourner s'il existe
    static AutomateManager& getAutomateManager();
    ///Supprimer le AutomateManager (contrôlé par Handler) quand le processus est terminé
    static void libererAutomateManager();
};

#endif /* AutomateManager_hpp */
