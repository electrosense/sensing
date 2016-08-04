/*
 * SDR.cpp
 *
 *  Created on: Aug 4, 2016
 *      Author: rocapal
 */

#include "SDR.h"

SDR::SDR() {

}

void SDR::initialize(long frequency, long samplingRate, long chunkSize, long overlapSize)
{
	mFrequency = frequency;
	mSamplingRate = samplingRate;
	mChunkSize = chunkSize;
	mOverlapSize = overlapSize;

	int device_index=0;
	int n_rtlsdr=rtlsdr_get_device_count();
	if (n_rtlsdr==0) {
		std::cerr << "Error: no RTL-SDR USB devices found..." << std::endl;
	}

	// Choose which device to use
	if ((n_rtlsdr==1)&&(device_index==-1)) {
		device_index=0;
	}
	if ((device_index<0)||(device_index>=n_rtlsdr)) {
		std::cerr << "Error: must specify which USB device to use with --device-index" << std::endl;
		std::cerr << "Found the following USB devices:" << std::endl;
		char vendor[256],product[256],serial[256];
		for (int t=0;t<n_rtlsdr;t++) {
			rtlsdr_get_device_usb_strings(t,vendor,product,serial);
			std::cerr << "Device index " << t << ": [Vendor: " << vendor << "] [Product: " << product << "] [Serial#: " << serial << "]" << std::endl;
		}
	}


	// Open
	if (rtlsdr_open(&mDevice,device_index)<0) {
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

	std::cout << "Initializing dongle with following configuration: " << std::endl;
	std::cout << "\t Center Frequency: " << frequency << " Hz" << std::endl;
	std::cout << "\t Sampling Rate: " << samplingRate << " samples/sec" << std::endl;

}

typedef struct {
  std::vector <unsigned char> * buf;
  rtlsdr_dev_t * dev;
  long chunk_size;
  long overlap_size;
  long* samples_read;
  struct timespec init_time;
} callback_package_t;


static void capbuf_rtlsdr_callback(
  unsigned char * buf,
  uint32_t len,
  void * ctx
) {
	struct timespec current_time;
	clock_gettime(CLOCK_REALTIME, &current_time);

	callback_package_t * cp_p=(callback_package_t *)ctx;
	callback_package_t & cp=*cp_p;
	rtlsdr_dev_t * dev=cp.dev;
	std::vector <unsigned char> capbuf_raw_p= *cp.buf;


	if ((current_time.tv_sec - cp.init_time.tv_sec) > 2)
	{
		rtlsdr_cancel_async(dev);
		return;
	}

	// Maximum len is 1024*256 = 262144

	for (uint32_t t=0;t<len;t++) {
		if (capbuf_raw_p.size() < (unsigned int)cp.chunk_size) {
			capbuf_raw_p.push_back(buf[t]);
		}

	}

	std::cout << current_time.tv_sec << ": Segment of " << capbuf_raw_p.size() << " IQ samples " << std::endl;
	capbuf_raw_p.clear();
}

void SDR::start()
{
	std::vector <unsigned char> capbuf_raw;
	capbuf_raw.reserve(256*1024);
	long samples_read = 0;

	callback_package_t cp;
	cp.buf=&capbuf_raw;
	cp.chunk_size = mChunkSize;
	cp.overlap_size = mOverlapSize;
	cp.samples_read = &samples_read;
	cp.dev = mDevice;

	struct timespec init_time;
	clock_gettime(CLOCK_REALTIME, &init_time);
	cp.init_time = init_time;

	rtlsdr_read_async(mDevice,capbuf_rtlsdr_callback,(void *)&cp,0,0);

	stop();
}

void SDR::stop()
{
	rtlsdr_close(mDevice);
	mDevice = NULL;
}

SDR::~SDR() {
	// TODO Auto-generated destructor stub
}

