#include "Simulateur.hpp"
#include "Automate.hpp"
#include <QJsonArray>
#include <typeinfo>

Simulateur::Simulateur(const Automate& a, unsigned int buffer) : automate(a), nbMaxEtats(buffer)
{
    etats = new Etat*[nbMaxEtats];
    for (unsigned int i = 0; i < nbMaxEtats; i++)
        etats[i] = nullptr;
}

Simulateur::Simulateur(const Automate& a, const Etat& dep, unsigned int buffer) : automate(a), depart(new Etat(dep)), nbMaxEtats(buffer)
{
    etats = new Etat*[nbMaxEtats];
    for (unsigned int i = 0; i < nbMaxEtats; i++)
        etats[i] = nullptr;
    etats[0] = new Etat(dep);
}

void Simulateur::build(unsigned int cellule)
{
    if (cellule >= nbMaxEtats)
        throw AutomateException("erreur taille buffer");
    if (etats[cellule] == nullptr)
    {
        etats[cellule] = new Etat;
    }
}

void Simulateur::setEtatDepart(const Etat& e)
{
    depart = &e;
    reset();
}

void Simulateur::reset()
{
    if (depart == nullptr)
        throw AutomateException("etat depart indefini");
    build(0);
    *etats[0] = *depart;
    rang = 0;
}

void Simulateur::next()
{
    if (depart == nullptr)
        throw AutomateException("etat depart indefini");
    rang++;
    build(rang%nbMaxEtats);
    automate.appliquerTransition(*etats[(rang - 1) % nbMaxEtats], *etats[rang%nbMaxEtats]);
}

void Simulateur::run(unsigned int nb_steps)
{
    for (unsigned int i = 0; i < nb_steps; i++)
        next();
}

const Etat& Simulateur::dernier() const
{
    return *etats[rang%nbMaxEtats];
}

Simulateur::~Simulateur()
{
    for (unsigned int i = 0; i < nbMaxEtats; i++)
        delete etats[i];
    delete[] etats;
    delete depart;
}


void Simulateur::saveConfiguration(QJsonObject& obj) const{
    std::string type =typeid(this->automate).name();
    QJsonArray array;
    if (type.find("AutomateUnDimension")!=std::string::npos){
        QString s("AutomateUnDimension");
        if(obj.contains(s)&&obj[s].isArray()){
            array=obj[s].toArray();
            for (unsigned int i=0;i<array.size();++i){
                if (array[i].toObject().value("numeroBit").toString().toStdString()== dynamic_cast<const AutomateUnDimension&>(automate).getNumeroBit()){
                    return;
                }
            }
        }
        QJsonObject item;
        item["numeroBit"]=QString::fromStdString(dynamic_cast<const AutomateUnDimension&>(automate).getNumeroBit());
        array.append(item);
        obj[s]=array;
    }
    else if (type.find("AutomateDeuxDimension")!=std::string::npos){
        QString s("AutomateDeuxDimension");
        if(obj.contains(s)&&obj[s].isArray()){
            array=obj[s].toArray();
            for (unsigned int i=0;i<array.size();++i){
                if ((array[i].toObject().value("numBitCellVivant").toString().toStdString()==dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitVivant())&&
                        (array[i].toObject().value("numBitCellMorte").toString().toStdString()== dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitMorte())){
                    return;
                }
            }
        }
        QJsonObject item;
        item["numBitCellVivant"]=QString::fromStdString(dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitVivant());
        item["numBitCellMorte"]=QString::fromStdString(dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitMorte());
        array.append(item);
        obj[s]=array;
    }
    else if (type.find("AutomatePlusieursEtats")!=std::string::npos){
        QString s("AutomatePlusieursEtats");
        if(obj.contains(s)&&obj[s].isArray()){
            array=obj[s].toArray();
            for (unsigned int i=0;i<array.size();++i){
                if (array[i].toObject().value("numeroBit").toString().toStdString()== dynamic_cast<const AutomatePlusieursEtats&>(automate).getNumeroBit()){
                    return;
                }
            }
        }
        QJsonObject item;
        item["numeroBit"]=QString::fromStdString(dynamic_cast<const AutomatePlusieursEtats&>(automate).getNumeroBit());
        array.append(item);
        obj[s]=array;
    }
}

QJsonObject Simulateur::saveAll() const{
    QJsonObject obj;
    std::string type =typeid(this->automate).name();
    if(type.find("AutomateUnDimension")!=std::string::npos){
        type="AutomateUnDimension";
         obj.insert("numBit",QString::fromStdString(dynamic_cast<const AutomateUnDimension&>(automate).getNumeroBit()));
    }
    else if(type.find("AutomateDeuxDimension")!=std::string::npos){
        type="AutomateDeuxDimension";
        obj.insert("numBitVivant",QString::fromStdString(dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitVivant()));
        obj.insert("numBitMorte",QString::fromStdString(dynamic_cast<const AutomateDeuxDimension&>(automate).getNumeroBitMorte()));
    }
    else if(type.find("AutomatePlusieursEtats")!=std::string::npos){
        type="AutomatePlusieursEtats";
        obj.insert("numBit",QString::fromStdString(dynamic_cast<const AutomatePlusieursEtats&>(automate).getNumeroBit()));
    }
    obj.insert("type",QString::fromStdString(type));
    obj.insert("rang",QString::fromStdString(std::to_string(rang)));
    obj.insert("largeur",QString::fromStdString(std::to_string(depart->getLargeur())));
    obj.insert("hauteur",QString::fromStdString(std::to_string(depart->getHauteur())));
    obj.insert("initialState",depart->saveState());
    return obj;
}
