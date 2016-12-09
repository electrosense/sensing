ControllerIQ
=

*ControllerIQ* is responsible to manage a set of sensors to enable the collaborative spectrum strategy. It commands sensing configuration to any sensor envolved in the campaign.


Usage
=

```
$ ./ControllerIQ --help
Usage: ControllerIQ [options]
Allowed options:
  --help                            produce help message
  -l [ --sensor-list ] arg          Sensor list config in the format IP:port 
                                    IP:port ....
  -f [ --center_frequency ] arg     frequency(Hz) to center the scanning
  -s [ --sampling_rate ] arg        samping rate (Hz) to scan
  -g [ --gain ] arg                 Gain in tenths of DB: 0(auto), 9, 125, 207,
                                    328, 402, 496
  -p [ --low_pass ] arg (=-1)       low pass filter in (Hz) to reduce the 
                                    bandwidth
  -c [ --chunk_size ] arg (=262144) Size of chunk to scan (IQ samples)
  -o [ --overlap_size ] arg (=0)    Size to overlap (IQ samples) among sensors
  -e [ --delay ] arg (=5)           Delay with respect absolute time to start 
                                    the scanning process
  -d [ --duration ] arg (=0)        Duration in seconds to scan spectrum

```
