#ifndef QUADRATICKERNEL
#define QUADRATICKERNEL

#include "GenericKernel.h"
#include <armadillo>
#include <math.h>
#include <iostream>

/** \brief Implements a quadratic kernel according to the GenericKernel specifiction.
 * 
 * This class implements a quadratic kernel given by the function K(u) = 15/16 (1 - |u|^2)^2
 * \ingroup LearningFramework
 */
class QuadraticKernel : public GenericKernel {
	
private:

public:

	/**
	 * \brief constructor
	 */
	QuadraticKernel();
	
	double evaluateKernel(arma::vec q1, arma::vec q2, void* kernelParam);
    
};

#endif