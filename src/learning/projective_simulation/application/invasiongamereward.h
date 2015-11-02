#ifndef KUKADU_INVASIONGAMEREWARD_H
#define KUKADU_INVASIONGAMEREWARD_H

#include "../core/reward.h"
#include "../../../types/KukaduTypes.h"

#define INVASIONGAMEREWARD_H_LEFT_PERCEPT 0
#define INVASIONGAMEREWARD_H_RIGHT_PERCEPT 1

#define INVASIONGAMEREWARD_H_LEFT_ACTION 0
#define INVASIONGAMEREWARD_H_RIGHT_ACTION 1

#define INVASIONGAMEREWARD_REW 1.0

class InvasionGameReward : public Reward {

private:

    int currentTimeStep;

    std::uniform_int_distribution<int> intDist;

    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > > perceptClips;

protected:

    double computeRewardInternal(KUKADU_SHARED_PTR<PerceptClip> providedPercept, KUKADU_SHARED_PTR<ActionClip> takenAction);

public:

    InvasionGameReward(KUKADU_SHARED_PTR<kukadu_mersenne_twister> generator, bool collectPrevRewards);

    int getDimensionality();

    KUKADU_SHARED_PTR<PerceptClip> generateNextPerceptClip(int immunity);
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<ActionClip> > > generateActionClips();
    KUKADU_SHARED_PTR<std::vector<KUKADU_SHARED_PTR<PerceptClip> > > generatePerceptClips();

};

#endif // INVASIONGAMEREWARD_H
