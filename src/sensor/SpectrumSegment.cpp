/*
 * SpectrumSegment.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: rocapal
 */

#include "SpectrumSegment.h"


SpectrumSegment::SpectrumSegment(long sensorId,
			struct timespec timeStamp,
			long centerFrequency,
			long samplingRate,
			std::vector<unsigned char> samples)
{

	mSensorID = sensorId;
	mTimestamp = timeStamp;
	mCenterFrequency = centerFrequency;
	mSamplingRate = samplingRate;
	mSamples = samples;
}

SpectrumSegment::~SpectrumSegment() {
	// TODO Auto-generated destructor stub
}

