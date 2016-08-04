/*
 * SDR.cpp
 *
 *  Created on: Aug 4, 2016
 *      Author: rocapal
 */

#include "SDR.h"

SDR::SDR() {
	// TODO Auto-generated constructor stub

}

void SDR::initialize(long frequency, long samplingRate)
{
	// Open
	if (rtlsdr_open(&mDevice,0)<0) {
		std::cerr << "Error: unable to open RTLSDR device" << std::endl;
	}

	// Sampling frequency
	if (rtlsdr_set_sample_rate(mDevice,samplingRate)<0) {
		std::cerr << "Error: unable to set sampling rate" << std::endl;
	}

	// Reset the buffer
	if (rtlsdr_reset_buffer(mDevice)<0) {
		std::cerr << "Error: unable to reset RTLSDR buffer" << std::endl;
	}

}
void SDR::start()
{

}

void SDR::stop()
{

}

SDR::~SDR() {
	// TODO Auto-generated destructor stub
}

