/*
 *  Copyright (C) IMDEA Networks 2016
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Author : Roberto Calvo Palomino <roberto [dot] calvo [at] imdea [dot] org>
 *
 */

#ifndef SRC_SENSOR_SPECTRUMSEGMENT_H_
#define SRC_SENSOR_SPECTRUMSEGMENT_H_

#include <vector>
#include <time.h>

class SpectrumSegment {
public:

	SpectrumSegment() {};
	SpectrumSegment(long sensorId,
			struct timespec timeStamp,
			long centerFrequency,
			long samplingRate,
			long downSampling,
			std::vector<unsigned char> samples);

	virtual ~SpectrumSegment();

	long getSensorId () { return mSensorID;};
	timespec getTimeStamp() { return mTimestamp;};
	long getCenterFrequency() { return mCenterFrequency; };
	long getSamplingRate() {return mSamplingRate; };
	long getDownSampling() {return mDownSampling; };
	std::vector<unsigned char> getSamples() { return  mSamples; };

private:

	long mSensorID;
	struct timespec mTimestamp;
	long mCenterFrequency;
	long mSamplingRate;
	long mDownSampling;
	std::vector<unsigned char> mSamples;
};

#endif /* SRC_SENSOR_SPECTRUMSEGMENT_H_ */
