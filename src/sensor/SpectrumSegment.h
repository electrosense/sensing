/*
 * SpectrumSegment.h
 *
 *  Created on: Aug 5, 2016
 *      Author: rocapal
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
