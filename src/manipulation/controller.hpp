#ifndef KUKADU_CONTROLLER_H
#define KUKADU_CONTROLLER_H

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <wordexp.h>

#include "controllerresult.hpp"
#include "../types/kukadutypes.hpp"

class Controller {

private:

    bool simulation;
    std::string caption;

protected:

    bool isShutUp;

    // is called by set simulation mode
    virtual void setSimulationModeInChain(bool simulationMode) {}

public:

    Controller(std::string caption);

    void shutUp();
    void startTalking();
    void setSimulationMode(bool simulationMode);

    bool getSimulationMode();

    std::string getCaption();

    virtual KUKADU_SHARED_PTR<ControllerResult> performAction() = 0;

};

#endif
