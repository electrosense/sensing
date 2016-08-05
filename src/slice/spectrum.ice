/*
 *  Copyright (C) 2016 Electrosense
 *
 *
 *  Authors : Roberto Calvo Palomino <roberto.calvo@imdea.org>
 */

#ifndef SPECTRUM_ICE
#define SPECTRUM_ICE


#include <Ice/BuiltinSequences.ice>
#include "sync.ice"

module Electrosense {


	class SpectrumSegment {

		long sensorID;
		Time timestamp;
		long centerFrequency;
		long samplingRate;
		Ice::ByteSeq samples;
		                
	};		


	interface Spectrum  {

		 void push (SpectrumSegment segment);

	};

};


#endif
 