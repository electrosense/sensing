/*
 * SDR.h
 *
 *  Created on: Aug 4, 2016
 *      Author: rocapal
 */

#ifndef SRC_SENSOR_SDR_H_
#define SRC_SENSOR_SDR_H_


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <rtl-sdr.h>
#include <iostream>
#include <vector>


#include "readerwriterqueue.h"

#include "Sender.h"
#include "SpectrumSegment.h"

using namespace moodycamel;

class SDR {

public:
	SDR();
	virtual ~SDR();
	void setSender ( std::string IP,  std::string port) {mIp = IP; mPort = port;};
	void setSensorId (int sensorId) {mSensorId = sensorId;};

	void initialize(long frequency, long samplingRate, int gain, long chunkSize, long overlapSize, long duration, long downSampling);
	void start();
	void stop();

private:
	rtlsdr_dev_t* mDevice;
	long mFrequency, mSamplingRate, mChunkSize, mOverlapSize, mDuration, mDownSampling;
	int mGain;
	std::string mIp, mPort;
	Sender* mSender;
	int mSensorId;
	ReaderWriterQueue<SpectrumSegment*> *mQueue;


};

#endif /* SRC_SENSOR_SDR_H_ */
