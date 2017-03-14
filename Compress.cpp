/* File: Compress.cpp
 * Author: Aaron Skouby
 * Last Modified: March 13, 2017
 * 
 * Library of functions for compressing and decompressing files
 */
 
/* Includes */
#include "Compress.h"

#include <iostream>
#include <thread>

void Compress::chooseReps (string fileName, repType * ans){
	atomic_ulong count[(((ulong)1)<<(repLen*8))]; //won't be able to do last num
	
	for(ulong i = 0; i<(((ulong)1)<<(repLen*8)); i++){
		count[i] = 0;
	}
	
	//getting file size (TODO: maybe slow)
	fstream file(fileName.c_str(), ios::in | ios::out | ios::binary |ios::ate);
	streampos fileEnd = file.tellg();
		
	//create threads to count occurrences
	thread counters[numThreads];
	for(int i = 0; i < numThreads; i++){
		counters[i] = thread(makeCount, fileName, (streampos)((fileEnd*i)/numThreads), (streampos)((fileEnd*(i+1))/numThreads), count);
	}
	for(int i = 0; i<numThreads; i++){
		counters[i].join();
	}
		
	findMaxOccur(count, ans);
}

void Compress::makeCount (string fileName, streampos start, streampos end, atomic_ulong * count){		
	//make new stream for each
	fstream file(fileName.c_str());
	file.seekg(start);
	
	//put first 3 bytes into the number
	unsigned long currNum = 0;
	for (int i = 0; i<(repLen-1); i++){
		currNum <<= 8;
		currNum |= file.get();
	}
	
	//increase counts
	while(file.tellg() != end){ //end should not be past eof
		currNum <<= 8;
		currNum |= file.get();
		currNum &= (((ulong)1)<<(repLen*8))-1; //clear out extra bits
		count[currNum]++;
	}	
}

void Compress::findMaxOccur (atomic_ulong * count, repType * ans){
	pair<ulong,ulong> maxCounts[numRep]; //1st count, 2nd value
	for(int i =0; i<numRep; i++){
		maxCounts[i] = make_pair(0,0);
	}
	
	for(ulong i =0; i<(((ulong)1)<<(repLen*8)); i++){ //may be too big
		pair<ulong,ulong> curr = make_pair((ulong)(count[i]),i);
		for(int j =0; j<numRep; j++){
			if (curr.first >= maxCounts[j].first){
				pair<ulong,ulong> temp = maxCounts[j];
				maxCounts[j] = curr;
				curr = temp;
			}
		}
	}
	
	for (int i = 0; i<numRep; i++){
		cout<<maxCounts[i].first<<" "<<maxCounts[i].second<<endl;
	}
	
	//turn pairs into repType
	for(int i =0; i<numRep; i++){
		ans[i] = maxCounts[i].second;
	}
}

void Compress::compress(string fileName){
	//open file
	fstream file(fileName.c_str());
	if(!file.is_open()){
		cerr<<"File didn't open"<<endl;
		return; //TODO: More robust error
	}
	
	//find strings of bits that will be replaced
	repType reps[numRep];
	chooseReps(fileName, reps);
	
	for (int i = 0; i<numRep; i++){
		cout<<reps[i]<<endl;
	}
	
	//replace reps witha shorter string (and mark where replacements are

	file.close();
}
