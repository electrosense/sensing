#include "../ESenseInterfaces/sync.h"
#include "../ESenseInterfaces/spectrum.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "readerwriterqueue.h"
#include "Saver.h"

using namespace moodycamel;


namespace Electrosense
{


class SpectrumI: virtual public Electrosense::Spectrum
{
public:

	SpectrumI()
	{
		mQueue = new ReaderWriterQueue< Electrosense::SpectrumSegmentPtr> (10);
		mSaver = new Saver(mQueue);
	}

	virtual void push(const Electrosense::SpectrumSegmentPtr& segment, const Ice::Current& current)
	{


		//std::cout << "Receiving " << segment->timestamp->sec << "." << segment->timestamp->nsec << " - CompressSize: " << segment->samplesSize << " - Vector Size: " << segment->samples.size() <<  std::endl;


		mQueue->enqueue(segment);

	}

private:
	ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *mQueue;

	Saver* mSaver;
};

}

int main(int argc, char** argv)
{
	Ice::CommunicatorPtr ic;

	Electrosense::Spectrum *spectrumPtr;
	try {
		int i = 0;
		char **c = NULL;
		ic = Ice::initialize(i, c);
		Ice::ObjectAdapterPtr adapter =
				ic->createObjectAdapterWithEndpoints("Spectrum", "default -h 0.0.0.0 -p 10001");
		spectrumPtr = new Electrosense::SpectrumI();
		adapter->add(spectrumPtr, ic->stringToIdentity("Spectrum"));
		adapter->activate();


	} catch (const Ice::Exception& e) {
		std::cerr << e << std::endl;
	} catch (const char* msg) {
		std::cerr << msg << std::endl;
	}


	ic->waitForShutdown();
	return 0;

}
