#ifndef DMPUTILS
#define DMPUTILS

#include <stdio.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <vector>
#include <cstring>
#include <armadillo>
#include <wordexp.h>
#include <signal.h>
#include <utility>
#include <limits>
#include <eigen3/Eigen/Dense>


#include <dirent.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_linalg.h>

#include <tf/transform_datatypes.h>
#include <geometry_msgs/Quaternion.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include "../trajectory/DMPTrajectoryGenerator.h"
#include "../learning/GaussianProcessRegressor.h"
#include "../learning/TricubeKernel.h"
#include "../learning/GaussianKernel.h"
#include "../robot/ControlQueue.h"
#include "../types/DMPBase.h"
#include "types.h"
#include "KukaduTokenizer.h"
#include "../manipulation/ControllerResult.hpp"

#include "gnuplot-cpp/gnuplot_i.hpp"



int createDirectory(std::string path);
void deleteDirectory(std::string path);

void printDoubleVector(std::vector<double>* data);
void printDoubleVector(double* data, int size);
void printDoubleMatrix(double** data, int rows, int columns);
void freeDoubleArray(double** data, int columns);
std::string resolvePath(std::string path);

int getch();

arma::mat readMovements(std::string file);
arma::mat readMovements(std::ifstream& stream);
std::pair<std::vector<std::string>, arma::mat> readSensorStorage(std::string file);

arma::vec readQuery(std::string file);
std::vector<double>* createStdVectorFromGslVector(gsl_vector* vec);

arma::vec computeDiscreteDerivatives(arma::vec x, arma::vec y);
gsl_vector* createGslVectorFromStdVector(std::vector<double>* data);
gsl_matrix* createMatrixFromQueueArray(std::queue<double>** data, int columns);
gsl_matrix* invertSquareMatrix(gsl_matrix* mat);

arma::vec createArmaVecFromDoubleArray(double* data, int n);

double* createDoubleArrayFromStdVector(std::vector<double>* data);
double* createDoubleArrayFromArmaVector(arma::vec data);
float* createFloatArrayFromStdVector(std::vector<float>* data);
double* createDoubleArrayFromVector(gsl_vector* data);
double* polyder(double* c, int len);
double* poly_eval_multiple(double* data, int data_len, double* c, int c_len);
float* copyJoints(const float* arr, int arrSize);

double** createDoubleArrayFromMatrix(gsl_matrix* data);

arma::vec stdToArmadilloVec(std::vector<double> stdVec);
arma::mat fillTrajectoryMatrix(arma::mat joints, double tMax);

std::string stringFromDouble(double d);

std::vector<double> armadilloToStdVec(arma::vec armadilloVec);
std::vector<double> computeDMPMys(std::vector<double> mys, double ax, double tau);

// taken from http://rosettacode.org/wiki/Polynomial_regression
double* polynomialfit(int obs, int degree, double *dx, double *dy);

std::vector<std::string> getFilesInDirectory(std::string folderPath);
std::vector<std::string> sortPrefix(std::vector<std::string> list, std::string prefix);

std::string buildPolynomialEquation(double* w, int paramCount);
std::vector<double>* getDoubleVectorFromArray(double* arr, int size);
std::vector<DMPBase> buildDMPBase(std::vector<double> tmpmys, std::vector<double> tmpsigmas, double ax, double tau);

arma::mat gslToArmadilloMatrix(gsl_matrix* matrix);
std::vector<double>* testGaussianRegressor();
std::vector<double> constructDmpMys(arma::mat joints);

arma::vec squareMatrixToColumn(arma::mat m);
arma::mat columnToSquareMatrix(arma::vec c);

arma::vec symmetricMatrixToColumn(arma::mat m);
arma::mat columnToSymmetricMatrix(arma::vec c);

void set_ctrlc_exit_handler();
void exit_handler(int s);

arma::mat armaJoinRows(arma::vec v1, arma::mat m2);
arma::mat armaJoinRows(arma::mat m1, arma::mat m2);

double absolute(double val);

geometry_msgs::Pose vectorarma2pose(arma::vec* vectorpose);
arma::vec pose2vectorarma(geometry_msgs::Pose posepose);

arma::vec log(const tf::Quaternion quat);
tf::Quaternion exp(arma::vec logQuat);
double distQuat(tf::Quaternion q1, tf::Quaternion q2);

tf::Transform Matrix4f2Transform(Eigen::Matrix4f Tm);
tf::Quaternion axisAngle2Quat (const double xx, const double &yy, const double &zz, const double &a);

double distancePoint2Line(double xp,double yp,double x1,double y1,double x2,double y2);
arma::vec pointOnLine2Point(double xp,double yp,double x1,double y1,double x2,double y2);

bool isDirectory(const std::string dirPath);
bool fileExists(const std::string filePath);
void copyFile(const std::string source, const std::string destination);

std::vector<double> createJointsVector(double j1, double j2, double j3, double j4, double j5, double j6, double j7);

pcl::PointCloud<pcl::PointXYZ> sensorMsgsPcToPclPc(sensor_msgs::PointCloud2 pc);
sensor_msgs::PointCloud2 pclPcToSensorMsgsPc(pcl::PointCloud<pcl::PointXYZ> pc);

#endif
