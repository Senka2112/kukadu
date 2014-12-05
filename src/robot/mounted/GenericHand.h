#ifndef GENERICHAND
#define GENERICHAND

#include <vector>
#include <armadillo>
#include "../../utils/DestroyableObject.h"

/**
 * \brief The GenericHand provides a very elementary interface to control robot hands mounted on a robot arm
 * This class provides very an interface for the very basic functionalities such as "connect to hand" or "close hand"
 * \ingroup RobotFramework
 */
class GenericHand : public DestroyableObject {

private:


public:

	/** \brief Initializes the connection to the hand
	 * 
	 */
	virtual void connectHand() = 0;
	
	/** \brief Opens and closes the hand according to the provided closing percentage and velocity
	 * \param percentage closing percentage (0.0 - hand fully open, 1.0 hand fully closed)
	 * \param velocity closing velocity in range between 0 and 1
	 */
	virtual void closeHand(double percentage, double velocity) = 0;
	
	/** \brief Closes connection between host computer and hand
	 * 
	 */
	virtual void disconnectHand() = 0;

    virtual std::vector<arma::mat> getTactileSensing() = 0;

    virtual std::string getHandName() = 0;

};

#endif
