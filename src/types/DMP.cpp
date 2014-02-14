#include "DMP.h"

using namespace std;
using namespace arma;

Dmp::Dmp(arma::vec supervisedTs, std::vector<arma::vec> sampleYs, std::vector<arma::vec> fitYs, std::vector<arma::vec> dmpCoeffs, std::vector<DMPBase> dmpBase, std::vector<arma::mat> designMatrices,
		double tau, double az, double bz, double ax, double ac, double dmpStepSize, double tolAbsErr, double tolRelErr)
				: SingleSampleTrajectory(supervisedTs, sampleYs) {
				
	construct(supervisedTs, sampleYs, fitYs, dmpCoeffs, dmpBase, designMatrices, tau, az, bz, ax, ac, dmpStepSize, tolAbsErr, tolRelErr);
	
}

Dmp::Dmp(arma::vec supervisedTs, std::vector<arma::vec> sampleYs, std::vector<arma::vec> fitYs, std::vector<arma::vec> dmpCoeffs, std::vector<DMPBase> dmpBase, std::vector<arma::mat> designMatrices,
		double tau, double az, double bz, double ax) : SingleSampleTrajectory(supervisedTs, sampleYs) {
	
	construct(supervisedTs, sampleYs, fitYs, dmpCoeffs, dmpBase, designMatrices, tau, az, bz, ax, 10.0, 1.8 * 1e4 * 1e-6, 0.1, 0.1);
	
}

void Dmp::construct(arma::vec supervisedTs, std::vector<arma::vec> sampleYs, std::vector<arma::vec> fitYs, std::vector<arma::vec> dmpCoeffs, std::vector<DMPBase> dmpBase, std::vector<arma::mat> designMatrices,
		double tau, double az, double bz, double ax, double ac, double dmpStepSize, double tolAbsErr, double tolRelErr) {
	
	this->dmpCoeffs = dmpCoeffs;
	this->dmpBase = dmpBase;
	this->designMatrices = designMatrices;
	this->tau = tau;
	this->az = az;
	this->bz = bz;
	this->ax = ax;
	this->fitYs = fitYs;
	
	this->ac = ac;
	this->dmpStepSize = dmpStepSize;
	this->tolAbsErr = tolAbsErr;
	this->tolRelErr = tolRelErr;
	
	initializeY0();
	initializeDy0();
	initializeDdy0();
	initializeG();
	
}

void Dmp::setDmpCoeffs(std::vector<arma::vec> coeffs) {
	dmpCoeffs = coeffs;
}

Dmp::Dmp(const Dmp& copy) : SingleSampleTrajectory(copy) {
	
	this->dmpCoeffs = copy.dmpCoeffs;
	this->dmpBase = copy.dmpBase;
	this->designMatrices = copy.designMatrices;
	this->tau = copy.tau;
	this->az = copy.az;
	this->bz = copy.bz;
	this->ax = copy.ax;
	this->dmpStepSize = copy.dmpStepSize;
	this->ac = copy.ac;
	this->tolAbsErr = copy.tolAbsErr;
	this->tolRelErr = copy.tolRelErr;
	
	initializeY0();
	initializeDy0();
	initializeDdy0();
	initializeG();
	
}

Dmp::Dmp() : SingleSampleTrajectory(vec(), vector<vec>()) {
//	cout << "(Dmp) warning: dummy constructor has been called" << endl;
}

std::vector<arma::vec> Dmp::getFitYs() {
	return fitYs;
}

arma::vec Dmp::getY0() {
	return y0;
}

arma::vec Dmp::getDy0() {
	return dy0;
}

arma::vec Dmp::getDdy0() {
	return ddy0;
}

arma::vec Dmp::getG() {
	return g;
}

arma::mat Dmp::getDesignMatrix(int freedomIdx) {
	return designMatrices.at(freedomIdx);
}

std::vector<arma::vec> Dmp::getDmpCoeffs() {
	return dmpCoeffs;
}

std::vector<arma::vec> Dmp::getCoefficients() {
	return getDmpCoeffs();
}

double Dmp::getY0(int freedomIdx) {
	return y0(freedomIdx);
}

double Dmp::getDy0(int freedomIdx) {
	return dy0(freedomIdx);
}

double Dmp::getDdy0(int freedomIdx) {
	return ddy0(freedomIdx);
}

double Dmp::getG(int freedomIdx) {
	return g(freedomIdx);
}

arma::vec Dmp::getDmpCoeffs(int freedomIdx) {
	return dmpCoeffs.at(freedomIdx);
}

void Dmp::setCoefficients(std::vector<arma::vec> coeffs) {
	setDmpCoeffs(coeffs);
}

std::vector<DMPBase> Dmp::getDmpBase() {
	return dmpBase;
}

double Dmp::getTau() {
	return tau;
}

double Dmp::getAz() {
	return az;
}

double Dmp::getBz() {
	return bz;
}

double Dmp::getAx() {
	return ax;
}

double Dmp::getStepSize() {
	return dmpStepSize;
}

double Dmp::getTolAbsErr() {
	return tolAbsErr;
}

double Dmp::getTolRelErr() {
	return tolRelErr;
}

double Dmp::getTmax() {
	
	int s = getDataPointsNum();
	if(s > 0)
		return getT(s - 1);
	else
		return 0.0;
	
}

void Dmp::initializeY0() {
	
	int degOfFreemdom = getDegreesOfFreedom();
	y0 = vec(degOfFreemdom);
	for(int i = 0; i < degOfFreemdom; ++i)
		y0(i) = getDataPoint(i, 0);
	
}

void Dmp::initializeDy0() {
	
	int degOfFreemdom = getDegreesOfFreedom();
	dy0 = vec(degOfFreemdom);
	for(int i = 0; i < degOfFreemdom; ++i)
		dy0(i) = ( getDataPoint(i, 1) - getDataPoint(i, 0) ) / ( getT(1) - getT(0) );
	
}

void Dmp::initializeDdy0() {
	
	int degOfFreemdom = getDegreesOfFreedom();
	ddy0 = vec(degOfFreemdom);
	for(int i = 0; i < degOfFreemdom; ++i) {
		double dy = ( getDataPoint(i, 2) - getDataPoint(i, 1) ) / ( getT(2) - getT(1) );
		ddy0(i) = ( dy - dy0(i) ) / ( getT(2) - getT(0) );
	}
	
}

void Dmp::initializeG() {
	
	int s = getDataPointsNum();
	int degOfFreemdom = getDegreesOfFreedom();
	g = vec(degOfFreemdom);
	for(int i = 0; i < degOfFreemdom; ++i)
		g(i) = getDataPoint(i, s - 1);
	
}

/*
std::vector<arma::vec> fitYs;
	std::vector<arma::vec> dmpCoeffs;
	arma::vec y0;
	arma::vec dy0;
	arma::vec ddy0;
	arma::vec g;
	
	std::vector<DMPBase> dmpBase;
	std::vector<arma::mat> designMatrices;
	
	double tau;
	double az;
	double bz;
	double ax;
*/
int Dmp::operator==(Dmp const& comp) const {
	
	Dmp tComp(comp);
	Dmp tThis(*this);
	
//	cout << "(dmp comp)" << (tComp == tThis)<< " " << (compareVectorOfArmadillos(dmpCoeffs, comp.dmpCoeffs)) << " " << (compareArmadilloVec(y0, comp.y0)) << " " << (compareArmadilloVec(dy0, comp.dy0)) << " " <<
//				(compareArmadilloVec(ddy0, comp.ddy0)) << " " << (compareArmadilloVec(g, comp.g)) << " " << (dmpBase == comp.dmpBase) << " " <<
//				(tau == comp.tau) << " " << (az == comp.az) << " " << (bz == comp.bz) << " " << (ax == comp.ax) << endl;
	
	// design matrices are ignored here
	return (compareVectorOfArmadillos(dmpCoeffs, comp.dmpCoeffs) && compareArmadilloVec(y0, comp.y0) && compareArmadilloVec(dy0, comp.dy0) &&
				compareArmadilloVec(ddy0, comp.ddy0) && compareArmadilloVec(g, comp.g) && dmpBase == comp.dmpBase &&
				tau == comp.tau && az == comp.az && bz == comp.bz && ax == comp.ax );
	
}

Trajectory* Dmp::copy() {
	return new Dmp(*this);
}

