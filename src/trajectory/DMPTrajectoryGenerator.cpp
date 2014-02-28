#include "DMPTrajectoryGenerator.h"

using namespace std;
using namespace arma;

DMPTrajectoryGenerator::DMPTrajectoryGenerator(std::vector<DMPBase> baseDef, double ax, double tau) {

	this->baseDef = baseDef;

	this->ax = ax;
	this->tau = tau;
	
}

double DMPTrajectoryGenerator::evaluateByCoefficientsSingle(double x, vec coeff) {
	int coeffDegree = this->getBasisFunctionCount();
	double val = 0.0;
	for(int i = 0; i < coeffDegree; ++i) {
		val += coeff(i) * evaluateBasisFunction(x, i);
	}
	return val;
}

double DMPTrajectoryGenerator::evaluateByCoefficientsSingleNonExponential(double x, vec coeff) {
	int coeffDegree = this->getBasisFunctionCount();
	double val = 0.0;
	for(int i = 0; i < coeffDegree; ++i) {
		val += coeff(i) * evaluateBasisFunctionNonExponential(x, i);
//		cout << evaluateBasisFunctionNonExponential(x, i) << " ";
	}
//	cout << endl;
//	getchar();
	if(abs(val) < 1e-308) {
	//	cout << "x: " << x << endl;
	//	double val = 0.0;
	//	for(int i = 0; i < coeffDegree; ++i) {
	//		val += coeff(i) * evaluateBasisFunctionNonExponential(x, i);
	//		cout << coeff(i) << " " << " " << evaluateBasisFunctionNonExponential(x, i) << " " << coeff(i) * evaluateBasisFunctionNonExponential(x, i) << endl;
	//	}
	}
	return val;
}


vec DMPTrajectoryGenerator::evaluateByCoefficientsMultiple(vec x, int sampleCount, vec coeff) {
	int coeffDegree = this->getBasisFunctionCount();
	vec values(sampleCount);
	for(int i = 0; i < sampleCount; ++i) {
		values(i) = evaluateByCoefficientsSingle(x(i), coeff);
	}
	return values;
}

vec DMPTrajectoryGenerator::evaluateByCoefficientsMultipleNonExponential(vec x, int sampleCount, vec coeff) {
	int coeffDegree = this->getBasisFunctionCount();
	vec values(sampleCount);
	for(int i = 0; i < sampleCount; ++i) {
		values(i) = evaluateByCoefficientsSingleNonExponential(x(i), coeff);
	}
	return values;
}

double DMPTrajectoryGenerator::computeNormalization(double x) {
	
	int myssize = baseDef.size();
	
	double normVal = 0.0;
	
	for(int i = 0; i < myssize; ++i) {
		double my = baseDef.at(i).getMy();
		vector<double> currentSigmas = baseDef.at(i).getSigmas();
		for(int j = 0; j < currentSigmas.size(); ++j) {
			double sigma = currentSigmas.at(j);
			normVal += exp(  -pow(x - my, 2) / (2 * pow(sigma, 2))   );
		}
	}
	return normVal;
	
}

// with this implementation, currently all sigmas have to be of the same size (see DMPTrajectoryGenerator::evaluateBasisFunction and DMPTrajectoryGenerator::getBasisFunctionCount)
double DMPTrajectoryGenerator::evaluateBasisFunction(double x, int fun) {
	
	int myssize = baseDef.size();
	int sigmassize = baseDef.at(0).getSigmas().size();
	
	int mypos = fun / sigmassize;
	int sigmapos = fun % sigmassize;
	
	double my = baseDef.at(mypos).getMy();
	double sigma = baseDef.at(mypos).getSigmas().at(sigmapos);
	
	double base = exp(  - pow(  exp( -ax / tau * x ) - my, 2) / (2 * pow(sigma, 2))   ) * exp( -ax / tau * x );
	double normVal = computeNormalization(exp( -ax / tau * x ));

	return base / normVal;

}

double DMPTrajectoryGenerator::evaluateBasisFunctionNonExponential(double x, int fun) {

	int myssize = baseDef.size();
	int sigmassize = baseDef.at(0).getSigmas().size();
	
	int mypos = fun / sigmassize;
	int sigmapos = fun % sigmassize;
	
	double my = baseDef.at(mypos).getMy();
	double sigma = baseDef.at(mypos).getSigmas().at(sigmapos);
//	cout << "x: " << x << " my: " << my << " sigma: " << sigma << endl;
//	cout << "x - my: " << (x - my) << endl;
	double base = exp( -pow( x - my, 2 ) / (2 * pow(sigma, 2)) ) * x;
	double normVal = computeNormalization(x);

	return base / normVal;

}

// with this implementation, currently all sigmas have to be of the same size (see DMPTrajectoryGenerator::evaluateBasisFunction and DMPTrajectoryGenerator::getBasisFunctionCount)
int DMPTrajectoryGenerator::getBasisFunctionCount() {
	int myssize = baseDef.size();
	int sigmassize = baseDef.at(0).getSigmas().size();
	return myssize * sigmassize;
}

string DMPTrajectoryGenerator::getTrajectoryType() {
	return "dmp";
}