
/******************************************************************************
 * File:    myDriver.cpp
 * Project: CMSC 341 Project 5, Fall 2016
 * Author:  Nicholas Stommel
 * Date:    12/06/16
 * Section: 05
 * E-mail:  stommel1@gl.umbc.edu
 * 
 * Description: This is the main driver file, modified to calculate all
 * relevant statistics and averages for the specified number of trials.
 * 
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "words.h"
#include "Pinball.h"

using namespace std ;


// Get amount of user time since the program began in milliseconds.
//
double getmsecs() {
	struct rusage use ; 
	struct timeval utime ;

	getrusage(RUSAGE_SELF, &use) ;
	utime = use.ru_utime ;
	return ((double) utime.tv_sec)*1000.0 +  ((double) utime.tv_usec)/1000.0 ;
}


// This version uses lrand48().
// Set the random seed by passing a non-zero parameter.
//
int myRand(int seed=0) {
	if (seed) srand48(seed) ;
	return lrand48() ;
}

// prints name and section to output stream
void printGreeting(ostream& os) {
	
	os << "Nicholas Stommel, Section 05" << endl << endl;
}

// Data struct returned by test() for use in storing data and computing 
// averages in main().
struct Data {
	
	int capacity;
	int loadFactor;
	int degree;
	int ejectLimit;
	
	double elapsedTime;
	double size;
	double numPrimarySlots;
	double avgPrimaryHits;
	double maxPrimaryHits;
	double totalEjections;
	double maxEjections;
	
	// Default constructor initializes all members to zero
	Data() : capacity(0), loadFactor(0), degree(0), ejectLimit(0),
	elapsedTime(0), size(0), numPrimarySlots(0), avgPrimaryHits(0), 
	maxPrimaryHits(0), totalEjections(0), maxEjections(0)
	{}
};

// Exercise the Pinball Hash table with size slots
// inserting reps items.
//
// Modified to return useful Data c-struct for storing all test data in main()
Data test(int size, int reps) {

	int index ;
	int slot ;
	double startTime, stopTime ;

	Pinball PH(size) ;

	startTime = getmsecs() ;

	// Insert reps randomly chosen items
	//
	for (int i=0 ; i < reps ; i++) {

		// don't choose items already in the hash table
		do { 
			index = myRand() % numWords ;
			slot = PH.find(words[index]) ;
		} while(slot > -1) ;
		try {

			PH.insert(words[index]) ;  // might throw exception


			// double check that inserted items were stored properly
			// for debugging.

			/*
			slot = PH.find(words[index]) ;
			if (slot >=0 && strcmp(PH.at(slot),words[index]) != 0) {
			   cout << "Inserted word not stored at slot = " << slot << endl ;    
			   cout << "   words[" << index << "] = " << words[index] << endl ;
			   cout << "   PH.at(" << slot << ") = " << PH.at(slot) << endl ;
			}
			*/

		} catch (PinballHashFull &e) {
			cout << e.what() << endl ;
			break ;
		} catch (...) {
			cout << "Unknown error\n" ;
		}
	}

	stopTime = getmsecs() ;
	printf("Elapsed time = %.3lf milliseconds\n", stopTime - startTime) ;

	PH.printStats() ;
	
	//---------------------------------------------------------------
	// Added code to return data struct for Test below:
	
	Data testData;
	testData.degree = PH.getDegree();
	testData.ejectLimit = PH.getEjectLimit();
	testData.elapsedTime = stopTime - startTime;
	testData.size = PH.size();
	testData.numPrimarySlots = PH.getNumPrimarySlots();
	testData.avgPrimaryHits = PH.getAvgPrimaryHits();
	testData.maxPrimaryHits = PH.getMaxPrimaryHits();
	testData.totalEjections = PH.getTotalEjections();
	testData.maxEjections = PH.getMaxEjections();
	
	return testData;
}

// Function returns a vector of data for specified number of trials
// given a capacity and load factor, calculates averages and stores in last
// vector slot.
vector<Data> getTestData(int capacity, int loadFactor, int numTrials) {
	
	// Create a trialDataBank for the specified capacity and load factor
	vector<Data> trialDataBank;

	for(int i = 0; i < numTrials; ++i) {
		Data trialData = test(capacity, capacity * loadFactor / 100);
		trialData.capacity = capacity;
		trialData.loadFactor = loadFactor;
		trialDataBank.push_back(trialData);
	}
	
	// Calculate averages for all trials
	Data avgData;
	
	// First add up totals for all trials
	for(int i = 0; i < numTrials; ++i) {
		avgData.elapsedTime += trialDataBank[i].elapsedTime;
		avgData.size += trialDataBank[i].size;
		avgData.numPrimarySlots += trialDataBank[i].numPrimarySlots;
		avgData.avgPrimaryHits += trialDataBank[i].avgPrimaryHits;
		avgData.maxPrimaryHits += trialDataBank[i].maxPrimaryHits;
		avgData.totalEjections += trialDataBank[i].totalEjections;
		avgData.maxEjections += trialDataBank[i].maxEjections;
	}
	// Then divide by number of trials to obtain final average
	avgData.elapsedTime /= numTrials;
	avgData.size /= numTrials;
	avgData.numPrimarySlots /= numTrials;
	avgData.avgPrimaryHits /= numTrials;
	avgData.maxPrimaryHits /= numTrials;
	avgData.totalEjections /= numTrials;
	avgData.maxEjections /= numTrials;
	
	// Add average data to end of data bank
	trialDataBank.push_back(avgData);
	
	return trialDataBank;
}

// Prints out stats from a data vector from getTestData to a specified output
// stream. A boolean parameter is included for choosing to print only the
// averages or all test data and averages.
void printData(const vector<Data>& data, bool printAll, ostream& os) {
	
	int numTrials = data.size() - 1;
	
	os << endl << "*****  Printing Data for Test  *****" << endl << endl;
	os << "Randomness:  " << "pseudo-random" << endl;
	os << "Degree:      " << data[0].degree << endl;
	os << "Eject Limit: " << data[0].ejectLimit << endl;
	os << "Capacity:    " << data[0].capacity << endl;
	os << "Load Factor: " << data[0].loadFactor << '%' << endl;
	os << "# Trials:    " << numTrials << endl << endl;
	
	// If desired, all statistics will be printed to the given stream
	if(printAll) {
		for(int i = 0; i < numTrials; ++i) {
			os << "Trial # " << i + 1 << endl;
			os << "Elapsed Time (ms): " << data[i].elapsedTime << endl;
			os << "Size:              " << (int)data[i].size << endl;
			os << "# Primary Slots:   " << (int)data[i].numPrimarySlots << endl;
			os << "Avg Primary Hits:  " << data[i].avgPrimaryHits << endl;
			os << "Max Primary Hits:  " << (int)data[i].maxPrimaryHits << endl;
			os << "Total # Ejections: " << (int)data[i].totalEjections << endl;
			os << "Max # Ejections:   " << (int)data[i].maxEjections << endl;
			os << endl;
		}
	}
	
	os << "******   Average Statistics   ******" << endl;
	os << endl;
	os << "Elapsed Time (ms): " << data[numTrials].elapsedTime << endl;
	os << "Size:              " << data[numTrials].size << endl;
	os << "# Primary Slots:   " << data[numTrials].numPrimarySlots << endl;
	os << "Avg Primary Hits:  " << data[numTrials].avgPrimaryHits << endl;
	os << "Max Primary Hits:  " << data[numTrials].maxPrimaryHits << endl;
	os << "Total # Ejections: " << data[numTrials].totalEjections << endl;
	os << "Max # Ejections:   " << data[numTrials].maxEjections << endl;
	os << endl;
	os << "************************************" << endl << endl;
}

int main() {

	// set random seed to wall clock time in milliseconds
	//
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	printf("Random seed set to: %ld\n\n", ms) ;
	myRand(ms) ;
	srand(ms*37) ;   // other uses

	const int TRIALS = 10;
	
	// prints name and section
	printGreeting(cout);
	
	vector< vector<Data> > testDataBank;
	
	// 5003 slots, at 50,60,70,80,& 90% full
	testDataBank.push_back(getTestData(5003, 50, TRIALS));
	testDataBank.push_back(getTestData(5003, 60, TRIALS));
	testDataBank.push_back(getTestData(5003, 70, TRIALS));
	testDataBank.push_back(getTestData(5003, 80, TRIALS));
	testDataBank.push_back(getTestData(5003, 90, TRIALS));
	
	// 10037 slots, at 50,60,70,80,& 90% full
	testDataBank.push_back(getTestData(10037, 50, TRIALS));
	testDataBank.push_back(getTestData(10037, 60, TRIALS));
	testDataBank.push_back(getTestData(10037, 70, TRIALS));
	testDataBank.push_back(getTestData(10037, 80, TRIALS));
	testDataBank.push_back(getTestData(10037, 90, TRIALS));
	
	// 20101 slots, at 50,60,70,80,& 90% full
	testDataBank.push_back(getTestData(20101, 50, TRIALS));
	testDataBank.push_back(getTestData(20101, 60, TRIALS));
	testDataBank.push_back(getTestData(20101, 70, TRIALS));
	testDataBank.push_back(getTestData(20101, 80, TRIALS));
	testDataBank.push_back(getTestData(20101, 90, TRIALS));
	
	// NOTE: the output stream specified in the call to printData()
	// can be changed to cout for printing data to the console.
	//
	// By default, all data will be directed to cout, this can be modified
	// to output to a file by specifying a valid ofstream.
	//
	// If all information is desired including the averages, 
	// the boolean value 'true' can be passed for the printAll parameter 
	// in the call to printData(). Alternatively, if only the
	// average information is desired, 'false' can be passed to printData().
	
	// All data has been calculated and stored, now loop through
	// bank and print/output processed data + averages to specified stream.
	for(int i = 0; i < (int)testDataBank.size(); ++i) {
		printData(testDataBank[i], true, cout);
	}
	
   return 0 ;
}

