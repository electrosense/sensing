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
	Sender(ReaderWriterQueue< SpectrumSegment*>*queue,  std::string IP,  std::string port);
	virtual ~Sender();

	void wait();

private:

	void run();

	Electrosense::SpectrumPrx mSpectrumManager;
	ReaderWriterQueue< SpectrumSegment*> *mQueue;
	boost::thread *mThread;
};

#endif /* SRC_SENSOR_SENDER_H_ */
