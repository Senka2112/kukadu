#ifndef LINCOMPDMP
#define LINCOMPDMP

#include "DictionaryTrajectory.h"
#include "../trajectory/DMPTrajectoryComparator.h"
#include "../learning/metric_learning/Mahalanobis.h"
#include "../learning/metric_learning/TogersonMetricLearner.h"
#include "../utils/types.h"
#include "../utils/utils.h"
#include "../utils/conversion_utils.h"
#include <vector>
#include <armadillo>

class LinCombDmp : public DictionaryTrajectory {
	
private:
	
	Mahalanobis metric;
	arma::vec currentQueryPoint;
	arma::vec trajMetricWeights;
	
public:
	
	LinCombDmp(int queryDegOfFreedom, int degOfFreedom, std::string baseFolder, std::vector<DMPBase> baseDef, double az, double bz, double ax, double tau,
		arma::vec trajMetricWeights
	);
	
	LinCombDmp(int queryDegOfFreedom, int degOfFreedom, std::string baseFolder, std::vector<DMPBase> baseDef, double az, double bz, double ax, double tau,
		arma::mat metricM
	);
	
	LinCombDmp(const LinCombDmp& copy);
	LinCombDmp();
	
	void setCurrentQueryPoint(arma::vec currQuery);
	arma::vec getCurrentQueryPoint();
	
	
	std::vector<arma::vec> getCoefficients();
	void setCoefficients(std::vector<arma::vec> coeffs);
	void initializeMetric();
	
	int getQueryDegreesOfFreedom() const;
	
	Mahalanobis getMetric();
	void setMetric(Mahalanobis metric);
	
	int operator==(LinCombDmp const& comp) const;
	
	Trajectory* copy();
	
};

#endif