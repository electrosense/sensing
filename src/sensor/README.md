sensorIQ
=

*sensorIQ* is a piece of software that tipically it runs in RaspberryPi boards and it commands the RF receiver (rtl-sdr device). *sensorIQ* waits for the controller to know the scanning configuration for a specific campaign and then, send the spectrum information to the collector.


Usage
=

```
./SensorIQ --help
Usage: SensorIQ [options]
Allowed options:
   --help                   produce help message
   -e [ --end_point ] arg   IP and port to attach this app (IP:port)
   -c [ --collector ] arg   IP and port of the collector (IP:port)
   -i [ --id ] arg          Sensor id to identify the sensor in the collector.
```
