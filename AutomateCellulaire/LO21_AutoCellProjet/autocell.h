#ifndef AUTOCELL_H
#define AUTOCELL_H

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QRandomGenerator>
#include "Automate.hpp"
#include "AutomateManager.hpp"
#include "Etat.hpp"
#include "Simulateur.hpp"

/**
 * @brief class AutoCell
 * @details herits from QMainWindow. Holds the interface of the application and all its slots.
 */
class AutoCell : public QMainWindow
{
    Q_OBJECT
    /*composition de Main Window*/
    QWidget* centralWidget;
    QVBoxLayout *layout;

    QWidget* topFiller;
    QWidget* bottomFiller;

    /* barre de menus */
    QMenuBar* menubar;
    QMenu* fileMenu;
    QAction* newAutomate;
    QAction* loadAutomate;

    /*composition du widget central*/
    QTableWidget* stateTable;
    QHBoxLayout* runLayouts[2];
    QVBoxLayout* bottomLayout;
    QPushButton* saveConfigButton;
    QPushButton* nextStepButton;
    QPushButton* autoRunButton;
    QPushButton* resetButton;
    QSlider* frequenceSlider;
    QString type;
    ///@brief chrono le compteur envoyant un signal à la fréquence de la valeur de frequenceSlider
    QTimer* chrono;
    unsigned int minDimension;
    unsigned int size;

    ///@brief le simulation de l'application initialisé à nullptr lors de la construction.
    Simulateur* simul;

    /*composition de load Window*/
    QWidget* loadWindow;
    QLabel* loadTitle;
    QVBoxLayout* loadLayout;
    QLabel* unDLabel;
    QTableWidget* unDTable;
    QLabel* deuxDLabel;
    QTableWidget* deuxDTable;
    QLabel* mStateLabel;
    QTableWidget* mStateTable;

    /*composition de Configuration Window*/
    QWidget* configWindow;
    QVBoxLayout* configLayout;
    QHBoxLayout* buttonLayout;
    QPushButton* initButton;
    //RadioButton de type d'automate
    QGroupBox* automateTypes;
    QRadioButton* oneDautomateRButton;
    QRadioButton* twoDautomateRButton;
    QRadioButton* mStateAutomateRButton;
    QCheckBox* lifeGameAutomateCBox;
    //Rules
        //Rule1
    QVBoxLayout* bitcR1[9];
    QHBoxLayout* numerocR1;
    QSpinBox* numR1Spin; // numéro
    unsigned int numR1;
    QLineEdit* numeroBitR1[9]; // un QLineEdit par bit
    QLabel* numlR1;
    QLabel* numeroBitlR1[9];
    QVBoxLayout* numcR1;
        //Rule2
    QVBoxLayout* bitcR2[9];
    QHBoxLayout* numerocR2;
    QSpinBox* numR2Spin; // numéro
    unsigned int numR2;
    QLineEdit* numeroBitR2[9]; // un QLineEdit par bit
    QLabel* numlR2;
    QLabel* numeroBitlR2[9];
    QVBoxLayout* numcR2;
        //Dimension
    QHBoxLayout* dimLayout;
    QVBoxLayout* dimContainer[2];
    QLabel* heightLab;
    QSpinBox* heightSpin;
    QLabel* widthLab;
    QSpinBox* widthSpin;
    unsigned int heightNbCells=1;
    unsigned int widthNbCells=10;
    QIntValidator* zeroOneValidator;

    //composition de Initial State window
    QWidget* stateWindow;
    QVBoxLayout* stateLayout;
    QTableWidget* initStateTable;
    QLabel* titleLabel;
    QPushButton* launchButton;
    ///@brief permet de vérifier que les cellules des automates plusieurs états soient bien entre 0 et 99
    QIntValidator* zeroNineNineValidator;
    QPushButton* randomButton;

    /**
     * @brief synchronizeNumToNumBit synchronise le tableau de QLineEdit passé en paramètre avec l'entier n passé en paramètre
     * @param n numéro pour synchroniser
     * @param edit tableau à synchroniser
     */
    void synchronizeNumToNumBit(int n, QLineEdit **edit);
    /**
     * @brief synchronizeNumBitToNum synchronise le SpinBox passé en paramètre avec le tableau QLineEdit passé en paramètre. Si s vide retour vide de la fonction
     * @param s le string u QLineEdit qui a changé de valeur. Si s vide alors la fonction s'arrête
     * @param spin la SpinBox à synchroniser
     * @param edit le tableau de QLineEdit pour synchroniser
     */
    void synchronizeNumBitToNum(const QString& s, QSpinBox* spin, QLineEdit **edit);
    /**
     * @brief initializeStateTable initialise le tableau de la fenêtre principale en fonction du type d'automate.
     * Si l'automate est à deux dimensions, initialise le tableau à la taille et hauteur renseigné.
     * Si l'automate n'a qu'une dimension, on cherche dans l'iterateur du simulateur pour complété les lignes suivantes du tableau.
     */
    void initializeStateTable();
    /**
     * @brief createMenu crée le menu dans la bar des menu en haut de la fenêtre principale.
     * Seul le menu a été implémenter permettant de cliquer sur nouvel automate ou charger automate.
     */
    void createMenu();
public:
    /**
     * @brief AutoCell constructeur de la fenêtre. Instancie la fenêtre principale avec le tableau de widget.
     * Si un fichier save.json existe, va chercher dans le fichier si un précédent contexte existe, si oui, le charge dans la fenêtre avec le simulateur jusqu'au dernier état.
     * Sinon instancie le tableau aux dimensions 10x10 à vide.
     */
    explicit AutoCell();

    /**
     * @brief Destructeur de AutoCell. Lors de la destruction de l'objet courant, sauvegarde le contexte de l'application si il existe.
     * Sauvegarde dimension, automate, état initial, et rang du dernier état pour pouvoir le charger la prochaine fois que l'on lance l'application.
     */
    ~AutoCell();
    /**
     * @brief tableToBinaryState se charge de convertir un tableau que l'on passera en paramètre en un état binaire.
     * @param tab le tableau de widget à convertir en état binaire
     * @return l'état binaire converti correspondant au tableau en paramètre
     */
    static Etat tableToBinaryState(const QTableWidget& tab);
    /**
     * @brief tableToMultipleState se charge de convertir un tableau que l'on passera en paramètre en un état multiple.
     * @param tab le tableau de widget à convertir en état multiple
     * @return l'état multiple converti correspondant au tableau en paramètre
     */
    static Etat tableToMultipleState(const QTableWidget& tab);
private slots:
    /**
     * @brief saveConfiguration sauvegarde l'automate courant (ses règles) dans le fichier save.json
     */
    void saveConfiguration();
    /**
     * @brief newAutomateClicked appelé lorsque l'on choisit nouvel automate dans la barre des menus
     * Instancie toute la fenêtre de configuration de l'automate et met le type à "AutomateUnDimension"
     */
    void newAutomateClicked();
    /**
     * @brief loadAutomateClicked appelé lorsque l'on choisit charger automate dans la barre des menus
     * Instancie la fenêtre de chargement d'automate si des automates ont précédemment été sauvegardé
     * Si aucun automate n'a été sauvegardé précédemment, affiche une page avec marqué "aucun automate sauvegardé"
     */
    void loadAutomateClicked();
    /**
     * @brief randomStateClicked appelé lorsque l'on clique sur "état aléatoire" lors du choix de l'état initial.
     * Instancie toutes les cases du tableau aléatoirement en fonction de la binarité des états. Si les états sont
     * binaire, alors à l'aide de std::rand on choisit aléatoirement l'état. Si les états sont multiples, alors toujours
     * à l'aide de std::rand on choisit un état entre 0 et 99 inclus.
     */
    void randomStateClicked();
    /**
     * @brief loadOneD charge l'automate une dimension sur lequel on a double cliqué
     * @param index l'index de l'automate cliqué dans le tableau des automates un dimension chargés
     */
    void loadOneD(const QModelIndex& index);
    /**
     * @brief loadTwoD charge l'automate deux dimensions sur lequel on a double cliqué
     * @param index l'index de l'automate cliqué dans le tableau des automates deux dimensions chargés
     */
    void loadTwoD(const QModelIndex& index);
    /**
     * @brief loadTwoD charge l'automate états multiples sur lequel on a double cliqué
     * @param index l'index de l'automate cliqué dans le tableau des automates états multiples chargés
     */
    void loadMState(const QModelIndex& index);
    /**
     * @brief defineInitialState instancie et fait apparaître la fenêtre de choix d'état initial. Instancie le tableau
     * de choix initial aux dimensions renseignés précédemment. Connecte les cases pour que lors d'un double clicked
     * l'état change en fonction du type d'automate.
     */
    void defineInitialState();
    /**
     * @brief launchSimulation créer le simulateur avec les informations stockées/renseignées puis synchronise tableau de cellules
     * avec la table d'état initial. Ferme la fenêtre de choix d'état initial si celle-ci est ouverte. Instancie les divers
     * boutons permettant d'avancer dans la simulation.
     */
    void launchSimulation();
    /**
     * @brief reset replace l'état à l'état initial et synchronise le tableau de cellules de la fenêtre principal.
     */
    void reset();
    /**
     * @brief syncNumR1 appelle la synchronisation NumBitToNum correspondante en donnant en paramètre numR1SpinBox
     * et QLineEdit numeroBitR1
     * @param s la valeur du QLineEdit qui vient de changer
     */
    void syncNumR1(const QString& s);
    /**
     * @brief syncNumR2 voir syncNumR1. Même chose mais avec la règle 2.
     * @param s
     */
    void syncNumR2(const QString& s);
    /**
     * @brief syncNumBitR1 appelle la synchronisation NumToNumBit correspondante en donnant en paramètre numR1SpinBox
     * et QLineEdit numeroBitR1
     * @param n la valeur du SpinBox qui vient de changer
     */
    void syncNumBitR1(int n);
    /**
     * @brief syncNumBitR2 voir syncNumBitR2. Même chose mais avec la règle 2.
     * @param n
     */
    void syncNumBitR2(int n);
    /**
     * @brief instanciate1D appelé lorsque l'on clique sur le radio bouton "une dimension" lors de la configuration.
     *  Bloque les règles qui ne correspondent pas à l'automate, à savoir la règle deux et abaisse la longueur de la règle à 8 bit
     */
    void instanciate1D();
    /**
     * @brief instanciate2D appelé lorsque l'on clique sur le radio bouton "deux dimensions" lors de la configuration.
     * Ouvre toutes les règles et la possibilité de changer la hauteur.
     */
    void instanciate2D();
    /**
     * @brief instanciateLifeGame lors du choix d'un automate deux dimensions, appelé lorsque qu'on coche "jeu de la vie"
     * met les règles à leur valeur correspondantes pour le jeu de la vie (à savoir 12 et 8)
     */
    void instanciateLifeGame();
    /**
     * @brief instanciate1D appelé lorsque l'on clique sur le radio bouton "plusieurs boutons" lors de la configuration.
     * Bloque les règles qui ne correspondent pas à l'automate, à savoir la règle deux et abaisse la longueur de la règle à 8 bit
     */
    void instanciateMState();
    /**
     * @brief oneStepFurther appelle le next du simulateur et synchronise avec le tableau de cellule principal
     */
    void oneStepFurther();
    /**
     * @brief autoRun lance le compteur chrono à la frequence freq renseigné par le QSlider et lance oneStepFurther
     * à chaque timeout du chrono.
     */
    void autoRun();
    /**
     * @brief syncFrequence synchronise la frequence de chrono avec la valeur du QSlider freqSlider
     */
    void syncFrequence();
    /**
     * @brief cellActivation appelé lors de l'intéraction avec le tableau d'état initial. Chaque l'état de la cellule
     * correspondante en fonction du type d'automate.
     * @param index de la cellule du tableau avec laquelle on a intéragi
     */
    void cellActivation(const QModelIndex& index);
};

/**
 * @brief The AutoCellException class
 */
class AutoCellException {
public:
    AutoCellException(const std::string& message) :info(message) {}
    std::string getInfo() const { return info; }
private:
    std::string info;
};

/**
 * @brief NumBitToNum renvoie le numéro correspondant au string (binaire) passé en paramètre
 * @param num string binaire à traduire en entier
 * @return int retourné traduit du string
 */
short unsigned int NumBitToNum(const std::string& num);
/**
 * @brief NumToNumBit fait l'inverse de NumBitToNum
 * @param num l'entier à traduire
 * @return le string traduit en binaire à partir de l'entier
 */
std::string NumToNumBit(short unsigned int num);

#endif // AUTOCELL_H
