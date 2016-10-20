/*
 * Saver.cpp
 *
 *  Created on: Aug 8, 2016
 *      Author: rocapal
 */

#include "Saver.h"

namespace Electrosense {

Saver::Saver(ReaderWriterQueue<Electrosense::SpectrumSegmentPtr> *queue) {

	mQueue=queue;
	mThread = new boost::thread(&Saver::run, this);
}

void Saver::run ()
{
	std::cout << "Saver::run thread" << std::endl;
	while(1)
	{
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
				/*
				cvec capbuf;
				capbuf.set_size(samples.size());
				for (unsigned int t=0;t<samples.size();t++) {
					capbuf(index)=std::complex<double>((((double)samples[(t<<1)])-127.0)/128.0,(((double)samples[(t<<1)+1])-127.0)/128.0);

				}
				*/
				cvec capbuf;
				capbuf.set_size(samples.size()/2);
				for (unsigned int t=0;t<samples.size();t=t+2) {
					capbuf(t/2)=std::complex<double>((((double)samples[t])-127.0)/128.0,(((double)samples[(t+1)])-127.0)/128.0);
				}

				std::stringstream filename;
				filename.str("");
				filename << "/tmp" << "/capbuf_" << segment->sensorID << "_" << segment->timestamp->sec << "." << segment->timestamp->nsec << ".it";

				// Save file
				it_file itf(filename.str(),true);
				itf << Name("capbuf") << capbuf;
				ivec fc_v(1);
				fc_v(0)=segment->centerFrequency;
				itf << Name("fc") << fc_v;
				itf.close();
				capbuf.clear();
			}

		}
	}

}
Saver::~Saver() {

	if (mThread)
		delete(mThread);
}

} /* namespace Electrosense */
