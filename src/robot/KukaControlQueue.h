#ifndef KUKACONTROLQUEUE
#define KUKACONTROLQUEUE

#include <unistd.h>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <thread>
#include <mutex>
#include <time.h>
#include <thread>

// Custom librairies
#include "ControlQueue.h"
#include "../utils/DestroyableObject.h"
#include "../utils/types.h"
#include "robotDriver/src/friMain.h"
#include "robotDriver/src/friMisc.h"
#include "robotDriver/src/kuka/friRemote.h"

#define COMMAND_NOT_SET -100

/** \brief The KukaControlQueue provides control capabilities for the Kuka LWR 4+ robotic arm
 * 
 * This class implements the abstract ControlQueue class for the usage with the Kuka LWR 4+ robotic arm. It provides basic functionalities such as command mode control
 * in joint space as well as point to point movement in cartesian and joint space. To use it, the additionally provided KRL script has to be selected on the robot
 * controller side. For further information how to use it, please see the sample programs and the kuka documentation
 * \ingroup RobotFramework
 */
class KukaControlQueue : public ControlQueue {

private:

	friRemote* friInst;
	std::thread* thr;
	
	int currentQueueSize;
	int port;
	int sleepTime;
	int finish;
	int currentMode;
	int initMode;
	
	float loadMass;
	float loadPos;
	
	float cpstiffnessxyz;
	float cpstiffnessabc;
	float cpdamping;
	float axismaxdeltatrq;
	float cpmaxdelta;
	float maxforce;
	
	double timeThreadStarted;
	double currentTime;
	
	std::queue<float*> movementQueue;
	std::mutex currentJointsMutex;
	std::mutex stiffnessMutex;
	
	bool isInit;
	
	float* startingJoints;
	
	void demoRobot();
	void closeRobotSession();
	void setInitValues();
	void initializeRobot(int);
	
	float* currentJoints;
	float* controlRobot(float*);
	float* setupCommandMode(int);
	float* setupGuidedMeasurementMode(int);

public:

	/** \brief Constructor for KukaControlQueue
	 * \param port port to listen for incoming fri connection
	 * \param sleepTime cycle sleep time (time between to packets sent in command mode)
	 * \param initMode control mode (e.g. command mode, monitor mode) with which the queue should be started
	 */
	KukaControlQueue(int port, int sleepTime, int initMode);
	
	void run();
	void setFinish();
	void addJointsPosToQueue(float* joints);
	void switchMode(int mode);
	void stopCurrentMode();
	void synchronizeToControlQueue(int maxNumJointsInQueue);
	void setStartingJoints(float* joints);
	void moveJoints(float* joints);
	
	void setAdditionalLoad(float loadMass, float loadPos);
	void setStiffness(float cpstiffnessxyz, float cpstiffnessabc, float cpdamping, float cpmaxdelta, float maxforce, float axismaxdeltatrq);
	
	float* getCartesianPos();
	float* getStartingJoints();
	float* retrieveJointsFromRobot();
	
	mes_result getCurrentJoints();
	bool isInitialized();
	void safelyDestroy();
    
};

#endif