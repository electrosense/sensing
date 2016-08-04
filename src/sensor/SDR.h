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

class SDR {
public:
	SDR();
	virtual ~SDR();

	void initialize(long frequency, long samplingRate, long chunkSize, long overlapSize);
	void start();
	void stop();

private:
	rtlsdr_dev_t* mDevice;
	long mFrequency, mSamplingRate, mChunkSize, mOverlapSize;

};

#endif /* SRC_SENSOR_SDR_H_ */
