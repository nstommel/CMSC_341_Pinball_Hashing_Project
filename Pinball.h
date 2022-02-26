
/******************************************************************************
 * File:    Pinball.h
 * Project: CMSC 341 Project 5, Fall 2016
 * Author:  Nicholas Stommel
 * Date:    12/06/16
 * Section: 05
 * E-mail:  stommel1@gl.umbc.edu
 * 
 * Description: This is the header file containing the declaration of 
 * the Pinball class.
 * 
 ******************************************************************************/

#ifndef _PINBALL_H_
#define _PINBALL_H_

#include <string>
#include <stdexcept>
using namespace std ;

// *** Note: Debugging output can be turned on by setting DEBUG constant to
// true in Pinball.cpp.

// Exception class, used in insert method of Pinball class.
class PinballHashFull : public std::out_of_range {

public:
	PinballHashFull(const string& what) : std::out_of_range(what) { }
};


class Pinball {

public:

	//-------------------------------------------------------
	// Name: Pinball hash table constructor
	// PreConditions: none (n should be prime, however)
	// PostConditions: allocates memory for member variables
	// Notes: srand() seeded with current time to determine random seeds
	// for each primary slot.
	//---------------------------------------------------------
	Pinball(int n=1019);    // default capacity is prime
	
	//-------------------------------------------------------
	// Name: Pinball hash table destructor
	// PreConditions: none
	// PostConditions: deallocates memory for member variable arrays and cstrings
	// Notes: free() used for deallocation of cstrings in H
	//---------------------------------------------------------
	~Pinball();

	//-------------------------------------------------------
	// Name: insert
	// PreConditions: str must be valid cstring and str cannot be a duplicate
	// PostConditions: allocates memory for string, attempts insertion to H array,
	// and adjusts counts accordingly
	// Notes: Can throw exception PinballHashFull
	//---------------------------------------------------------
	void insert(const char *str);
	
	//-------------------------------------------------------
	// Name: find
	// PreConditions: str must be valid cstring (check for NULL performed)
	// PostConditions: returns index of requested string or -1 if not found
	//---------------------------------------------------------
	int find(const char *str);
	
	//-------------------------------------------------------
	// Name: at
	// PreConditions: index must be valid
	// PostConditions: returns pointer to const cstring
	// Notes: Can throw std::out_of_range exception
	//---------------------------------------------------------
	const char * at(int index);
	
	//-------------------------------------------------------
	// Name: remove
	// PreConditions: str must be valid cstring
	// PostConditions: returns pointer to requested cstring, removes it from H,
	// adjusts counts accordingly, or if not found returns null.
	//---------------------------------------------------------
	char * remove(const char *str);
	
	//-------------------------------------------------------
	// Name: printStats
	// PreConditions: none
	// PostConditions: prints relevant hash table stats to cout.
	//---------------------------------------------------------
	void printStats();

	//-------------------------------------------------------
	// Name: size
	// PreConditions: none
	// PostConditions: returns integer representing current table size.
	//--------------------------------------------------------- 
	int size() { return m_size ; }

	// add public data members and 
	// public member functions as needed
	
	//-------------------------------------------------------
	// Name: printHashTable
	// PreConditions: none
	// PostConditions: prints contents of the hash table in list
	// form, null pointers are indicated with 'NULL'
	//---------------------------------------------------------
	void printHashTable() const;

	// Simple one-line getters for member variables useful for gathering
	// table data in driver program
	int getDegree() const;
	int getEjectLimit() const;
	int getNumPrimarySlots() const;
	double getAvgPrimaryHits() const;
	int getMaxPrimaryHits() const;
	int getTotalEjections() const;
	int getMaxEjections() const;
	
private:
 
	char ** H;        // the actual hash table
	int m_size;       // number of items stored in H
	int m_capacity;   // number slots allocated in H

	int m_degree;     // use degree assigned to you
	int m_ejectLimit; // use ejection limit assigned to you

	// Made inline definition of hashCode() so everyone uses
	// the same function.
	//
	unsigned int hashCode(const char *str) {
		
		unsigned int val = 0; 
		const unsigned int thirtyThree = 33;  // magic number from textbook

		int i = 0 ;
		while (str[i] != '\0') {
			val = val * thirtyThree + str[i];
			i++;
		}
		return val; 
	}

	// add private data members and 
	// private member functions as needed
	
	// Array of random seeds for each slot in hash table
	int* H_seeds;
	
	// Array of integers representing primary slot hits for each slot in
	// the hash table
	int* H_primaryHits;
	
	// The number of slots in the table currently considered primary slots
	int m_numPrimarySlots;
	// the maximum number of items that hash to the same primary slot	
	int m_maxPrimaryHits;
	// the number of ejections performed in all insertions since the 
	// hash table was created
	int m_totalEjections;
	// the maximum number of ejections in a single insertion
	int m_maxEjections;

};

#endif
