#ifndef GAUSSIANOBSTACLEREWARDCOMPUTER
#define GAUSSIANOBSTACLEREWARDCOMPUTER

#include <armadillo>
#include <vector>
#include <cmath>

#include <RedundantKin.h>

#include "SampleRewardComputer.h"
#include "../../utils/types.h"

class GaussianObstacleRewardComputer : public TrajectoryBasedReward {

private:
	
	double my;
	double sigma;
	double height;
	
public:

	GaussianObstacleRewardComputer(double my, double sigma, double height);
	double computeFun(double t);
	
};

// TODO: move this to separate file
class GraspingRewardComputer : public CostComputer{

private:
	
	arma::vec pos;
	
	double robotPos[4], fingerDir[4], palmNormal[4];
	double worldPos[4], worldFingerDir[4], worldPalmNormal[4];
	
public:
	
	GraspingRewardComputer(std::vector<double> finalJointPos);

	double computeCost(t_executor_res results);
	
};

#endif