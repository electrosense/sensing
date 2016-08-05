/*
 * Sender.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: rocapal
 */

#include "Sender.h"


Sender::Sender (ReaderWriterQueue< SpectrumSegment*>*queue)
{
	std::cout << "[*] Initializing sender" << std::endl;

	Ice::CommunicatorPtr ic;
	Ice::PropertiesPtr props = Ice::createProperties();
	props->setProperty("Ice.Default.EncodingVersion", "1.0");
	Ice::InitializationData id;
	id.properties = props;

	ic = Ice::initialize(id);

	Ice::ObjectPrx base = ic->stringToProxy("Spectrum:default -h 0.0.0.0 -p 10001");
	if (0==base)
	{
		throw "Invalid base";
	}


	mSpectrumManager = Electrosense::SpectrumPrx::checkedCast(base);
	if (!mSpectrumManager)
		throw "Invalid proxy";



	mQueue = queue;
	mThread = new boost::thread(&Sender::run, this);

}

void Sender::run ()
{
	std::cout << "Sender::run thread" << std::endl;

	while(1)
	{
		usleep(5);
		SpectrumSegment* segment = new SpectrumSegment();
		if (mQueue->try_dequeue(segment)) {
			std::cout << "	- Received: " << segment->getSensorId() << " - Time: " << segment->getTimeStamp().tv_sec
					<< "." << segment->getTimeStamp().tv_nsec << std::endl;

			// Send
			Electrosense::SpectrumSegmentPtr eSegment = new Electrosense::SpectrumSegment();
			eSegment->sensorID = segment->getSensorId();
			Electrosense::TimePtr time = new Electrosense::Time();
			time->sec = segment->getTimeStamp().tv_sec;
			time->nsec = segment->getTimeStamp().tv_nsec;
			eSegment->timestamp = time;
			eSegment->centerFrequency = segment->getCenterFrequency();
			eSegment->samplingRate = segment->getSamplingRate();
			eSegment->samples = segment->getSamples();

			mSpectrumManager->push(eSegment);

			// free memory
			segment->getSamples().clear();
			delete(segment);
		}

	}
}

Sender::~Sender() {

	if (mThread)
		delete(mThread);

}

void Sender::wait()
{
	mThread->join();
}
