#ifndef MALANOBIS
#define MALANOBIS

#include <math.h>
#include <armadillo>

class Mahalanobis {

private:
	
	arma::mat M;
	
public:
	
	Mahalanobis(int dim);
	Mahalanobis(arma::mat M);
	Mahalanobis(const Mahalanobis& maha);
	
	double computeSquaredDistance(arma::vec vec1, arma::vec vec2);
	
	arma::vec getCoefficients();
	
	void setM(arma::mat M);
	arma::mat getM() const;
	arma::mat getDecomposition();
	
};

#endif