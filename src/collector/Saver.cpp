/*
 * Saver.cpp
 *
 *  Created on: Aug 8, 2016
 *      Author: rocapal
 */

#include "Saver.h"

namespace Electrosense {

Saver::Saver(ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *queue, bool uniqueFile) {

	mQueue=queue;
	mUniqueFile = uniqueFile;
	mFileName.str("");
	mStop = false;
	m_fc_v.set_size(1);

	mThread = new boost::thread(&Saver::run, this);

}

void Saver::stop()
{
	mStop = true;
}
void Saver::run ()
{
	std::cout << "Saver::run thread" << std::endl;
	while(1)
	{
		if (mStop)
		{
			std::cout << "Exit the thread" << std::endl;
			break;
		}

		usleep(2);
		Electrosense::SpectrumSegmentPtr segment;

		if (mQueue->try_dequeue(segment)) {

			std::cout << "Receiving " << segment->timestamp->sec << "." << segment->timestamp->nsec << " - OriginalSize: "
							<< segment->samplesSize << " - Vector Size: " << segment->samples.size() <<  std::endl;

			//Uncompress
			size_t dest_len = segment->samplesSize;
			size_t source_len = segment->samples.size();
			unsigned char* origin_buf = (unsigned char*) malloc(dest_len);

			int r = uncompress((Bytef *) origin_buf, (uLongf *) &dest_len, (const Bytef *) (segment->samples.data()), (uLong)source_len);
			if(r != Z_OK) {
				std::cerr << "Error while uncompressed data" << std::endl;
			}
			else
			{
				std::vector<unsigned char> samples;
				samples.assign(origin_buf,origin_buf+dest_len);


				//Normalize
				cvec capbuf;
				capbuf.set_size(samples.size()/2);
				// TODO: Be sure that size() is a even
				for (unsigned int t=0;t<samples.size()-1;t=t+2) {
					capbuf(t/2)=std::complex<double>((((double)samples[t])-127.0)/128.0,(((double)samples[(t+1)])-127.0)/128.0);
					//capbuf(t/2)=std::complex<double>( (double)samples[t] , (double)samples[(t+1)]);
					//if (t==20)
					//	std::cout << capbuf(t/2) << std::endl;
				}

				if (mUniqueFile)
				{
					if (mFileName.str().empty())
					{
						mFileName << "/tmp" << "/capbuf_" << segment->sensorID << "_" << segment->timestamp->sec << "." << segment->timestamp->nsec << ".it";
						// Save file
						mItFile.open(mFileName.str(),true);
						//mFinalBuffer = concat(mFinalBuffer, capbuf);

						mItFile << Name("capbuf") << capbuf;
						mItFile.close();
						capbuf.clear();
					}
					else
					{
						// We save data in memory until the end.
						m_fc_v(0)=segment->centerFrequency;

						mFinalBuffer.clear();
						mItFile.open(mFileName.str(), false);
						mItFile >> Name("capbuf") >> mFinalBuffer;
						mFinalBuffer = concat(mFinalBuffer, capbuf);
						mItFile.close();

						mItFile.open(mFileName.str(),true);
						mItFile << Name("capbuf") << mFinalBuffer;
						mItFile << Name("fc") << m_fc_v;
						mItFile.close();

						capbuf.clear();

					}
				}
				else
				{
					mFileName.str("");
					mFileName << "/tmp" << "/capbuf_" << segment->sensorID << "_" << segment->timestamp->sec << "." << segment->timestamp->nsec << ".it";
					// Save file
					mItFile.open(mFileName.str(),true);
					mItFile << Name("capbuf") << capbuf;
					ivec fc_v(1);
					fc_v(0)=segment->centerFrequency;
					mItFile << Name("fc") << fc_v;
					mItFile.flush();

					mItFile.close();
					capbuf.clear();
				}

			}

		}
	}

}

void Saver::finish()
{
/*	if (mUniqueFile)
	{
		mItFile << Name("fc") << m_fc_v;
		mItFile.close();
	}
	*/
}

Saver::~Saver() {

	if (mThread)
	{
		stop();
		mThread->join();
		delete(mThread);
	}
}

} /* namespace Electrosense */
