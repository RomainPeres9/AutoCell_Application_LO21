#include "AutomateManager.hpp"

AutomateManager::AutomateManager()
{
    for (unsigned int i = 0; i < 256; i++)
        automatesUnDimension[i] = nullptr;
    for (unsigned int i = 0; i < 512*512; i++)
        automatesDeuxDimension[i] = nullptr;
    for (unsigned int i = 0; i < 256; i++)
        automatesPlusieursEtats[i] = nullptr;
}

AutomateManager::~AutomateManager()
{
    for (unsigned int i = 0; i < 256; i++)
        delete automatesUnDimension[i];
    for (unsigned int i = 0; i < 512*512; i++)
        delete automatesDeuxDimension[i];
    for (unsigned int i = 0; i < 256; i++)
        delete automatesPlusieursEtats[i];
    
}

AutomateManager::Handler AutomateManager::handler = Handler();

AutomateManager& AutomateManager::getAutomateManager()
{
    if (!handler.instance)
        handler.instance = new AutomateManager;
    return *handler.instance;
}

void AutomateManager::libererAutomateManager()
{
    delete handler.instance;
    handler.instance = nullptr;
}

const AutomateUnDimension& AutomateManager::getAutomateUnDimension(unsigned short int num)
{
    if (!automatesUnDimension[num])
        automatesUnDimension[num] = new AutomateUnDimension(num);
    return *automatesUnDimension[num];
}

const AutomateUnDimension& AutomateManager::getAutomateUnDimension(const std::string& numBit)
{
    return getAutomateUnDimension(AutomateUnDimension::NumBitToNum(numBit));
}

const AutomateDeuxDimension& AutomateManager::getAutomateDeuxDimension(unsigned short int nV, unsigned short int nM)
{
    if (!automatesDeuxDimension[nV, nM])
        automatesDeuxDimension[nV, nM] = new AutomateDeuxDimension(nV, nM);
    return *automatesDeuxDimension[nV, nM];
}
const AutomateDeuxDimension& AutomateManager::getAutomateDeuxDimension(const std::string& BitVivant, const std::string& BitMorte)
{
    return getAutomateDeuxDimension(AutomateDeuxDimension::NumBitToNum(BitVivant), AutomateDeuxDimension::NumBitToNum(BitMorte));
}

const AutomatePlusieursEtats& AutomateManager::getAutomatePlusieursEtats(unsigned short int num)
{
    if (!automatesPlusieursEtats[num])
        automatesPlusieursEtats[num] = new AutomatePlusieursEtats(num);
    return *automatesPlusieursEtats[num];
}

const AutomatePlusieursEtats& AutomateManager::getAutomatePlusieursEtats(const std::string& numBit)
{
    return getAutomatePlusieursEtats(AutomatePlusieursEtats::NumBitToNumPE(numBit));
}


