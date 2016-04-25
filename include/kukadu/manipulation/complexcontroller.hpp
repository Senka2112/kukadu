#ifndef KUKADU_COMPLEXCONTROLLER_H
#define KUKADU_COMPLEXCONTROLLER_H

#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <wordexp.h>
#include <kukadu/manipulation/controller.hpp>
#include <kukadu/types/controllerresult.hpp>
#include <kukadu/manipulation/sensingcontroller.hpp>
#include <kukadu/types/kukadutypes.hpp>
#include <kukadu/robot/sensorstorage.hpp>
#include <kukadu/robot/arm/kukiecontrolqueue.hpp>
#include <kukadu/learning/projective_simulation/core/reward.hpp>
#include <kukadu/learning/projective_simulation/core/projectivesimulator.hpp>
#include <kukadu/learning/projective_simulation/application/manualreward.hpp>
#include <kukadu/manipulation/haptic/intermediateeventclip.hpp>
#include <kukadu/manipulation/haptic/controlleractionclip.hpp>

namespace kukadu {

    class EnvironmentReward : public Reward {

    private:

        double reward;

    protected:

        virtual double computeRewardInternal(KUKADU_SHARED_PTR<PerceptClip> providedPercept, KUKADU_SHARED_PTR<ActionClip> takenAction) {
            // all paths are rewarded, because only observed paths are performed
            return reward;
        }

    public:

        EnvironmentReward(KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, double stdReward) : Reward(generator, false) {
            reward = stdReward;
        }

        virtual int getDimensionality() {
            return 2;
        }

        virtual KUKADU_SHARED_PTR<PerceptClip> generateNextPerceptClip(int immunity) {
            return nullptr;
        }

        virtual KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > generateActionClips() {
            return KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > >(new std::vector<KUKADU_SHARED_PTR<ActionClip> >());
        }

        virtual KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > > generatePerceptClips() {
            return KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > >(new std::vector<KUKADU_SHARED_PTR<PerceptClip> >());
        }

    };

    class ComplexController : public Controller, public Reward, public KUKADU_ENABLE_SHARED_FROM_THIS<ComplexController> {

    private:

        bool cleanup;
        bool storeReward;
        bool colPrevRewards;

        int stdPrepWeight;
        int maxEnvPathLength;
        int currentIterationNum;

        double gamma;
        double boredom;
        double stdReward;
        double punishReward;
        double senseStretch;
        double pathLengthCost;

        KUKADU_SHARED_PTR<EnvironmentReward> envReward;

        std::string psPath;
        std::string storePath;
        std::string historyPath;
        std::string envModelPath;
        std::string rewardHistoryPath;

        KUKADU_SHARED_PTR<PerceptClip> root;
        KUKADU_SHARED_PTR<kukadu_mersenne_twister> gen;
        KUKADU_SHARED_PTR<ProjectiveSimulator> projSim;
        KUKADU_SHARED_PTR<std::ofstream> rewardHistoryStream;
        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<Clip> > > prepActions;
        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > prepActionsCasted;

        KUKADU_DISCRETE_DISTRIBUTION<int> simSuccDist;

        std::vector<double> sensingWeights;
        std::vector<KUKADU_SHARED_PTR<Controller> > preparationControllers;
        std::vector<KUKADU_SHARED_PTR<SensingController> > sensingControllers;

        std::map<std::string, KUKADU_SHARED_PTR<kukadu::SensingController> > availableSensingControllers;
        std::map<std::string, KUKADU_SHARED_PTR<kukadu::Controller> > availablePreparatoryControllers;

        // novelty in icdl paper
        std::map<std::string, KUKADU_SHARED_PTR<kukadu::ProjectiveSimulator> > environmentModels;

        double computeRewardInternal(KUKADU_SHARED_PTR<PerceptClip> providedPercept, KUKADU_SHARED_PTR<ActionClip> takenAction);

        KUKADU_SHARED_PTR<kukadu::ProjectiveSimulator> createEnvironmentModelForSensingAction(KUKADU_SHARED_PTR<kukadu::SensingController> sensingAction, KUKADU_SHARED_PTR<ProjectiveSimulator> projSim);
        std::vector<std::tuple<double, KUKADU_SHARED_PTR<Clip>, std::vector<KUKADU_SHARED_PTR<Clip> > > > computeEnvironmentPaths(KUKADU_SHARED_PTR<Clip> sensingClip, KUKADU_SHARED_PTR<Clip> stateClip, int maxPathLength);
        void computeTotalPathCost(std::vector<std::tuple<double, KUKADU_SHARED_PTR<Clip>, std::vector<KUKADU_SHARED_PTR<Clip> > > >& paths);

        std::pair<double, int> computeEnvironmentTransitionConfidence(KUKADU_SHARED_PTR<Clip> stateClip);

        void printPaths(std::vector<std::tuple<double, KUKADU_SHARED_PTR<Clip>, std::vector<KUKADU_SHARED_PTR<Clip> > > >& paths);

        std::tuple<KUKADU_SHARED_PTR<IntermediateEventClip>, KUKADU_SHARED_PTR<Clip>, KUKADU_SHARED_PTR<ControllerActionClip> > extractClipsFromPath(std::vector<int>& hops);

    protected:

        virtual void setSimulationModeInChain(bool simulationMode);
        virtual double getSimulatedReward(KUKADU_SHARED_PTR<SensingController> usedSensingController, KUKADU_SHARED_PTR<kukadu::PerceptClip> providedPercept, KUKADU_SHARED_PTR<kukadu::Controller> takenAction, int sensingClassIdx, int prepContIdx) = 0;

        virtual double getSimulatedRewardInternal(KUKADU_SHARED_PTR<SensingController> usedSensingController, KUKADU_SHARED_PTR<kukadu::PerceptClip> providedPercept, KUKADU_SHARED_PTR<kukadu::Controller> takenAction, int sensingClassIdx, int prepContIdx);

    public:

        ComplexController(std::string caption, std::string storePath,
                          bool storeReward, double senseStretch, double boredom, KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator,
                          int stdReward, double punishReward, double gamma, int stdPrepWeight, bool collectPrevRewards, int simulationFailingProbability,
                          int maxEnvPathLength = 4, double pathLengthCost = 0.01, double stdEnvironmentReward = 10.0);
        ~ComplexController();

        void store();
        void load(std::string path, std::map<std::string, KUKADU_SHARED_PTR<kukadu::SensingController> > availableSensingControllers, std::map<std::string, KUKADU_SHARED_PTR<kukadu::Controller> > availablePreparatoryControllers);

        virtual void initialize();
        void storeNextIteration();
        void createSensingDatabase();
        void setBoredom(double boredom);
        void store(std::string destination);
        void setTrainingMode(bool doTraining);
        void createSensingDatabase(std::vector<KUKADU_SHARED_PTR<SensingController> > sensingControllers);

        void setSensingControllers(std::vector<KUKADU_SHARED_PTR<kukadu::SensingController> > sensingControllers);
        void setPreparatoryControllers(std::vector<KUKADU_SHARED_PTR<kukadu::Controller> > preparatoryControllers);

        void clearPreparatoryControllers();
        void addPreparatoryController(KUKADU_SHARED_PTR<kukadu::Controller> prepCont);

        virtual void executeComplexAction() = 0;

        int getDimensionality();

        // overwrite this virtual function if the next idx should be created randomly
        virtual int getNextSimulatedGroundTruth(KUKADU_SHARED_PTR<SensingController> sensCont);

        virtual KUKADU_SHARED_PTR<Clip> computeGroundTruthTransition(KUKADU_SHARED_PTR<Clip> sensingClip, KUKADU_SHARED_PTR<Clip> stateClip, KUKADU_SHARED_PTR<Clip> actionClip) = 0;

        double getStdReward();
        double getPunishReward();

        // overwrite this if the ground truth of the sensing classes is known for some sensing actions
        virtual std::string getClassLabel(KUKADU_SHARED_PTR<Clip> sensingClip, KUKADU_SHARED_PTR<Clip> stateClip);

        KUKADU_SHARED_PTR<kukadu_mersenne_twister> getGenerator();

        virtual void cleanupAfterAction() = 0;
        virtual KUKADU_SHARED_PTR<ControllerResult> performAction();
        virtual KUKADU_SHARED_PTR<ControllerResult> performAction(bool cleanup, bool generateNewGroundTruth = true);

        KUKADU_SHARED_PTR<ProjectiveSimulator> getProjectiveSimulator();
        KUKADU_SHARED_PTR<PerceptClip> generateNextPerceptClip(int immunity);
        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > generateActionClips();
        KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > > generatePerceptClips();

        void updateFiles();

#ifdef USEBOOST
        static const std::string FILE_SENSING_PREFIX;
        static const std::string FILE_PREP_PREFIX;
        static const std::string FILE_END_PREFIX;
#else
        static constexpr auto FILE_SENSING_PREFIX = "***sensing controllers:";
        static constexpr auto FILE_PREP_PREFIX = "***preparatory controllers:";
        static constexpr auto FILE_END_PREFIX = "***end";
#endif

    };

}

#endif
