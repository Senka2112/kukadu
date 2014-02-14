#include <stdio.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <queue>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_poly.h>
#include <signal.h>
#include "utils/gnuplot-cpp/gnuplot_i.hpp"

#include "../include/kukadu.h"

using namespace std;
using namespace SDH;

char consoleInputter();
void got_signal(int);

char consoleInput = 0;
vector<DestroyableObject*>* destructList = NULL;
int main(int argc, char** args) {
	
	int columns = 8;
	
	// better dmp parameters
	double az = 15.0;
	double bz = (az - 1.0) / 4.0;
	double ac = 5.0;
	
	char* pickupGen = "./movements/thesis_sample/pickup-gen-1d/";
	char* drillFile = "./movements/thesis_sample/catching.txt";
	char* screwFile = "./movements/thesis_sample/screw.txt";
	char* handPort = "/dev/ttyUSB1";
	
	vector<double> pickupTmpmys{0, 1, 2, 3, 4, 5, 6, 7};
	vector<double> catchTmpmys{0, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 7, 8, 9, 10, 11, 11.5, 12, 12.5, 13, 14, 15, 16, 17};
	vector<double> screwTmpmys{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
	
	vector<double> tmpsigmas{0.2, 0.8};
	
	double handVelocity = 20.0;
	
	int kukaPort = 49938;
	int kukaStepWaitTime = 1.8 * 1e4;
	
	double tStart = 0.0;
	double tEnd = 6.5 + 2;
	
	double pickupStiffness = 2200;
	double catchStiffness = 1500;
	double screwStiffness = 1000;
	
	int plotNum = columns - 1;
	double close_ratio = 0.0;
	double dmpStepSize = kukaStepWaitTime * 1e-6;
	double tolAbsErr = 1e-3;
	double tolRelErr = 1e-3;
	std::thread* thr = NULL;
	SchunkHand* hand = NULL;
	Gnuplot* g1 = NULL;
	KukaControlQueue* movementQu = NULL;
	
	destructList = new vector<DestroyableObject*>();
	
	try {
	
		if(!strcmp(args[1], "-exe") || !strcmp(args[1], "-sim")) {

			int doSimulation;
			
			if(!strcmp(args[1], "-exe")) {
				cout << "execute dmp" << endl;
				doSimulation = 0;
			}
			else {
				cout << "simulate dmp" << endl;
				doSimulation = 1;
			}

			float timeCounter = 0.0;
			float* jointValues = NULL;
			float* currentJoints = NULL;
			
			DMPGeneralizer* dmpGen = new DMPGeneralizer(pickupGen, columns - 1, pickupTmpmys, tmpsigmas, az, bz);
			
			vec newQueryPoint(1);
			newQueryPoint(0) = 6.5;
//			newQueryPoint(1) = 0.0;
			
			TricubeKernel* tricubeKernel = new TricubeKernel();
			QuadraticKernel* quadraticKernel = new QuadraticKernel();
			GaussianKernel* gaussianKernel = new GaussianKernel(10.0, 1.0);
			t_learned_dmp pickDmps = dmpGen->generalizeDmp(tricubeKernel, gaussianKernel, newQueryPoint, 100000);
			
			TrajectoryDMPLearner drillLearner(catchTmpmys, tmpsigmas, az, bz, drillFile, columns - 1);
			t_learned_dmp drillDmps = drillLearner.fitTrajectories();
			
			TrajectoryDMPLearner screwLearner(screwTmpmys, tmpsigmas, az, bz, screwFile, columns - 1);
			t_learned_dmp screwDmps = screwLearner.fitTrajectories();
			
			t_executor_res* pickupSampleResults;
			
			int samples = dmpGen->getQueryPointCount();
			pickupSampleResults = new t_executor_res[samples + 1];
			
			for(int i = 0; i < samples; ++i) {
				DMPExecutor samplePickups(dmpGen->getQueryPointByIndex(i).dmp);
				pickupSampleResults[i] = samplePickups.simulateDMP(tStart, tEnd, dmpStepSize, tolAbsErr, tolRelErr);
			}
			
			DMPExecutor samplePickups(pickDmps);
			pickupSampleResults[samples] = samplePickups.simulateDMP(tStart, tEnd - 1, dmpStepSize, tolAbsErr, tolRelErr);

			if(!doSimulation) {
				
				hand = new SchunkHand(handPort);
			
			//	include again as soon as schunk hand bug is solved
			//	destructList->push_back(hand);
				
				hand->connectHand();
				hand->closeHand(0.0, handVelocity);
				
				// Example for ControlQueue usage
				movementQu = new KukaControlQueue(kukaPort, kukaStepWaitTime, COMMAND_DEMO_COMMAND_MODE);
				destructList->push_back(movementQu);

				// cpdaming is important as long as the calibration is not good
				movementQu->setStiffness(pickupStiffness, 300, 1.0, 15000, 150, 2.0);
				
				double* tmp = createDoubleArrayFromArmaVector(pickDmps.y0);
				float* startingJoints = new float[columns - 1];
				for(int i = 0; i < (columns - 1); ++i) startingJoints[i] = tmp[i];
				movementQu->setStartingJoints(startingJoints);
				thr = new std::thread(&KukaControlQueue::run, movementQu);

				while(!movementQu->isInitialized());
				printf("(main) robot connection initialized\n");
				currentJoints = movementQu->getCurrentJoints().joints;
				printf("(main) begin joint [%f,%f,%f,%f,%f,%f,%f]\n", currentJoints[0], currentJoints[1], currentJoints[2], currentJoints[3], currentJoints[4], currentJoints[5], currentJoints[6]);
				fflush(stdout);

			}
			
			DMPExecutor pickupDmpexec(pickDmps);
			t_executor_res pickupDmpResult;
			
			if(doSimulation) {
				pickupDmpResult = pickupSampleResults[samples];
				
			} else {
				
				pickupDmpResult = pickupDmpexec.runDMP(ac, tStart, tEnd, dmpStepSize, tolAbsErr, tolRelErr, movementQu);
				
				hand->closeHand(1.0, handVelocity);
				
				movementQu->setFinish();
				thr->join();
				std::cout << "(main) lifting object" << endl;
				std::cout.flush();
				
				movementQu->setAdditionalLoad(0.23, 120);
				movementQu->moveJoints(movementQu->getStartingJoints());
				
			}
			/*
			std::cout << "(main) starting catch execution\n"; std::cout.flush();
			DMPExecutor drillDmpexec(drillDmps);
			t_executor_res drillDmpResult;
			if(doSimulation) drillDmpResult = drillDmpexec.simulateDMP(tStart, drillDmps.tmax, dmpStepSize, tolAbsErr, tolRelErr);
			else {
				
				cout << "(main) moving to catch position" << endl;
				
				// select point in between such that robot does not crash into chair
				float inbetweenjoints[7] = {1.88425, -0.368607, -1.05401, -1.5363, 0.980371, -0.026814, 0.000288049};
				movementQu->moveJoints(inbetweenjoints);
				
				double* tmp2 = createDoubleArrayFromArmaVector(drillDmps.y0);
				float* startingJoints2 = new float[columns - 1];
				for(int i = 0; i < (columns - 1); ++i) startingJoints2[i] = tmp2[i];
				movementQu->setStartingJoints(startingJoints2);
				movementQu->setStiffness(catchStiffness, 300, 1.0, 15000, 150, 2.0);
				thr = new std::thread(&KukaControlQueue::run, movementQu);

				while(!movementQu->isInitialized());
				printf("(main) robot connection initialized\n");
				currentJoints = movementQu->getCurrentJoints().joints;
				printf("(main) begin joint [%f,%f,%f,%f,%f,%f,%f]\n", currentJoints[0], currentJoints[1], currentJoints[2], currentJoints[3], currentJoints[4], currentJoints[5], currentJoints[6]);
				fflush(stdout);
				
				drillDmpResult = drillDmpexec.runDMP(ac, tStart, drillDmps.tmax, dmpStepSize, tolAbsErr, tolRelErr, movementQu);
				std::cout << "(main) object catched\n"; std::cout.flush();
				
				hand->closeHand(0.0, handVelocity);
				
				std::cout << "(main) catching done\n"; std::cout.flush();
				
			}
			
			if(!doSimulation) {
				movementQu->setFinish();
				thr->join();
			}
			
			std::cout << "(main) starting screw execution\n"; std::cout.flush();
			DMPExecutor screwDmpexec(screwDmps);
			t_executor_res screwDmpResult;
			if(doSimulation) screwDmpResult = screwDmpexec.simulateDMP(tStart, screwDmps.tmax, dmpStepSize, tolAbsErr, tolRelErr);
			else {
				
				cout << "(main) moving to screw position" << endl;
				
				double* tmp2 = createDoubleArrayFromArmaVector(screwDmps.y0);
				float* startingJoints2 = new float[columns - 1];
				for(int i = 0; i < (columns - 1); ++i) startingJoints2[i] = tmp2[i];
				movementQu->moveJoints(startingJoints2);
				movementQu->setStartingJoints(startingJoints2);
				movementQu->setStiffness(screwStiffness, 100, 1.0, 15000, 150, 2.0);
				
				thr = new std::thread(&KukaControlQueue::run, movementQu);
				while(!movementQu->isInitialized());
				printf("(main) robot connection initialized\n");
				currentJoints = movementQu->getCurrentJoints().joints;
				printf("(main) begin joint [%f,%f,%f,%f,%f,%f,%f]\n", currentJoints[0], currentJoints[1], currentJoints[2], currentJoints[3], currentJoints[4], currentJoints[5], currentJoints[6]);
				fflush(stdout);
				
				hand->closeHand(1.0, handVelocity);
			
				screwDmpResult = screwDmpexec.runDMP(ac, tStart, screwDmps.tmax, dmpStepSize, tolAbsErr, tolRelErr, movementQu);
				std::cout << "(main) object screwed\n"; std::cout.flush();
				
				hand->closeHand(0.0, handVelocity);
				
				std::cout << "(main) screwed done\n"; std::cout.flush();
				
			}
			
			if(!doSimulation) {
				movementQu->setFinish();
				thr->join();
			}
			
			float inbetweenjoints[7] = {2.41108, -1.42299, -0.330589, -1.73406, -1.95417, 0.00287363, 0.01166};
			movementQu->moveJoints(inbetweenjoints);
			*/
			if(!doSimulation) hand->disconnectHand();
			
			cout << "(main) plotting trajectory for query point [ ";
			for(int i = 0; i < newQueryPoint.n_elem; ++i) cout << newQueryPoint(i) << " ";
			cout << "]" << endl;
			
			for(int plotTraj = 0; plotTraj < plotNum; ++plotTraj) {
				
				ostringstream convert;   // stream used for the conversion
				convert << plotTraj;
				
				string title = string("fitted sensor data (joint") + convert.str() + string(")");
				g1 = new Gnuplot(title);
				
				for(int i = 0; i < dmpGen->getQueryPointCount(); ++i) {
					
					std::ostringstream o;
					o << "sample (";
					
					for(int j = 0; j < dmpGen->getQueryPointByIndex(i).queryPoint.n_elem; ++j) {
						o << dmpGen->getQueryPointByIndex(i).queryPoint(j) << " ";
					}
					o << ")";
					g1->set_style("lines").plot_xy(pickupSampleResults[i].t, pickupSampleResults[i].y[plotTraj], o.str());
					
				}
				
				g1->set_style("lines").plot_xy(pickupDmpResult.t, pickupDmpResult.y[plotTraj], "executed gen y");
				g1->set_style("lines").plot_xy(pickupSampleResults[dmpGen->getQueryPointCount()].t, pickupSampleResults[dmpGen->getQueryPointCount()].y[plotTraj], "simulated gen dmp");
				g1->showonscreen();
				
			}

		}
		
	} catch(cSDHLibraryException* e) {
		std::cerr << "demo-griphand main(): An exception was caught: " << e->what() << "\n";
		delete e;
	}
	
	getch();
	
	return 0;

}

char consoleInputter() {
	cin >> consoleInput;
}

void got_signal(int in) {

	for(int i = 0; i < destructList->size(); ++i) {
		DestroyableObject* destr = destructList->at(i);
		destr->safelyDestroy();
	}
	cout << "everything safely stopped" << endl;
	cout.flush();
	usleep(0.3 * 1e6);
	exit(1);

}