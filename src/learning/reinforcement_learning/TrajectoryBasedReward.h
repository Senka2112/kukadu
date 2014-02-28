#ifndef TRAJECTORYBASEDREWARD
#define TRAJECTORYBASEDREWARD

#include <armadillo>
#include <vector>
#include <cmath>

#include "CostComputer.h"
#include "../../utils/types.h"

class TrajectoryBasedReward : public CostComputer {

private:
	
	double tmax;
	double slope;

public:

	TrajectoryBasedReward();
	double computeCost(t_executor_res results);
	
	t_executor_res getOptimalTraj(double tmax);
	arma::vec computeFun(arma::vec t);
	
	virtual double computeFun(double t) = 0;
	
};

#endif