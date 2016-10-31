/*
 * Saver.h
 *
 *  Created on: Aug 8, 2016
 *      Author: rocapal
 */

#ifndef SRC_COLLECTOR_SAVER_H_
#define SRC_COLLECTOR_SAVER_H_

#include <boost/thread.hpp>
#include <itpp/itbase.h>
#include <sstream>
#include <zlib.h>

#include "../ESenseInterfaces/sync.h"
#include "../ESenseInterfaces/spectrum.h"
#include "readerwriterqueue.h"

using namespace moodycamel;
using namespace itpp;

namespace Electrosense {

class Saver {
public:
	Saver(ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *queue, bool uniqueFile);
	virtual ~Saver();

	void finish();

private:

	void run ();
	void stop();

	ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *mQueue;
	boost::thread *mThread;

	std::stringstream mFileName;
	it_file mItFile;
	bool mUniqueFile;
	bool mStop;
	ivec m_fc_v;
	cvec mFinalBuffer;
};

} /* namespace Electrosense */

#endif /* SRC_COLLECTOR_SAVER_H_ */
