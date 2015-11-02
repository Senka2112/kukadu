#ifndef KUKADU_COMPLEXCONTROLLER_H
#define KUKADU_COMPLEXCONTROLLER_H

#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <wordexp.h>

#include "Controller.hpp"
#include "ControllerResult.hpp"
#include "SensingController.hpp"
#include "../types/KukaduTypes.h"
#include "../robot/SensorStorage.h"
#include "../robot/KukieControlQueue.h"
#include "../learning/projective_simulation/core/reward.h"
#include "../learning/projective_simulation/core/projectivesimulator.h"
#include "../learning/projective_simulation/application/manualreward.h"

class ComplexController : public Controller, public Reward, public KUKADU_ENABLE_SHARED_FROM_THIS<ComplexController> {

private:

    bool storeReward;
    bool colPrevRewards;

    int stdPrepWeight;
    int currentIterationNum;

    double gamma;
    double boredom;
    double stdReward;
    double punishReward;
    double senseStretch;

    std::string corrPSPath;
    std::string rewardHistoryPath;

    KUKADU_SHARED_PTR<PerceptClip> root;
    KUKADU_SHARED_PTR<kukadu_mersenne_twister> gen;
    KUKADU_SHARED_PTR<ProjectiveSimulator> projSim;
    KUKADU_SHARED_PTR<std::ofstream> rewardHistoryStream;
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > prepActions;
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > prepActionsCasted;

    std::vector<double> sensingWeights;
    std::vector<KUKADU_SHARED_PTR<Controller> > preparationControllers;
    std::vector<KUKADU_SHARED_PTR<SensingController> > sensingControllers;

    double computeRewardInternal(KUKADU_SHARED_PTR<PerceptClip> providedPercept, KUKADU_SHARED_PTR<ActionClip> takenAction);

protected:

    void setSimulationModeInChain(bool simulationMode);
    virtual double getSimulatedReward(KUKADU_SHARED_PTR<SensingController> usedSensingController, KUKADU_SHARED_PTR<PerceptClip> providedPercept, KUKADU_SHARED_PTR<ActionClip> takenAction, int sensingClassIdx, int prepContIdx) = 0;

public:

    ComplexController(std::string caption, std::vector<KUKADU_SHARED_PTR<SensingController> > sensingControllers, std::vector<KUKADU_SHARED_PTR<Controller> > preparationControllers,
                      std::string corrPSPath, std::string rewardHistoryPath, bool storeReward, double senseStretch, double boredom, KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, int stdReward, double punishReward, double gamma, int stdPrepWeight, bool collectPrevRewards);
    ~ComplexController();

    void store();
    void initialize();
    void storeNextIteration();
    void createSensingDatabase();
    void setBoredom(double boredom);
    void store(std::string destination);
    void setTrainingMode(bool doTraining);
    void createSensingDatabase(std::vector<KUKADU_SHARED_PTR<SensingController> > sensingControllers);

    virtual void executeComplexAction() = 0;

    int getDimensionality();

    // overwrite this virtual function if the next idx should be created randomly
    virtual int getNextSimulatedGroundTruth(KUKADU_SHARED_PTR<SensingController> sensCont);

    double getStdReward();
    double getPunishReward();

    KUKADU_SHARED_PTR<ControllerResult> performAction();
    KUKADU_SHARED_PTR<ProjectiveSimulator> getProjectiveSimulator();
    KUKADU_SHARED_PTR<PerceptClip> generateNextPerceptClip(int immunity);
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > generateActionClips();
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > > generatePerceptClips();

};



#endif
