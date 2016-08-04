#include "../ESenseInterfaces/sync.h"
#include "../ESenseInterfaces/spectrum.h"

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "SDR.h"




Ice::CommunicatorPtr ic;

namespace Electrosense
{


class SynchronizationI: virtual public Electrosense::Synchronization
{
public:

	SynchronizationI(IceUtil::Mutex* mutex)
{
		std::cout << "Synchronization Interface listening ... " << std::endl;
		mMutex = mutex;
		mMutex->lock();
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


		CustomSleep* c = new CustomSleep(reference,delay,parameters,mMutex);
		c->start();
	}

private:

	IceUtil::Mutex* mMutex;
	class CustomSleep: public IceUtil::Thread{

	public:
		CustomSleep(const Electrosense::TimePtr &reference,
				const Electrosense::TimePtr &delay,
				const Electrosense::ScanningParametersPtr &parameters,
				IceUtil::Mutex* mutex){

			mMutex = mutex;

			mReference = reference;
			mDelay = delay;
			mParameters = parameters;

			mRefTime.tv_sec = reference->sec;
			mRefTime.tv_nsec = reference->nsec;

			mSDR = new SDR();
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
			mSDR->initialize(mParameters->frequency, mParameters->samplingRate, mParameters->chunkSize, mParameters->overlapSize);

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
			ic->shutdown();
		}
	private:
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

	Electrosense::SynchronizationI *syncPtr;


	IceUtil::Mutex mutex;

	try {
		int i = 0;
		char **c = NULL;
		ic = Ice::initialize(i, c);
		Ice::ObjectAdapterPtr adapter =
				ic->createObjectAdapterWithEndpoints("SyncAdapter", "default -h 0.0.0.0 -p 10000");
		syncPtr = new Electrosense::SynchronizationI(&mutex);
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


