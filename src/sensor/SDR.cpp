/*
 * SDR.cpp
 *
 *  Created on: Aug 4, 2016
 *      Author: rocapal
 */

#include "SDR.h"

SDR::SDR() {

}

void SDR::initialize(long frequency, long samplingRate, long chunkSize, long overlapSize, long duration, long downSampling)
{
	mFrequency = frequency;
	mSamplingRate = samplingRate;
	mChunkSize = chunkSize;
	mOverlapSize = overlapSize;
	mDuration = duration;
	mDownSampling = downSampling;

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
		std::cerr << "Error: unable to set sampling rate to " << samplingRate << std::endl;
	}

	if (rtlsdr_set_center_freq(mDevice,frequency)<0)
	{
		std::cerr << "Error: unable to set frequency to" << frequency << std::endl;
	}

	// Reset the buffer
	if (rtlsdr_reset_buffer(mDevice)<0) {
		std::cerr << "Error: unable to reset RTLSDR buffer" << std::endl;
	}

	std::cout << "[*] Initializing dongle with following configuration: " << std::endl;
	std::cout << "\t Center Frequency: " << frequency << " Hz" << std::endl;
	std::cout << "\t Sampling Rate: " << samplingRate << " samples/sec" << std::endl;


	mQueue = new ReaderWriterQueue< SpectrumSegment*> (10);


}

typedef struct {
  std::vector <unsigned char> * buf;
  rtlsdr_dev_t * dev;
  long center_frequency;
  long sampling_rate;
  long down_sampling;
  long chunk_size;
  long overlap_size;
  long duration;
  long* samples_read;
  struct timespec init_time;
  int sensor_id;
  int control_flow;
  ReaderWriterQueue< SpectrumSegment*>* queue;
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
	ReaderWriterQueue< SpectrumSegment*> *queue = cp.queue;

	if (cp.control_flow == 8)
	{
		cp.control_flow = 0;
		return;
	}

	if ((current_time.tv_sec - cp.init_time.tv_sec) > cp.duration)
	{
		rtlsdr_cancel_async(dev);
		return;
	}

	// Maximum len is 1024*256 = 262144

	//TODO: Check if we can do this operation as atomic one.

	unsigned int factor = 0;

	if (cp.down_sampling != -1)
		factor = ((cp.sampling_rate / cp.down_sampling) + 1) * 2;

	unsigned int counter = 0;
	unsigned int mycounter = 0;

	for (uint32_t t=0;t<len;t++) {

		counter=counter+1;
		if ( (cp.down_sampling != -1) && (counter < factor))
		{
			continue;
		}

		if (capbuf_raw_p.size() < (unsigned int)cp.chunk_size*2) {
			capbuf_raw_p.push_back(buf[t]);
			counter = 0;
			mycounter=mycounter+1;
		}
	}

	std::cout << "Samples saved: " << mycounter << std::endl;
	SpectrumSegment* segment = new SpectrumSegment(cp.sensor_id,current_time,cp.center_frequency, cp.sampling_rate, cp.down_sampling, capbuf_raw_p);
	queue->enqueue(segment);

	//std::cout << current_time.tv_sec << ": Segment of " << capbuf_raw_p.size() << " IQ samples - Size:" << queue->size_approx() << std::endl;
	capbuf_raw_p.clear();

	cp.control_flow = cp.control_flow+1;

}

void SDR::start()
{
	std::vector <unsigned char> capbuf_raw;
	if (mDownSampling == -1)
	{
		std::cout << "Reserving 262144 values for the buffer" << std::endl;
		capbuf_raw.reserve(256*1024);
	}
	else
	{
		unsigned int factor = ((mSamplingRate / mDownSampling) + 1) * 2;
		capbuf_raw.reserve((256*1024)/factor);

		std::cout << "Reserving " << (256*1024)/factor << " values for the buffer" << std::endl;
	}

	long samples_read = 0;

	callback_package_t cp;
	cp.buf=&capbuf_raw;
	cp.center_frequency = mFrequency;
	cp.sampling_rate = mSamplingRate;
	cp.down_sampling = mDownSampling;
	cp.chunk_size = mChunkSize;
	cp.overlap_size = mOverlapSize;
	cp.duration = mDuration;
	cp.samples_read = &samples_read;
	cp.dev = mDevice;
	cp.queue = mQueue;
	cp.sensor_id = mSensorId;
	cp.control_flow = 0;
	struct timespec init_time;
	clock_gettime(CLOCK_REALTIME, &init_time);
	cp.init_time = init_time;

	rtlsdr_read_async(mDevice,capbuf_rtlsdr_callback,(void *)&cp,0,0);

	mSender = new Sender(mQueue, mIp, mPort);
	mSender->wait();
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

