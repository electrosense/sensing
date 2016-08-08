/*
 * Sender.h
 *
 *  Created on: Aug 5, 2016
 *      Author: rocapal
 */

#ifndef SRC_SENSOR_SENDER_H_
#define SRC_SENSOR_SENDER_H_

#include <boost/thread.hpp>
#include "readerwriterqueue.h"
#include "SpectrumSegment.h"

#include <zlib.h>

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "../ESenseInterfaces/spectrum.h"


using namespace moodycamel;

class Sender {
public:
	Sender(ReaderWriterQueue< SpectrumSegment*>*queue);
	virtual ~Sender();

	void wait();

private:

	void run();

	Electrosense::SpectrumPrx mSpectrumManager;
	ReaderWriterQueue< SpectrumSegment*> *mQueue;
	boost::thread *mThread;
};

#endif /* SRC_SENSOR_SENDER_H_ */
