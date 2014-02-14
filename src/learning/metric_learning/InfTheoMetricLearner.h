#ifndef INFTHEOMETRICLEARNING
#define INFTHEOMETRICLEARNING

#include "Mahalanobis.h"
#include "MahalanobisLearner.h"
#include "InfTheoConstraints.h"

#include <math.h>
#include <algorithm>
#include <armadillo>

// this class implements the method from paper "Information-Theoretic Metric Learning" from davis, kulis, jain, sra and dhillon
class InfTheoMetricLearner : public MahalanobisLearner {

private:
	
	int checkForConCount;
	
	double divTol;
	double simBorder;
	double disSimBorder;
	double gamma;
	
	std::vector<arma::mat> lastMetrics;
	std::vector<arma::mat> oldMetrics;
	
	arma::mat M0;
	
	InfTheoConstraints simConstraints;
	InfTheoConstraints disSimConstraints;
	
	void computeConstraints();
	int checkConvergence(arma::mat currentM);
	
public:
	
	InfTheoMetricLearner(std::vector<arma::vec> x1s, std::vector<arma::vec> x2s, std::vector<double> distances, double simBorder, double disSimBorder, double gamma, double divTol, int checkForConCount);
	
	Mahalanobis learnMetric();
	
};

#endif