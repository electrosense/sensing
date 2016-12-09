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
