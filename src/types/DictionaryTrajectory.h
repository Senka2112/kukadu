#ifndef DICTIONARYTRAJECTORY
#define DICTIONARYTRAJECTORY

#include "DMPBase.h"
#include "QueryPoint.h"
#include "Trajectory.h"
#include "../trajectory/JointDMPLearner.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "../utils/conversion_utils.h"
#include <vector>
#include <string>
#include <armadillo>
#include <iostream>
#include <fstream>

class DictionaryTrajectory : public Trajectory {
	
private:

	int degOfFreedom;
	std::string baseFolder;
	
	std::vector<std::string> files;
	std::vector<std::string> queryFiles;
    std::vector<std::string> dmpFiles;
	std::vector<std::string> trajFiles;
	std::vector<QueryPoint> queryPoints;
	std::vector<arma::vec> coefficients;
	
    arma::vec startingPos;

    std::vector<QueryPoint> mapFiles(std::vector<std::string> queryFiles, std::vector<std::string> trajFiles, std::string prefix1, std::string prefix2);
    std::vector<QueryPoint> mapFiles(std::vector<std::string> queryFiles, std::vector<std::string> trajFiles, std::vector<std::string> dmpFiles, std::string prefix1, std::string prefix2, std::string prefix3);
	
public:
	
    DictionaryTrajectory(std::string baseFolder, double az, double bz);
	DictionaryTrajectory(const DictionaryTrajectory& copy);
	DictionaryTrajectory();
	
	int getDegreesOfFreedom() const;
	arma::vec getStartingPos();
	
    double getTmax();
    void setTmax(double tmax);
	
	std::vector<QueryPoint> getQueryPoints();
	
	std::vector<arma::vec> getCoefficients();
	void setCoefficients(std::vector<arma::vec> coeffs);
	
	int operator==(DictionaryTrajectory const& comp) const;
	
    std::shared_ptr<Trajectory> copy();
	
};

#endif
