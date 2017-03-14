/* File: Compress.H
 * Author: Aaron Skouby
 * Last Modified: March 13, 2017
 * 
 * Library of functions for compressing and decompressing files
 */
 
#ifndef COMPRESS
#define COMPRESS

/* Includes */
#include <fstream>
#include <string>
#include <atomic>
#include <thread>
 
/* Defines */
#define repType unsigned long //type for a representitive
#define repLen 2 //number of bytes in a representitive
#define numRep 16 //number of representitives chosen
#define numThreads 10 //number of counting threads
#define ulong unsigned long

using namespace std;

class Compress{
	private:
		static void chooseReps (string fileName, repType * ans);
		static void makeCount (string fileName, streampos start, 
								streampos end, atomic_ulong * count);
		static void findMaxOccur (atomic_ulong * count, repType * ans);
	
	public:
		static void compress(string fileName);
};

#endif