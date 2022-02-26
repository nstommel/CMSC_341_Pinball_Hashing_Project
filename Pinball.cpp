
/******************************************************************************
 * File:    Pinball.cpp
 * Project: CMSC 341 Project 5, Fall 2016
 * Author:  Nicholas Stommel
 * Date:    12/06/16
 * Section: 05
 * E-mail:  stommel1@gl.umbc.edu
 * 
 * Description: This is the source file containing the implementation of 
 * the Pinball class.
 * 
 ******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

#include "Pinball.h"

// Change constant to true to turn on debugging output.
#define DEBUG false

Pinball::Pinball(int n) {
	
	m_capacity = n;
	m_size = 0;
	
	// Project assigned parameters
	m_ejectLimit = 14;
	m_degree = 9;
	
	// Initialize helper statistics variables to zero
	m_numPrimarySlots = 0;
	m_maxPrimaryHits = 0;
	m_totalEjections = 0;
	m_maxEjections = 0;
	
	// Allocate memory for array of c-strings
	//H = (char**) malloc(m_capacity * sizeof(char*));
	// Initialize all char* pointers to NULL
	//memset(H, 0, m_capacity * sizeof(char*));
	
	H = new char*[m_capacity]();
	
	// Allocate memory for array of seeds for each slot in hash table
	H_seeds = new int[m_capacity]();
	
	// Fill seeds array, first seed random number generator with current time
	srand(time(NULL));
	
	// Uncomment to use fixed seed for debugging
	//srand(1480707218);
	
	for(int i = 0; i < m_capacity; ++i) {
		H_seeds[i] = rand();
	}
	
	// Allocate memory for corresponding array of primary hits
	// Initialize all hit values to zero with ()
	H_primaryHits = new int[m_capacity]();
		
}

Pinball::~Pinball() {
	
	// Free individual c-strings inside H array
	for(int i = 0; i < m_capacity; ++i) {
		free(H[i]);
	}
	// Free remaining memory
	delete [] H;
	delete [] H_seeds;
	delete [] H_primaryHits;
	

}

const char* Pinball::at(int index) {
	
	// Check whether index is in range
	if(index < 0 || index >= m_capacity) {
		throw std::out_of_range("*** Exception: Invalid index, out of range!");
	}
	
	// Note that this function may return a NULL pointer, as no string may
	// be present at the index in H.
	return H[index];
}

int Pinball::find(const char* str) {
	
	// Failsafe (don't feed find NULL pointers!) - returns -1 for 'missing'
	if(str == NULL) {
		return -1;
	}
	
	if(DEBUG) {
		cout << "Called find on " << str << endl;
	}
	
	// Calculate primary slot index
	unsigned int pIndex = hashCode(str) % m_capacity;
	
	if(DEBUG) {
		cout << "primary slot index = " << pIndex << endl;
	}
	
	// If the string is in the primary slot, return that index
	if(H[pIndex] != NULL && strcmp(str, H[pIndex]) == 0) {
		
		if(DEBUG) {
			cout << "string found in primary slot" << endl;
		}
		return pIndex;
	}
	
	// Seed random number generator with stored seed at primary slot
	srand(H_seeds[pIndex]);
	
	// Check auxiliary slots until a match is found or all slots are checked.
	for(int i = 0; i < m_degree - 1; ++i) {
		
		unsigned int auxIndex = rand() % m_capacity;
		
		// If string is found in auxiliary slot, return that slot's index.
		if(H[auxIndex] != NULL && strcmp(str, H[auxIndex]) == 0) {
			
			if(DEBUG) {
				cout << "string found in aux slot" << endl;
			}
			return auxIndex;
		}
	}
	
	if(DEBUG) {
		cout << "string not found" << endl;
	}
	
	// If item was not found in primary or any auxiliary slots, return -1.
	return -1;
}

void Pinball::insert(const char* str) {
	
	if(DEBUG) {
		cout << "Called insert on " << str << endl;
	}
	
	// Throw exception if hash table size cannot be increased (max capacity reached)
	if(m_size == m_capacity) {
		throw PinballHashFull("*** Exception: Hash table full, size cannot exceed capacity!");
	}
	
	// If the same string exists in the hash table, do nothing and return.
	if(find(str) != -1) {
		
		if(DEBUG) {
			cout << "String already exists in hash table, no action taken." << endl;
		}
		return;
	}
	
	
	// Calculate primary slot index
	unsigned int pIndex = hashCode(str) % m_capacity;
	
	// Increment number of primary hits for that slot (does not happen during
	// ejection because the ejected string's primary slot hits have already been
	// counted) If the slot is first hit, increment the total number of primary 
	// slots in use.
	if((H_primaryHits[pIndex])++ == 0) {
		++m_numPrimarySlots;
	}
	
	// If the number of primary hits for the primary slot is greater than the
	// current maximum, assign it to m_maxPrimaryHits.
	if(H_primaryHits[pIndex] > m_maxPrimaryHits) {
			m_maxPrimaryHits = H_primaryHits[pIndex];
	}
	
	// If the primary hits for that slots exceeds the degree, throw an exception
	// Re-hashing is necessary in this case.
	if(H_primaryHits[pIndex] > m_degree) {
		throw PinballHashFull("*** Exception: Max hits to a primary slot cannot exceed degree!");
	}
	
	// Allocate memory for new insertable string
	char* insertable = strdup(str);

	int numEjections = 0;
	
	while(true) {	
		
		// If primary slot is empty, put newly allocated string in slot and exit function.
		if(H[pIndex] == NULL) {
			H[pIndex] = insertable;
			
			// If the number of ejections for this insertion exceeds the current
			// max, assign it to m_maxEjections.
			if(numEjections > m_maxEjections) {
				m_maxEjections = numEjections;
			}
			if(DEBUG) {
				cout << "Success, empty primary slot found" << endl;
			}
			
			++m_size;
			return;
		}
		
		// Save randomness
		int savedSeed = rand();
		// Seed random number generator with stored seed at primary slot
		srand(H_seeds[pIndex]);

		// Store array of aux slots
		unsigned int aux[m_degree - 1];
		
		// Check auxiliary slots until an empty slot is found or all slots are checked.
		for(int i = 0; i < m_degree - 1; ++i) {

			aux[i] = rand() % m_capacity;

			// If empty auxiliary slot is found, put string there and
			// exit the function.
			if(H[aux[i]] == NULL) {
				H[aux[i]] = insertable;
				
				// If the number of ejections for this insertion exceeds the current
				// max, assign it to m_maxEjections.
				if(numEjections > m_maxEjections) {
					m_maxEjections = numEjections;
				}
				if(DEBUG) {
					cout << "success, empty aux slot " << aux[i] << " found" << endl;
				}
				
				++m_size;
				return;
			}
		}
		
		// If we reach this point, it means that an ejection must occur. Primary and all
		// auxiliary slots are full.
		++numEjections;
		++m_totalEjections;
		
		// If the ejection limit is reached, throw an exception
		if(numEjections > m_ejectLimit) {
			// The function will exit here, so update m_maxEjections
			m_maxEjections = numEjections;
			
			// Free memory for 'dangling' word
			free(insertable);
			throw PinballHashFull("*** Exception: Maximum ejections reached!");
		}
		
		// Restore randomness for picking auxiliary slot for ejection
		srand(savedSeed);
		int pickAux = rand() % (m_degree - 1);
		
		if(DEBUG) {
			cout << "pickAuxIndex = " << pickAux << endl;
			cout << "All aux slots full, ejection must occur from " << aux[pickAux] << endl;
		}
		
		// Set insertable and pIndex for use in next iteration of loop,
		// replace the ejected string with the new one.
		
		char* pick = H[aux[pickAux]];
		
		if(DEBUG) {
			cout << "Ejecting " << H[aux[pickAux]] << " from slot " << aux[pickAux] << endl;
		}
		H[aux[pickAux]] = insertable;
		
		insertable = pick;
		pIndex = hashCode(insertable) % m_capacity;
	}
}

char* Pinball::remove(const char* str) {
	
	if(DEBUG) {
		cout << "called remove on " << str << endl;
	}
	
	int index = find(str);
	
	if(index == -1) {
		
		if(DEBUG) {
			cout << "string not found, returning NULL" << endl;
		}
		return NULL;
		
	} else {
		
		if(DEBUG) {
			cout << "string found, removing..." << endl;
		}
		--m_size;
		
		// Decrement number of hits to string's primary slot
		// If the slot now has zero hits, decrement the total number of primary
		// slots in use.
		if(--(H_primaryHits[hashCode(str) % m_capacity]) == 0) {
			--m_numPrimarySlots;
		}

		// Save the pointer to the removed string for return
		char* temp = H[index];
		// Set the pointer at the index of the removed string to null 
		// (no string exists there now and the slot is free for other insertions)
		H[index] = NULL;
		
		return temp;
	}
	
}

void Pinball::printStats() {
	
	cout << "Hash statistics report:" << endl;
	cout << "   randomness level = pseudo-random" << endl;
	cout << "   capacity = " << m_capacity << endl;
	cout << "   size = " << m_size << endl;
	cout << "   degree = " << m_degree << endl;
	cout << "   ejection limit = " << m_ejectLimit << endl;
	cout << "   number of primary slots = " << m_numPrimarySlots << endl;
	cout << "   average hits to primary slots = " << (double)m_size / m_numPrimarySlots << endl;
	cout << "   maximum hits to primary slots = " << m_maxPrimaryHits << endl;
	cout << "   total number of ejections = " << m_totalEjections << endl;
	cout << "   maximum number of ejections in a single insertion = " << m_maxEjections << endl;
	cout << endl;

}

void Pinball::printHashTable() const {
	
	cout << "Hash Table Contents" << endl;
	cout << '[';
	for(int i = 0; i < m_capacity; ++i) {
		if(H[i]) {
			cout << H[i];
		} else {
			cout << "NULL";
		}
		if(i < m_capacity - 1) {
			cout << ", ";
		}
	}
	cout << ']' << endl;
}

int Pinball::getDegree() const {
	return m_degree;
}

int Pinball::getEjectLimit() const {
	return m_ejectLimit;
}

int Pinball::getNumPrimarySlots() const {
	return m_numPrimarySlots;
}

double Pinball::getAvgPrimaryHits() const {
	return (double)m_size / m_numPrimarySlots;
}

int Pinball::getMaxPrimaryHits() const {
	return m_maxPrimaryHits;
}

int Pinball::getTotalEjections() const {
	return m_totalEjections;
}

int Pinball::getMaxEjections() const {
	return m_maxEjections;
}

