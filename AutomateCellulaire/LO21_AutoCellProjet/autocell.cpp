#include "ui_autocell.h"
#include <iostream>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include "autocell.h"


AutoCell::AutoCell()
{
    simul=nullptr;
    stateWindow=nullptr;
    numR1=0;
    numR2=0;

    minDimension = 10;
    size=20;
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    createMenu();

    stateTable=new QTableWidget(minDimension,minDimension,centralWidget);
    stateTable->setMinimumSize(minDimension*size+2, minDimension*size+2);
    stateTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    stateTable->horizontalHeader()->setVisible(false);
    stateTable->verticalHeader()->setVisible(false);
    stateTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    stateTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    stateTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // désactive la modification par l'utilisateur
    // on va créer les items, on utilise 2 boucles car on parcourt un tableau 2 dimensions
    for(unsigned int ligne = 0; ligne < minDimension; ++ligne) {
        // fixe les dimensions des lignes et des colonnes
        stateTable->setColumnWidth(ligne, size);
        stateTable->setRowHeight(ligne, size);
        for(unsigned int colonne = 0; colonne < minDimension; ++colonne) {
            stateTable->setItem(ligne, colonne, new QTableWidgetItem(""));
            stateTable->item(ligne, colonne)->setBackgroundColor(QColor(255,255,255));
        }
    }

    bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->addWidget(topFiller);
    layout->addWidget(stateTable);
    layout->addWidget(bottomFiller);
    centralWidget->setLayout(layout);

    QFile file("save.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data=file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj(doc.object());
    if(obj.size()>0 && obj.contains("context"))
    {
        QJsonObject context(obj.find("context").value().toObject());
        type=context.find("type").value().toString();
        if(type!="AutomateDeuxDimension"){
            numR1=NumBitToNum("0"+context.find("numBit").value().toString().toStdString());
        }
        else
        {
            numR1=NumBitToNum(context.find("numBitVivant").value().toString().toStdString());
            numR2=NumBitToNum(context.find("numBitMorte").value().toString().toStdString());
        }
        heightNbCells=context.find("hauteur").value().toString().toInt();
        widthNbCells=context.find("largeur").value().toString().toInt();
        initStateTable=new QTableWidget(heightNbCells,widthNbCells);
        QJsonArray init=context.find("initialState").value().toArray();
        if(type=="AutomateDeuxDimension"){
            for(unsigned int row=0;row<heightNbCells;++row)
            {
                for(unsigned int column=0;column<widthNbCells;++column)
                {
                    initStateTable->setItem(row,column,new QTableWidgetItem(""));
                    if(init.at(row*heightNbCells+column).toString()=="1"){
                        initStateTable->item(row,column)->setBackgroundColor(QColor(0,0,0));
                    }
                }
            }
        }
        else if(type=="AutomatePlusieursEtats")
        {
            for(unsigned int column=0;column<widthNbCells;++column)
            {
                initStateTable->setItem(0,column,new QTableWidgetItem(init.at(column).toString()));
            }
        }
        else if(type=="AutomateUnDimension")
        {
            for(unsigned int column=0;column<widthNbCells;++column)
            {
                initStateTable->setItem(0,column,new QTableWidgetItem(""));
                if(init.at(column).toString()=="1"){
                    initStateTable->item(0,column)->setBackgroundColor(QColor(0,0,0));
                }
            }
        }
        launchSimulation();
        simul->run(context.find("rang").value().toString().toInt());
        initializeStateTable();
    }
}


void AutoCell::createMenu()
{
    newAutomate=new QAction(tr("Nouvel Automate"),this);
    newAutomate->setShortcut(QKeySequence::New);
    newAutomate->setStatusTip("Créer un nouvel automate");
    connect(newAutomate,SIGNAL(triggered(bool)),this,SLOT(newAutomateClicked()));

    loadAutomate=new QAction(tr("Charger Automate"),this);
    loadAutomate->setShortcut(QKeySequence::Open);
    loadAutomate->setStatusTip("Charger un automate sauvegardé");
    connect(loadAutomate,SIGNAL(triggered(bool)),this,SLOT(loadAutomateClicked()));

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAutomate);
    fileMenu->addAction(loadAutomate);
}

AutoCell::~AutoCell()
{
    if(simul!=nullptr)
    {
        QFile file("save.json");
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray data=file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        QJsonObject obj(doc.object());
        QJsonObject simulJson(simul->saveAll());
        obj.insert("context",simulJson);
        doc.setObject(obj);
        //on vide le fichier avant de le réécrire
        file.resize(0);
        file.write(doc.toJson());
        file.close();
    }
}

void AutoCell::saveConfiguration()
{
    if(chrono->isActive())
        chrono->stop();
    QFile file("save.json");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray data=file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj(doc.object());
    simul->saveConfiguration(obj);
    doc.setObject(obj);
    //on vide le fichier avant de le réécrire
    file.resize(0);
    file.write(doc.toJson());
    file.close();
}

void AutoCell::loadOneD(const QModelIndex& index){
    type="AutomateUnDimension";
    QString numbit(unDTable->item(index.row(),index.column())->text());
    loadWindow->close();
    newAutomateClicked();

    for(int i=0;i<8;++i)
    {
        numeroBitR1[i+1]->setText(numbit.at(i));
        numeroBitR1[i+1]->setEnabled(false);
    }
    automateTypes->setEnabled(false);
    numR1Spin->setEnabled(false);
}

void AutoCell::loadTwoD(const QModelIndex& index){
    type="AutomateDeuxDimension";
    loadWindow->close();
    QString numbitVivant(deuxDTable->item(index.row(),0)->text());
    QString numbitMort(deuxDTable->item(index.row(),1)->text());
    newAutomateClicked();
    instanciate2D();
    for(int i=0;i<9;++i)
    {
        numeroBitR1[i]->setText(numbitVivant.at(i));
        numeroBitR2[i]->setText(numbitMort.at(i));
        numeroBitR1[i]->setEnabled(false);
        numeroBitR2[i]->setEnabled(false);
    }
    automateTypes->setEnabled(false);
    numR1Spin->setEnabled(false);
    numR2Spin->setEnabled(false);
    lifeGameAutomateCBox->setEnabled(false);
}

void AutoCell::loadMState(const QModelIndex& index){
    type="AutomatePlusieursEtats";

    loadWindow->close();
    QString numbit(mStateTable->item(index.row(),index.column())->text());
    loadWindow->close();
    newAutomateClicked();
    instanciateMState();
    for(int i=0;i<8;++i)
    {
        numeroBitR1[i+1]->setText(numbit.at(i));
        numeroBitR1[i+1]->setEnabled(false);
    }
    automateTypes->setEnabled(false);
    numR1Spin->setEnabled(false);
}

void AutoCell::loadAutomateClicked(){
    loadWindow=new QWidget();
    loadLayout=new QVBoxLayout(loadWindow);
    QFile fichier("save.json");
    fichier.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray data(fichier.readAll());
    fichier.close();
    QJsonDocument document(QJsonDocument::fromJson(data));
    QJsonObject object = document.object(); //A la racine on a bien un objet, Cet objet contient 3 tableaux :
    int loadSize=100;
    if(object.size()>0)
    {
        loadTitle=new QLabel("Double-clic pour choisir un automate: ",loadWindow);
        loadLayout->addWidget(loadTitle);
        if(!object["AutomateUnDimension"].isUndefined() && object["AutomateUnDimension"].isArray())
        {
            QJsonArray tab1 = object["AutomateUnDimension"].toArray(); // tableau 1 : "AutomateUnDimension"
            unDLabel=new QLabel("Automate une dimension: ",loadWindow);
            unDTable=new QTableWidget(loadWindow);
            unDTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            unDTable->horizontalHeader()->setVisible(false);
            //unDTable->verticalHeader()->setVisible(false);
            unDTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            unDTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // désactive la modification par l'utilisateur
            // on va créer les items, on utilise 2 boucles car on parcourt un tableau 2 dimensions
            unDTable->setRowCount(tab1.size());
            unDTable->setColumnCount(1);
            for(unsigned int i=0; i < tab1.size(); i++)
            {
                QJsonObject objectRule = tab1[i].toObject();
                QString numBit = objectRule["numeroBit"].toString(); //On récupere les "numeroBit" en QString dans la variable numBit
                //        numBit.toStdString(); //On converti notre QString en string
                //        AutomateManager::getAutomateUnDimension(numBit) //Ici on envoie le numbit à l'automateManager
                unDTable->setItem(i,0,new QTableWidgetItem(numBit));
                unDTable->setRowHeight(i,30);
            }
            if(unDTable->rowCount()<=5)
                unDTable->setMaximumSize(unDTable->columnWidth(0)*2,unDTable->rowCount()*30+2);
            else
                unDTable->setMaximumSize(unDTable->columnWidth(0)*2,150);
            unDTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            connect(unDTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(loadOneD(QModelIndex)));
            loadLayout->addWidget(unDLabel);
            loadLayout->addWidget(unDTable);

        }

        if(!object["AutomateDeuxDimension"].isUndefined() && object["AutomateDeuxDimension"].isArray())
        {
            QJsonArray tab2 = object["AutomateDeuxDimension"].toArray(); // tableau 1 : "AutomateUnDimension"
            deuxDLabel=new QLabel("Automate deux dimensions: ",loadWindow);
            deuxDTable=new QTableWidget(loadWindow);
            deuxDTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            deuxDTable->horizontalHeader()->setVisible(false);
            deuxDTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            deuxDTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // désactive la modification par l'utilisateur

            deuxDTable->setRowCount(tab2.size());
            deuxDTable->setColumnCount(2);
            for(unsigned int i=0; i < tab2.size(); i++)
            {
                QJsonObject ObjectRules = tab2[i].toObject();
                QString numBitVivant = ObjectRules["numBitCellVivant"].toString(); //On récupere les "numeroBitVivant" en QString dans la variable numBitVivant
                QString numBitMort = ObjectRules["numBitCellMorte"].toString(); //On récupere les "numeroBitMort" en QString dans la variable numBitMort
                //        numBit.toStdString(); //On converti notre QString en string
                //        AutomateManager::getAutomateUnDimension(numBit) //Ici on envoie le numbit à l'automateManager
                deuxDTable->setItem(i,0,new QTableWidgetItem(numBitVivant));
                deuxDTable->setItem(i,1,new QTableWidgetItem(numBitMort));
                deuxDTable->setRowHeight(i,40);
            }
            if(deuxDTable->rowCount()<=5)
                deuxDTable->setMaximumSize(200,deuxDTable->rowCount()*30+2);
            else
                deuxDTable->setMaximumSize(200,150);
            deuxDTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            connect(deuxDTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(loadTwoD(QModelIndex)));
            loadLayout->addWidget(deuxDLabel);
            loadLayout->addWidget(deuxDTable);
        }

        if(!object["AutomatePlusieursEtats"].isUndefined() && object["AutomatePlusieursEtats"].isArray())
        {
            QJsonArray tab3 = object["AutomatePlusieursEtats"].toArray();// tableau 3 : "AutomatePlusieursEtats"
            mStateLabel=new QLabel("Automate une dimension, plusieurs états: ",loadWindow);
            mStateTable=new QTableWidget(loadWindow);
            mStateTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
            mStateTable->horizontalHeader()->setVisible(false);
            //mStateTable->verticalHeader()->setVisible(false);
            mStateTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            mStateTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // désactive la modification par l'utilisateur

            mStateTable->setRowCount(tab3.size());
            mStateTable->setColumnCount(1);
            for(unsigned int i=0; i < tab3.size(); i++)
            {
                QJsonObject ObjectRules = tab3[i].toObject();
                QString numBitPE = ObjectRules["numeroBit"].toString(); //On récupere les "numeroBit" en QString dans la variable numBitPE
        //        numBitPE.toStdString(); //On converti notre QString en string
        //        AutomateManager::getAutomatePlusieursEtats(numBitPE);//Ici on envoie le numbit à l'automateManage
                mStateTable->setItem(i,0,new QTableWidgetItem(numBitPE));
                mStateTable->setRowHeight(i,40);
            }
            if(mStateTable->rowCount()<=5)
                mStateTable->setMaximumSize(mStateTable->columnWidth(0)*2,mStateTable->rowCount()*30+2);
            else
                mStateTable->setMaximumSize(mStateTable->columnWidth(0)*2,150);
            mStateTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            connect(mStateTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(loadMState(QModelIndex)));
            loadLayout->addWidget(mStateLabel);
            loadLayout->addWidget(mStateTable);
        }
    }
    else
    {
        loadTitle=new QLabel("Aucun automate enregistré.",loadWindow);
        loadLayout->addWidget(loadTitle);
    }
    loadWindow->setLayout(loadLayout);
    loadWindow->show();
}

void AutoCell::newAutomateClicked()
{
    configWindow=new QWidget();
    automateTypes=new QGroupBox("Type d'automate:",configWindow);
    oneDautomateRButton=new QRadioButton("1D",configWindow);
    oneDautomateRButton->setChecked(true);
    connect(oneDautomateRButton,SIGNAL(clicked(bool)),this,SLOT(instanciate1D()));
    twoDautomateRButton=new QRadioButton("2D",configWindow);
    connect(twoDautomateRButton,SIGNAL(clicked(bool)),this,SLOT(instanciate2D()));
    mStateAutomateRButton=new QRadioButton("Multiple States",configWindow);
    connect(mStateAutomateRButton,SIGNAL(clicked(bool)),this,SLOT(instanciateMState()));
    buttonLayout=new QHBoxLayout(configWindow);
    buttonLayout->addWidget(oneDautomateRButton);
    buttonLayout->addWidget(twoDautomateRButton);
    buttonLayout->addWidget(mStateAutomateRButton);
    automateTypes->setLayout(buttonLayout);
    configLayout=new QVBoxLayout(configWindow);
    configLayout->addWidget(automateTypes);
    type="AutomateUnDimension";
    //Règle 1
    numR1Spin = new QSpinBox(this);
    numR1Spin->setRange(0, 255);
    numR1Spin->setValue(0);
    numlR1 = new QLabel("Numéro", this);
    numcR1 = new QVBoxLayout;
    numcR1->addWidget(numlR1);
    numcR1->addWidget(numR1Spin);
    numerocR1 = new QHBoxLayout;
    numerocR1->addLayout(numcR1);
    zeroOneValidator = new QIntValidator(this);
    zeroOneValidator->setRange(0, 1);
    numeroBitlR1[0] = new QLabel("---");
    numeroBitlR1[1] = new QLabel("111");
    numeroBitlR1[2] = new QLabel("110");
    numeroBitlR1[3] = new QLabel("101");
    numeroBitlR1[4] = new QLabel("100");
    numeroBitlR1[5] = new QLabel("011");
    numeroBitlR1[6] = new QLabel("010");
    numeroBitlR1[7] = new QLabel("001");
    numeroBitlR1[8] = new QLabel("000");
    for(unsigned int counter = 0; counter < 9; ++counter) {
        numeroBitR1[counter] = new QLineEdit(this);
        numeroBitR1[counter]->setFixedWidth(20); //pixels
        numeroBitR1[counter]->setMaxLength(1); //caractères
        numeroBitR1[counter]->setText("0");
        numeroBitR1[counter]->setValidator(zeroOneValidator);
        bitcR1[counter] = new QVBoxLayout;
        bitcR1[counter]->addWidget(numeroBitlR1[counter]);
        bitcR1[counter]->addWidget(numeroBitR1[counter]);
        numerocR1->addLayout(bitcR1[counter]);
        connect(numeroBitR1[counter], SIGNAL(textChanged(QString)), this, SLOT(syncNumR1(QString)));
    }
    numeroBitR1[0]->setEnabled(false);
    connect(numR1Spin, SIGNAL(valueChanged(int)), this, SLOT(syncNumBitR1(int)));


    //Règle2
    numR2Spin = new QSpinBox(this);
    numR2Spin->setRange(0, 511);
    numR2Spin->setValue(0);
    numlR2 = new QLabel("Numéro", this);
    numcR2 = new QVBoxLayout;
    numcR2->addWidget(numlR2);
    numcR2->addWidget(numR2Spin);
    numerocR2 = new QHBoxLayout;
    numerocR2->addLayout(numcR2);
    numeroBitlR2[0] = new QLabel("8");
    numeroBitlR2[1] = new QLabel("7");
    numeroBitlR2[2] = new QLabel("6");
    numeroBitlR2[3] = new QLabel("5");
    numeroBitlR2[4] = new QLabel("4");
    numeroBitlR2[5] = new QLabel("3");
    numeroBitlR2[6] = new QLabel("2");
    numeroBitlR2[7] = new QLabel("1");
    numeroBitlR2[8] = new QLabel("0");
    for(unsigned int counter = 0; counter < 9; ++counter) {
        numeroBitR2[counter] = new QLineEdit(this);
        numeroBitR2[counter]->setFixedWidth(20); //pixels
        numeroBitR2[counter]->setMaxLength(1); //caractères
        numeroBitR2[counter]->setText("0");
        numeroBitR2[counter]->setValidator(zeroOneValidator);
        numeroBitR2[counter]->setEnabled(false);
        bitcR2[counter] = new QVBoxLayout;
        bitcR2[counter]->addWidget(numeroBitlR2[counter]);
        bitcR2[counter]->addWidget(numeroBitR2[counter]);
        numerocR2->addLayout(bitcR2[counter]);
        connect(numeroBitR2[counter], SIGNAL(textChanged(QString)), this, SLOT(syncNumR2(QString)));
    }
    connect(numR2Spin, SIGNAL(valueChanged(int)), this, SLOT(syncNumBitR2(int)));
    numR2Spin->setEnabled(false);
    lifeGameAutomateCBox=new QCheckBox("Jeu de la vie",configWindow);

    connect(lifeGameAutomateCBox,SIGNAL(clicked(bool)),this,SLOT(instanciateLifeGame()));
    lifeGameAutomateCBox->setEnabled(false);

    heightLab=new QLabel("Hauteur");
    heightSpin=new QSpinBox(configWindow);
    heightSpin->setRange(1,100);
    heightSpin->setValue(1);
    heightSpin->setEnabled(false);
    widthLab=new QLabel("Largeur");
    widthSpin=new QSpinBox(configWindow);
    widthSpin->setRange(10,100);
    dimContainer[0]=new QVBoxLayout;
    dimContainer[0]->addWidget(widthLab);
    dimContainer[0]->addWidget(widthSpin);
    dimContainer[1]=new QVBoxLayout;
    dimContainer[1]->addWidget(heightLab);
    dimContainer[1]->addWidget(heightSpin);
    dimLayout=new QHBoxLayout;
    dimLayout->addLayout(dimContainer[0]);
    dimLayout->addLayout(dimContainer[1]);

    initButton=new QPushButton("Suivant",configWindow);
    connect(initButton,SIGNAL(clicked(bool)),this,SLOT(defineInitialState()));
    configLayout->addLayout(numerocR1);
    configLayout->addLayout(numerocR2);
    configLayout->addWidget(lifeGameAutomateCBox);
    configLayout->addLayout(dimLayout);
    configLayout->addWidget(initButton);
    configWindow->setLayout(configLayout);
    configWindow->show();
}

void AutoCell::instanciate1D(){
    type="AutomateUnDimension";
    for(int count=0;count<9;count++){
        numeroBitR2[count]->setEnabled(false);
        numeroBitR1[count]->setEnabled(true);
    }
    numR2Spin->setEnabled(false);
    numR1Spin->setEnabled(true);
    numR1Spin->setValue(0);
    numR1Spin->setRange(0,255);
    numeroBitR1[0]->setEnabled(false);
    numeroBitR1[0]->setText("0");
    numeroBitlR1[0]->setText("---");
    numeroBitlR1[1]->setText("111");
    numeroBitlR1[2]->setText("110");
    numeroBitlR1[3]->setText("101");
    numeroBitlR1[4]->setText("100");
    numeroBitlR1[5]->setText("011");
    numeroBitlR1[6]->setText("010");
    numeroBitlR1[7]->setText("001");
    numeroBitlR1[8]->setText("000");
    lifeGameAutomateCBox->setEnabled(false);
    lifeGameAutomateCBox->setChecked(false);
    heightSpin->setRange(1,100);
    heightSpin->setValue(1);
    heightSpin->setEnabled(false);
}

void AutoCell::instanciate2D(){
    type="AutomateDeuxDimension";
    if(!lifeGameAutomateCBox->isChecked()){
        for(int count=0;count<9;count++)
            numeroBitR2[count]->setEnabled(true);
        numR2Spin->setEnabled(true);
        numR1Spin->setRange(0,511);
        numeroBitR1[0]->setEnabled(true);
        numeroBitlR1[0]->setText("8");
        numeroBitlR1[1]->setText("7");
        numeroBitlR1[2]->setText("6");
        numeroBitlR1[3]->setText("5");
        numeroBitlR1[4]->setText("4");
        numeroBitlR1[5]->setText("3");
        numeroBitlR1[6]->setText("2");
        numeroBitlR1[7]->setText("1");
        numeroBitlR1[8]->setText("0");
        lifeGameAutomateCBox->setEnabled(true);
        heightSpin->setEnabled(true);
        heightSpin->setValue(2);
        heightSpin->setRange(2,100);
    }
}
void AutoCell::instanciateLifeGame(){
    if(lifeGameAutomateCBox->isChecked())
    {
        numR1Spin->setValue(12);
        numR2Spin->setValue(8);
        numR1Spin->setEnabled(false);
        numR2Spin->setEnabled(false);
        for(int count=0;count<9;count++)
        {
            numeroBitR1[count]->setEnabled(false);
            numeroBitR2[count]->setEnabled(false);
        }
    }
    else
    {
        numR1Spin->setEnabled(true);
        numR2Spin->setEnabled(true);
        for(int count=0;count<9;count++)
        {
            numeroBitR1[count]->setEnabled(true);
            numeroBitR2[count]->setEnabled(true);
        }
    }
}

void AutoCell::instanciateMState(){
    instanciate1D();
    type="AutomatePlusieursEtats";
    numeroBitlR1[1]->setText("PPP");
    numeroBitlR1[2]->setText("PPI");
    numeroBitlR1[3]->setText("PIP");
    numeroBitlR1[4]->setText("PII");
    numeroBitlR1[5]->setText("IPP");
    numeroBitlR1[6]->setText("IPI");
    numeroBitlR1[7]->setText("IIP");
    numeroBitlR1[8]->setText("III");
}

void AutoCell::defineInitialState(){
    this->size=20;
    if(heightNbCells*size>800){
        size=800/heightNbCells;
    }
    else if(widthNbCells*size>1500){
        size=1500/widthNbCells;
    }
    heightNbCells=heightSpin->value();
    widthNbCells=widthSpin->value();
    configWindow->close();
    stateWindow=new QWidget();
    zeroNineNineValidator= new QIntValidator(0,99,stateWindow);
    initStateTable = new QTableWidget(heightNbCells,widthNbCells,stateWindow);
    initStateTable->setFixedSize(widthNbCells*size,heightNbCells*size);
    initStateTable->horizontalHeader()->setVisible(false);
    initStateTable->verticalHeader()->setVisible(false);
    initStateTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    initStateTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    initStateTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // désactive la modification par l'utilisateur
    // on va créer les items, on utilise 2 boucles car on parcourt un tableau 2 dimensions
    for(unsigned int row = 0; row < heightNbCells; ++row) {
        // fixe les dimensions des lignes
        initStateTable->setRowHeight(row, size);
        for(unsigned int column = 0; column < widthNbCells; ++column) {
            initStateTable->setColumnWidth(column, size);
            initStateTable->setItem(row, column, new QTableWidgetItem(""));
            initStateTable->item(row, column)->setBackgroundColor(QColor(255,255,255));
        }
    }
    connect(initStateTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(cellActivation(QModelIndex)));
    titleLabel=new QLabel("Choississez votre état initial:",stateWindow);
    randomButton=new QPushButton("Générer un état aléatoire",stateWindow);
    connect(randomButton,SIGNAL(clicked(bool)),this,SLOT(randomStateClicked()));
    launchButton=new QPushButton("Lancer la simulation !",stateWindow);
    connect(launchButton,SIGNAL(clicked(bool)),this,SLOT(launchSimulation()));
    stateLayout=new QVBoxLayout;
    stateLayout->addWidget(titleLabel);
    stateLayout->addWidget(initStateTable);
    stateLayout->addWidget(randomButton);
    stateLayout->addWidget(launchButton);
    stateWindow->setLayout(stateLayout);
    stateWindow->show();
}

void AutoCell::randomStateClicked()
{
    if(type!="AutomatePlusieursEtats")
    {
        for(unsigned int row=0;row<initStateTable->rowCount();++row)
        {
            for(unsigned int column=0;column<initStateTable->columnCount();++column)
            {
                int randomResult=255*(std::rand()%2);
                initStateTable->item(row,column)->setBackgroundColor(QColor(randomResult,randomResult,randomResult));
            }
        }
    }
    else
    {
        for(unsigned int row=0;row<initStateTable->rowCount();++row)
        {
            for(unsigned int column=0;column<initStateTable->columnCount();++column)
            {
                int randomResult=std::rand()%100;
                initStateTable->item(row,column)->setText(QString::fromStdString(std::to_string(randomResult)));
            }
        }
    }
}

void AutoCell::launchSimulation(){
    if(stateWindow!=nullptr){
        stateWindow->close();
    }
    if(type=="AutomateUnDimension")
    {
        simul=new Simulateur(AutomateManager::getAutomateManager().getAutomateUnDimension(numR1),Etat(AutoCell::tableToBinaryState(*initStateTable)),10);
    } else if(type=="AutomateDeuxDimension")
    {
        simul=new Simulateur(AutomateManager::getAutomateManager().getAutomateDeuxDimension(numR1,numR2),Etat(AutoCell::tableToBinaryState(*initStateTable)));
    } else if(type=="AutomatePlusieursEtats")
    {
        simul=new Simulateur(AutomateManager::getAutomateManager().getAutomatePlusieursEtats(numR1),Etat(AutoCell::tableToMultipleState(*initStateTable)),10);
    } else throw AutoCellException("Invalid automate type");
    initializeStateTable();
    if(bottomFiller->layout()==nullptr)
    {
        saveConfigButton=new QPushButton("Sauvegarder",bottomFiller);
        connect(saveConfigButton,SIGNAL(clicked(bool)),this,SLOT(saveConfiguration()));
        resetButton=new QPushButton("Reset",bottomFiller);
        connect(resetButton,SIGNAL(clicked(bool)),this,SLOT(reset()));
        nextStepButton=new QPushButton("Étape Suivante",bottomFiller);
        connect(nextStepButton,SIGNAL(clicked(bool)),this,SLOT(oneStepFurther()));
        autoRunButton=new QPushButton("Autorun",bottomFiller);
        connect(autoRunButton,SIGNAL(clicked(bool)),this,SLOT(autoRun()));
        frequenceSlider=new QSlider(Qt::Horizontal,bottomFiller);
        frequenceSlider->setMinimumWidth(50);
        frequenceSlider->setMinimum(50);
        frequenceSlider->setMaximum(2000);
        connect(frequenceSlider,SIGNAL(valueChanged(int)),this,SLOT(syncFrequence()));
        chrono=new QTimer(bottomFiller);
        chrono->setInterval(100);
        connect(chrono,SIGNAL(timeout()),this,SLOT(oneStepFurther()));
        runLayouts[0]=new QHBoxLayout;
        runLayouts[0]->addWidget(nextStepButton);
        runLayouts[0]->addWidget(autoRunButton);
        runLayouts[0]->addWidget(frequenceSlider);
        runLayouts[1]=new QHBoxLayout;
        runLayouts[1]->addWidget(resetButton);
        runLayouts[1]->addWidget(saveConfigButton);
        bottomLayout=new QVBoxLayout;
        bottomLayout->addLayout(runLayouts[0]);
        bottomLayout->addLayout(runLayouts[1]);
        bottomFiller->setLayout(bottomLayout);
    }
}

void AutoCell::reset()
{
    if(chrono->isActive()) chrono->stop();
    simul->reset();
    initializeStateTable();
}

void AutoCell::oneStepFurther(){
    simul->next();
    initializeStateTable();
}

void AutoCell::autoRun(){
    if(chrono->isActive()){
        chrono->stop();
    }
    else{
        chrono->start();
    }
}

void AutoCell::syncFrequence(){
    chrono->setInterval(frequenceSlider->value());
}

void AutoCell::cellActivation(const QModelIndex& index){

    if(!mStateAutomateRButton->isChecked())
    {
        if (initStateTable->item(index.row(), index.column())->backgroundColor() == QColor(0,0,0)) {
            initStateTable->item(index.row(), index.column())->setBackgroundColor(QColor(255,255,255));
        } else {
            initStateTable->item(index.row(), index.column())->setBackgroundColor(QColor(0,0,0));
        }
    }
    else
    {
        int nb=0;
        QTableWidgetItem* item=initStateTable->item(index.row(),index.column());
        if(item->text()!="")
            nb=item->text().toInt();
        nb++;
        nb=nb%100;
        item->setText(QString::fromStdString(std::to_string(nb)));
    }
}

void AutoCell::syncNumR1(const QString& s){
    synchronizeNumBitToNum(s,numR1Spin,numeroBitR1);
}
void AutoCell::syncNumR2(const QString& s){
    synchronizeNumBitToNum(s,numR2Spin,numeroBitR2);
}
void AutoCell::syncNumBitR1(int n){
    numR1=n;
    synchronizeNumToNumBit(n,numeroBitR1);
}
void AutoCell::syncNumBitR2(int n){
    numR2=n;
    synchronizeNumToNumBit(n,numeroBitR2);
}

void AutoCell::synchronizeNumToNumBit(int n, QLineEdit** edit){
    std::string numBit = NumToNumBit(n);
    for(unsigned int counter = 0; counter < 9; ++counter) {
        edit[counter]->setText(QString(numBit[counter]));
    }
}
void AutoCell::synchronizeNumBitToNum(const QString& s, QSpinBox* spin, QLineEdit** edit){
    if (s == "") {
        return;
    }
    std::string numBit = "";
    for(unsigned int counter = 0; counter < 9; ++counter) {
        numBit += edit[counter]->text().toStdString();
    }
    spin->setValue(NumBitToNum(numBit));

}

short unsigned int NumBitToNum(const std::string& num) {
    if (num.size() != 9) throw AutoCellException("Numero d'automate indefini");
    int puissance = 1;
    short unsigned int numero = 0;
    for (int i = 8; i >= 0; i--) {
        if (num[i] == '1') numero += puissance;
        else if (num[i] != '0') throw AutoCellException("Numero d'automate indefini");
        puissance *= 2;
    }
    return numero;
}

std::string NumToNumBit(short unsigned int num) {
    std::string numeroBit;
    if (num > 512) throw AutoCellException("Numero d'automate indefini");
    unsigned short int p = 256;
    int i = 8;
    while (i >= 0) {
        if (num >= p) {
            numeroBit.push_back('1');
            num -= p;
        }
        else { numeroBit.push_back('0'); }
        i--;
        p = p / 2;
    }
    return numeroBit;
}

Etat AutoCell::tableToBinaryState(const QTableWidget& tab){
    Etat returnState(tab.rowCount(),tab.columnCount());
    for(int rows=tab.rowCount()-1;rows>=0;rows--)
    {
        for(int columns=tab.columnCount()-1;columns>=0;columns--)
        {
                if(tab.item(rows,columns)->backgroundColor()==QColor(0,0,0))
                    returnState.setCellule(rows,columns,1);
        }
    }
    return returnState;
}

Etat AutoCell::tableToMultipleState(const QTableWidget& tab){
    Etat returnState(tab.rowCount(),tab.columnCount(),0,99);
    for(int rows=tab.rowCount()-1;rows>=0;rows--)
    {
        for(int columns=tab.columnCount()-1;columns>=0;columns--)
        {
            returnState.setCellule(rows,columns,tab.item(rows,columns)->text()==""?0:tab.item(rows,columns)->text().toInt());
        }
    }
    return returnState;
}

void AutoCell::initializeStateTable(){
    if(heightNbCells>=10)
        stateTable->setMinimumSize(widthNbCells*size+2,heightNbCells*size+2);
    else
        stateTable->setMinimumSize(widthNbCells*size+2,10*size+2);
    if(type=="AutomateDeuxDimension")
        stateTable->setRowCount(heightNbCells);
    else
        stateTable->setColumnCount(10);
    stateTable->setColumnCount(widthNbCells);
    if(type!="AutomateDeuxDimension")
    {
        Simulateur::Iterator it=simul->getIterator();
        stateTable->setRowCount(10);
        for(unsigned int row=0;row<10;++row){
            stateTable->setRowHeight(row,size);
            for(unsigned int column=0;column<widthNbCells;++column)
            {
                stateTable->setColumnWidth(column,size);
                if(!it.isDone())
                {
                    if(type=="AutomatePlusieursEtats")
                    {
                        if(stateTable->item(row,column)==0){
                            stateTable->setItem(row,column,new QTableWidgetItem(it.current().getCellule(0,column)));
                        }
                        stateTable->item(row,column)->setBackgroundColor(QColor(255,255,255));
                        stateTable->item(row,column)->setText(QString::fromStdString(std::to_string(it.current().getCellule(0,column))));
                    } else if(type=="AutomateUnDimension")
                    {
                        if(stateTable->item(row,column)==0)
                            stateTable->setItem(row,column,new QTableWidgetItem(""));
                        stateTable->item(row,column)->setText("");
                        if(it.current().getCellule(0,column)==1)
                            stateTable->item(row,column)->setBackgroundColor(QColor(0,0,0));
                        else
                            stateTable->item(row,column)->setBackgroundColor(QColor(255,255,255));
                    }
                }
                else
                {
                    if(stateTable->item(row,column)==0)
                        stateTable->setItem(row,column,new QTableWidgetItem(""));
                    else{
                        stateTable->item(row,column)->setBackgroundColor(QColor(255,255,255));
                        stateTable->item(row,column)->setText("");
                    }
                }
            }
            if(!it.isDone())
                it.next();
        }
    }
    else{
        stateTable->setRowCount(heightNbCells);
        stateTable->setColumnCount(widthNbCells);
        for(unsigned int row=0;row<heightNbCells;++row)
        {
            stateTable->setRowHeight(row,size);
            for(unsigned int column=0;column<widthNbCells;++column)
            {
                stateTable->setColumnWidth(column,size);
                if(stateTable->item(row,column)==0)
                    stateTable->setItem(row,column,new QTableWidgetItem(""));
                stateTable->item(row,column)->setText("");
                if(simul->dernier().getCellule(row,column)==1){
                    stateTable->item(row,column)->setBackgroundColor(QColor(0,0,0));
                }
                else if(stateTable->item(row,column)->backgroundColor()==QColor(0,0,0)){
                    stateTable->item(row,column)->setBackgroundColor(QColor(255,255,255));
                }
            }
        }
    }
}
