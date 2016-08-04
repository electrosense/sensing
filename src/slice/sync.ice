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



	interface Synchronization {

		void start(Time reference, Time delay);

	};

};


#endif
