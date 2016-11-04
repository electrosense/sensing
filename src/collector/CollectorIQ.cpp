#include "../ESenseInterfaces/sync.h"
#include "../ESenseInterfaces/spectrum.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "readerwriterqueue.h"
#include "Saver.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace moodycamel;

//TODO: Add this feature using input parameters

namespace Electrosense
{


class SpectrumI: virtual public Electrosense::Spectrum
{
public:

	SpectrumI(bool singleFile)
	{
		mQueue = new ReaderWriterQueue< Electrosense::SpectrumSegmentPtr> (10);
		mSaver = new Saver(mQueue, singleFile);
	}

	virtual void push(const Electrosense::SpectrumSegmentPtr& segment, const Ice::Current& current)
	{
		//std::cout << "Receiving " << segment->timestamp->sec << "." << segment->timestamp->nsec << " - CompressSize: " << segment->samplesSize << " - Vector Size: " << segment->samples.size() <<  std::endl;

		mQueue->enqueue(segment);

	}

	virtual void finish()
	{
		if (mSaver)
			mSaver->finish();
	}

	~SpectrumI()
	{
		if (mSaver)
			delete(mSaver);
	}

private:
	ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *mQueue;

	Saver* mSaver;
};

}

Electrosense::SpectrumI *spectrumPtr;
Ice::CommunicatorPtr ic;

void terminate_handler(int s){

	if (spectrumPtr)
		spectrumPtr->finish();

	delete(spectrumPtr);
	ic->shutdown();

	fprintf(stderr, "Terminated.\n");
}

int main(int argc, char** argv)
{
	int port = 10001;
	bool single_file = false;

	// Parser

	int opt;
	bool sfile;
	po::options_description desc("Allowed options");
	desc.add_options()
						("help", "produce help message")
						("port,p", po::value<int>(&opt)->default_value(10001),
								"port to listen Spectrum interface")
						("single-file,s",
								"Save the spectrum data in a single file ")
						;

	po::positional_options_description p;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << "Usage: CollectorIQ [options]\n";
		std::cout << desc;
		return 0;
	}

	if (vm.count("port"))
		port = vm["port"].as<int>();

	if (vm.count("single-file"))
		single_file = true;


	Ice::ObjectAdapterPtr adapter;

	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = terminate_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);


	try {
		int i = 0;
		char **c = NULL;
		ic = Ice::initialize(i, c);
		adapter = ic->createObjectAdapterWithEndpoints("Spectrum", "default -h 0.0.0.0 -p " + std::to_string(port));
		spectrumPtr = new Electrosense::SpectrumI(single_file);
		adapter->add(spectrumPtr, ic->stringToIdentity("Spectrum"));
		adapter->activate();

		ic->waitForShutdown();

	} catch (const Ice::Exception& e) {
		std::cerr << e << std::endl;
	} catch (const char* msg) {
		std::cerr << msg << std::endl;
	}



	if (ic) {
		try {
			ic->destroy();
		} catch (const Ice::Exception& e) {
			std::cout << e << std::endl;
		}
	}

	return 0;

}
