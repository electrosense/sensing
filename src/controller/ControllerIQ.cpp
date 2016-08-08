#include "../ESenseInterfaces/sync.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <boost/program_options.hpp>

using namespace boost;
using namespace std;
namespace po = boost::program_options;

const string VERSION="1.0";

int main(int argc, char** argv){

	// Parser

	int opt;
	int portnum;
	po::options_description desc("Allowed options");
	desc.add_options()
	            		("help", "produce help message")
						("center_frequency,f", po::value<int>(&opt)->default_value(806*1e6),
								"frequency(Hz) to center the scanning")
						("sampling_rate,s", po::value<int>(&opt)->default_value(1.92*1e6),
								"samping rate (Hz) to scan")
						("chunk_size,c", po::value<int>(&opt)->default_value(10000),
								"Size of chunk to scan (IQ samples)")
						("overlap_size,o", po::value<int>(&opt)->default_value(0),
								"Size to overlap (IQ samples) among sensors")
						("duration,d", po::value<int>(&opt)->default_value(0),
								"Duration in seconds to scan spectrum")
/*
						("verbose,v", po::value<int>()->implicit_value(1),
								"enable verbosity (optionally specify level)")
						("listen,l", po::value<int>(&portnum)->implicit_value(1001)
								->default_value(0,"no"),
								"listen on a port.")
						("include-path,I", po::value< vector<string> >(),
								"include path")
								("input-file", po::value< vector<string> >(), "input file")
*/
						;


	po::positional_options_description p;
	p.add("input-file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		cout << "Usage: ControllerIQ [options]\n";
		cout << desc;
		return 0;
	}

	int frequency = vm["center_frequency"].as<int>();
	int sampling_rate = vm["sampling_rate"].as<int>();
	int chunk_size = vm["chunk_size"].as<int>();
	int overlap_size = vm["overlap_size"].as<int>();
	int duration = vm["duration"].as<int>();

	std::cout << "ControllerIQ " << "v" << VERSION << std::endl;
	std::cout << std::endl;
	std::cout << "\t - Frequency: " << frequency << std::endl;
	std::cout << "\t - SamplingRate: " << sampling_rate << std::endl;
	std::cout << "\t - ChunkSize: " << chunk_size << std::endl;
	std::cout << "\t - OverlapSize: " << overlap_size << std::endl;
	std::cout << "\t - Duration: " << duration << std::endl;

	Ice::CommunicatorPtr ic;

	//	const char* proxy[] = {"Sync:default -h 172.16.1.148 -p 10000",
        const char* proxy[] =  {"Sync:default -h 0.0.0.0 -p 10000",
				"Sync:default -h 172.16.18.22 -p 10000",
				"Sync:default -h 172.16.18.22 -p 10000",
				"Sync:default -h 172.16.18.23 -p 10000",
				"Sync:default -h 172.16.18.24 -p 10000"};

	//const char* proxy[] = {"Sync:default -h 0.0.0.0 -p 10000"};
	int n_proxies = 1;
	std::vector<std::string> proxyStr(proxy, proxy + n_proxies);

	std::vector<Electrosense::SynchronizationPrx> syncProxies;

	try{
		Ice::PropertiesPtr props = Ice::createProperties();
		props->setProperty("Ice.Default.EncodingVersion", "1.0");
		Ice::InitializationData id;
		id.properties = props;
		ic = Ice::initialize(id);

		// Create the proxies
		for (int i=0; i<n_proxies; i++)
		{
			Ice::ObjectPrx base = ic->stringToProxy(proxyStr.at(i));
			if (0==base)
			{
				 throw "Invalid base";
			}


			Electrosense::SynchronizationPrx syncManager = Electrosense::SynchronizationPrx::checkedCast(base);
			if (!syncManager)
						throw "Invalid proxy";

			syncProxies.push_back(syncManager);

		}

		// Compute the reference time to send.
		struct timespec currentTime;
		Electrosense::TimePtr absTime = new Electrosense::Time();
		Electrosense::TimePtr delayTime = new Electrosense::Time();
		delayTime->sec=0;
		delayTime->nsec=0;

		clock_gettime(CLOCK_REALTIME, &currentTime);
		absTime->sec = currentTime.tv_sec +1;
		absTime->nsec = currentTime.tv_nsec;

		// Scanning Parameters
		Electrosense::ScanningParametersPtr config = new Electrosense::ScanningParameters();
		config->frequency = 806e6;
		config->samplingRate = 1.92e6;
		config->chunkSize = 10000;
		config->overlapSize = 0;
		config->duration = 0;

		for (int i=0; i<n_proxies; i++)
		{
			absTime->sec = currentTime.tv_sec +3;
			std::cout << "[" << i << "] Sending RefTime: " << absTime->sec << "." << absTime->nsec << std::endl;
			syncProxies.at(i)->start(absTime, delayTime, config);

		}


	}catch (const Ice::Exception& ex) {
		std::cerr << ex << std::endl;

	} catch (const char* msg) {
		std::cerr << msg << std::endl;

	}

	ic->shutdown();

	return 0;



}

