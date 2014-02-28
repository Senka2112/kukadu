#ifndef OROCOSCONTROLQUEUE
#define OROCOSCONTROLQUEUE

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
#include "robotDriver/src/kuka/friRemote.h"

#include "ros/ros.h"
#include "std_msgs/String.h"

#include <motion_control_msgs/JointPositions.h>
#include <sensor_msgs/JointState.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/Float64MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/MultiArrayDimension.h>
#include <geometry_msgs/Pose.h>
#include <lwr_fri/typekit/Types.hpp>

#define COMMAND_NOT_SET -100

/** \brief The OrocosControlQueue provides control capabilities for the Kuka LWR 4+ robotic arm
 * 
 * This class implements the abstract ControlQueue class for the usage with the iisorocos system. It provides basic functionalities such as command mode control
 * in joint space as well as point to point movement in cartesian and joint space. To use it, the additionally provided KRL script has to be selected on the robot
 * controller side. For further information how to use it, please see the sample programs and the kuka documentation
 * \ingroup RobotFramework
 */
class OrocosControlQueue : public ControlQueue {

private:
	
	int sleepTime;
	int finish;
	int isInit;
	int argc;
	
	int ptpReached;
	int monComMode;
	int impMode;
	int currentMode;
	
	char** argv;
	
	float* startingJoints;
	float* currentJoints;
	float* currentCarts;
	
	double currentTime;
	
	std::queue<float*> movementQueue;
	std::mutex currentJointsMutex;
	std::mutex currentCartsMutex;
	
	std::string commandTopic;
	std::string retJointPosTopic;
	std::string retCartPosTopic;
	std::string switchModeTopic;
	std::string stiffnessTopic;
	std::string jntStiffnessTopic;
	std::string ptpTopic;
	std::string commandStateTopic;
	std::string ptpReachedTopic;
	std::string addLoadTopic;
	
	ros::NodeHandle node;
	ros::Rate* loop_rate;
	
	ros::Publisher pubCommand;
	ros::Publisher pubSwitchMode;
	ros::Publisher pubPtp;
	ros::Publisher pubAddLoad;
	
	ros::Publisher pub_set_cart_stiffness;
	ros::Publisher pub_set_joint_stiffness;
	
	ros::Subscriber subJntPos;
	ros::Subscriber subCartPos;
	ros::Subscriber subComState;
	ros::Subscriber subPtpReached;
	
	double computeDistance(float* a1, float* a2, int size);

public:

	/** \brief Constructor for KukaControlQueue
	 * \param port port to listen for incoming fri connection
	 * \param sleepTime cycle sleep time (time between to packets sent in command mode)
	 * \param initMode control mode (e.g. command mode, monitor mode) with which the queue should be started
	 */
	OrocosControlQueue(int argc, char** argv, int sleepTime, std::string commandTopic, std::string retPosTopic, std::string switchModeTopic, std::string retCartPosTopic,
		std::string cartStiffnessTopic, std::string jntStiffnessTopic, std::string ptpTopic,
		std::string commandStateTopic, std::string ptpReachedTopic, std::string addLoadTopic, ros::NodeHandle node
	);
	
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
	void setInitValues();
	
	/* orocos callback functions */
	void robotJointPosCallback(const sensor_msgs::JointState& msg);
	void robotCartPosCallback(const geometry_msgs::Pose& msg);
	void commandStateCallback(const std_msgs::Float32MultiArray& msg);
	void phpReachedCallback(const std_msgs::Int32MultiArray& msg);
    
};

#endif