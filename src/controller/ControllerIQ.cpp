#include "../ESenseInterfaces/sync.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>


int main(int argc, char** argv){

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
			Ice::ObjectPrx base = ic->stringToProxy(proxyStr.at(i)); //ic->stringToProxy("Sync:default -h 0.0.0.0 -p 10000");
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
		config->frequency = 906e6;
		config->samplingRate = 1.92e6;
		config->chunkSize = 9600;
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

