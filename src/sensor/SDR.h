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
