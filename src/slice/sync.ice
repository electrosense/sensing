/*
 *  Copyright (C) 2016 Electrosense
 *
 *
 *  Authors : Roberto Calvo Palomino <roberto.calvo@imdea.org>
 */


#ifndef SYNCHRONIZATION_ICE
#define SYNCHRONIZATION_ICE


module Electrosense {


	class Time {

		long sec;
		long nsec;		
                
	};
	
	class ScanningParameters {
		long frequency;			// Hz
		long samplingRate;		// Hz
		long downSample;		// Hz
		long chunkSize;			// IQ samples
		long overlapSize;		// IQ samples
		long duration;			// seconds
	};


	interface Synchronization {

		void start(Time reference, Time delay, ScanningParameters parameters);

	};

};


#endif
