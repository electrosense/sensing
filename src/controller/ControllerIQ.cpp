#include "../ESenseInterfaces/sync.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace boost;
using namespace std;
namespace po = boost::program_options;

const string VERSION="1.0";

void build_proxys (std::vector<std::string> list, std::vector<std::string>* proxys)
{

	for (unsigned int i=0; i<list.size(); i++)
	{
		std::vector<std::string> info;

		boost::split(info, list[i], boost::is_any_of(":"), boost::token_compress_on);

		if (info.size() != 2)
		{
			std::cerr << "Error to process this proxy: " << list[i] << std::endl;
		}
		else
		{
			proxys->push_back( "Sync:default -h " + info[0] + " -p " + info[1] );
		}

	}
}

int main(int argc, char** argv){

	// Parser

	int opt;
	int chunk_size;
	int overlap_size;
	int duration;
	int delay;
	int low_pass;
	std::vector<std::string> sensorList;

	po::options_description desc("Allowed options");
	desc.add_options()
	            		("help", "produce help message")
						("sensor-list,l", po::value< vector<string> >(&sensorList)->multitoken(), "Sensor list config in the format IP:port IP:port ....")
						("center_frequency,f", po::value<int>(&opt)->required(),
								"frequency(Hz) to center the scanning")
						("sampling_rate,s", po::value<int>(&opt)->required(),
								"samping rate (Hz) to scan")
						("low_pass,p", po::value<int>(&low_pass)->default_value(-1),
											"low pass filter in (Hz) to reduce the bandwidth")
						("chunk_size,c", po::value<int>(&chunk_size)->default_value(262144),
								"Size of chunk to scan (IQ samples)")
						("overlap_size,o", po::value<int>(&overlap_size)->default_value(0),
								"Size to overlap (IQ samples) among sensors")

						("delay,e", po::value<int>(&delay)->default_value(5),
						   			  "Delay with respect absolute time to start the scanning process")
						("duration,d", po::value<int>(&duration)->default_value(0),
								"Duration in seconds to scan spectrum")
						;


	if (argc == 2) {

		cout << "Usage: ControllerIQ [options]\n";
		cout << desc;
		return 0;
	}

	po::positional_options_description p;
	po::variables_map vm;
	try
	{
		p.add("input-file", -1);


		po::store(po::command_line_parser(argc, argv).
				options(desc).positional(p).run(), vm);
		po::notify(vm);
	}
	catch (po::error const& e) {
		std::cerr << e.what() << '\n';
		exit( EXIT_FAILURE );
	}

	int frequency = vm["center_frequency"].as<int>();
	int sampling_rate = vm["sampling_rate"].as<int>();

	if (vm.count("low_pass"))
		low_pass = vm["low_pass"].as<int>();

	if (vm.count("chunk_size"))
		chunk_size = vm["chunk_size"].as<int>();

	if (vm.count("overlap_size"))
		overlap_size = vm["overlap_size"].as<int>();

	if (vm.count("duration"))
		duration = vm["duration"].as<int>();

	if (vm.count("delay"))
		delay = vm["delay"].as<int>();

	std::cout << "ControllerIQ " << "v" << VERSION << std::endl;
	std::cout << std::endl;
	std::cout << "\t - Frequency: " << frequency << std::endl;
	std::cout << "\t - SamplingRate: " << sampling_rate << std::endl;
	std::cout << "\t - LowPass: " << low_pass << std::endl;
	std::cout << "\t - ChunkSize: " << chunk_size << std::endl;
	std::cout << "\t - OverlapSize: " << overlap_size << std::endl;
	std::cout << "\t - Delay: " << delay << std::endl;
	std::cout << "\t - Duration: " << duration << std::endl;

	std::vector<std::string> proxies;
	build_proxys(sensorList, &proxies);

	std::cout << "\t - Proxys set: " << std::endl;
	for (unsigned int i=0; i<proxies.size(); i++)
	{
		std::cout << "\t\t - " << proxies[i] << std::endl;
	}

	Ice::CommunicatorPtr ic;

	//	const char* proxy[] = {"Sync:default -h 172.16.1.148 -p 10000",
     /*   const char* proxy[] =  {"Sync:default -h 0.0.0.0 -p 10000",
				"Sync:default -h 172.16.18.22 -p 10000",
				"Sync:default -h 172.16.18.22 -p 10000",
				"Sync:default -h 172.16.18.23 -p 10000",
				"Sync:default -h 172.16.18.24 -p 10000"};
*/
	//const char* proxy[] = {"Sync:default -h 0.0.0.0 -p 10000"};
	//int n_proxies = 1;
	//std::vector<std::string> proxyStr(proxy, proxy + n_proxies);

	std::vector<Electrosense::SynchronizationPrx> syncProxies;

	try{
		Ice::PropertiesPtr props = Ice::createProperties();
		props->setProperty("Ice.Default.EncodingVersion", "1.0");
		Ice::InitializationData id;
		id.properties = props;
		ic = Ice::initialize(id);

		// Create the proxies
		for (unsigned int i=0; i<proxies.size(); i++)
		{
			Ice::ObjectPrx base = ic->stringToProxy(proxies[i]);
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
		absTime->sec = currentTime.tv_sec;
		absTime->nsec = currentTime.tv_nsec;

		// Scanning Parameters
		Electrosense::ScanningParametersPtr config = new Electrosense::ScanningParameters();
		config->frequency = frequency;
		config->samplingRate = sampling_rate;
		config->chunkSize = chunk_size;
		config->overlapSize = overlap_size;
		config->duration = duration;
		config->downSample = low_pass;

		for (unsigned int i=0; i<proxies.size(); i++)
		{
			std::cout << "config->downSample: " << config->downSample << std::endl;
			absTime->sec = currentTime.tv_sec + delay;
			std::cout << "[" << i << "] Sending RefTime: " << absTime->sec << "." << absTime->nsec << std::endl;
			syncProxies.at(i)->start(absTime, delayTime, config);

		}


	}catch (const Ice::Exception& ex) {
		std::cerr << ex << std::endl;

	} catch (const char* msg) {
		std::cerr << msg << std::endl;

	}

	sleep(2);
	ic->shutdown();

	return 0;



}

