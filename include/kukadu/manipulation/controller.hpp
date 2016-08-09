#ifndef KUKADU_CONTROLLER_H
#define KUKADU_CONTROLLER_H

#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <wordexp.h>
#include <kukadu/types/controllerresult.hpp>
#include <kukadu/types/kukadutypes.hpp>

namespace kukadu {

    class Controller {

    private:

        bool simulation;

        double simulationFailingProbability;

        std::string caption;

    protected:

        bool isShutUp;

        // is called by set simulation mode
        virtual void setSimulationModeInChain(bool simulationMode);

    public:

        Controller(std::string caption, double simulationFailingProbability);

        void shutUp();
        void startTalking();
        virtual void setSimulationMode(bool simulationMode);

        virtual bool requiresGrasp() = 0;
        virtual bool producesGrasp() = 0;

        virtual void initialize();

        // returns true, if the controller should be simulated
        // returns false otherwise
        virtual bool getSimulationMode();

        double getSimFailingProb();

        std::string getCaption();

        virtual KUKADU_SHARED_PTR<ControllerResult> performAction() = 0;

    };

}

#endif
