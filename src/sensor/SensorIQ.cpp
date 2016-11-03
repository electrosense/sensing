#include "../ESenseInterfaces/sync.h"
#include "../ESenseInterfaces/spectrum.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "SDR.h"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;


Ice::CommunicatorPtr ic;

namespace Electrosense
{


class SynchronizationI: virtual public Electrosense::Synchronization
{
public:

	SynchronizationI(IceUtil::Mutex* mutex, std::string IP, std::string port, int sensorId)
{
		std::cout << "Synchronization Interface listening ... " << std::endl;
		mMutex = mutex;
		mMutex->lock();
		mIp = IP;
		mPort = port;
		mSensorId=sensorId;
}

	void getMutex()
	{
		mMutex->lock();
	}

	virtual void start (const Electrosense::TimePtr &reference,
			const Electrosense::TimePtr &delay,
			const Electrosense::ScanningParametersPtr	 &parameters,
			const Ice::Current& current)
	{
		// Get actual time
		/*
			struct timespec refTime;
			refTime.tv_sec = reference->sec;
			refTime.tv_nsec = reference->nsec;

			struct timespec delayTime;
			delayTime.tv_sec = delay->sec;
			delayTime.tv_nsec = delay->nsec;
		 */
		//struct timespec currentTime, res, finalDelay;
		//res = boost::detail::timespec_plus(refTime, delayTime);
		//finalDelay = boost::detail::timespec_minus(refTime,currentTime);


		CustomSleep* c = new CustomSleep(reference,delay,parameters,mMutex,mIp,mPort,mSensorId);
		c->start();
	}

private:

	IceUtil::Mutex* mMutex;
	std::string mIp, mPort;
	int mSensorId;
	class CustomSleep: public IceUtil::Thread{

	public:
		CustomSleep(const Electrosense::TimePtr &reference,
				const Electrosense::TimePtr &delay,
				const Electrosense::ScanningParametersPtr &parameters,
				IceUtil::Mutex* mutex,
				std::string IP, std::string port, int sensorId){

			mMutex = mutex;

			mReference = reference;
			mDelay = delay;
			mParameters = parameters;

			mRefTime.tv_sec = reference->sec;
			mRefTime.tv_nsec = reference->nsec;

			mIp = IP;
			mPort = port;
			mSDR = new SDR();
			mSDR->setSender(IP,port);
			mSDR->setSensorId(sensorId);
		}

		~CustomSleep()
		{
			if (mSDR)
			{
				mSDR->stop();
				delete(mSDR);
			}
		}

		virtual void run()
		{

			// Initialize dongle and wait to wake up
			mSDR->initialize(mParameters->frequency, mParameters->samplingRate,
					mParameters->chunkSize, mParameters->overlapSize, mParameters->duration, mParameters->downSample);

			// Sleep and wait
			struct timespec currentTime;
			clock_gettime(CLOCK_REALTIME, &currentTime);
			std::cout << "CurrentTime: " << currentTime.tv_sec << "." << currentTime.tv_nsec << std::endl;
			std::cout << "ReferenceTime: " << mReference->sec << "." << mReference->nsec << std::endl;
			std::cout << "Sleeping ..." << std::endl;
			clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &mRefTime, NULL);

			clock_gettime(CLOCK_REALTIME, &currentTime);
			std::cout << "* WakeUp: " << currentTime.tv_sec << "." << currentTime.tv_nsec << std::endl;

			//mMutex->unlock();

			// Start to scan
			mSDR->start();
			//ic->shutdown();
		}
	private:
		std::string mIp;
		std::string mPort;
		IceUtil::Mutex* mMutex;
		struct timespec mRefTime;
		Electrosense::TimePtr mReference;
		Electrosense::TimePtr mDelay;
		Electrosense::ScanningParametersPtr mParameters;
		SDR* mSDR;
	};
};

}


int main( const int argc, char * const argv[])
{

	std::string collector, end_point;
	int sensorId;
	po::options_description desc("Allowed options");
	desc.add_options()
						("help", "produce help message")


						("end_point,e", po::value<std::string>(&end_point)->required(),
								"IP and port to attach this app (IP:port)")
						("collector,c", po::value<std::string>(&collector)->required(),
								"IP and port of the collector (IP:port)")
						("id,i", po::value<int>(&sensorId)->required(),
								"Sensor id to identify the sensor in the collector.")
						;

	if (argc == 2) {

		std::cout << "Usage: SensorIQ [options]\n";
		std::cout << desc;
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

	std::vector<std::string> collector_address;
	boost::split(collector_address, collector, boost::is_any_of(":"), boost::token_compress_on);
	if (collector_address.size() != 2)
	{
		std::cerr << "Collector information expected is wrong: " << collector << std::endl;
		exit(-1);
	}

	std::vector<std::string> endpoint_address;
	boost::split(endpoint_address, end_point, boost::is_any_of(":"), boost::token_compress_on);
	if (endpoint_address.size() != 2)
	{
		std::cerr << "Collector information expected is wrong: " << end_point << std::endl;
		exit(-1);
	}


	Electrosense::SynchronizationI *syncPtr;

	IceUtil::Mutex mutex;

	try {
		int i = 0;
		char **c = NULL;
		ic = Ice::initialize(i, c);
		Ice::ObjectAdapterPtr adapter =
				ic->createObjectAdapterWithEndpoints("SyncAdapter", "default -h " + endpoint_address[0] + " -p " + endpoint_address[1]);
		syncPtr = new Electrosense::SynchronizationI(&mutex, collector_address[0], collector_address[1], sensorId);
		adapter->add(syncPtr, ic->stringToIdentity("Sync"));
		adapter->activate();


	} catch (const Ice::Exception& e) {
		std::cerr << e << std::endl;
	} catch (const char* msg) {
		std::cerr << msg << std::endl;
	}


	ic->waitForShutdown();
	return 0;

}


