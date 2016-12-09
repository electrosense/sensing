Sensing
=

*sensing* is the open source implementation part of the proposals done by R. Calvo-Palomino, D. Giustiniano, V. Lenders and A. Fakhreddine in **"Crowdsourcing Spectrum Data Decoding"** [[INFOCOM 2016]]


Software
=

The software contains three main components that make possible the collaborative spectrum scanning:

* *sensor:* This software runs in the sensing nodes (typically in RBPi boards). It receives the scanning configuration (frequency, bandwidth, sampling rate, chunk size, overlap size, etc) from the controller.

* *controller*: This software is responsible to command the sensing configuration to the sensors and the time sensing scheduling.

* *collector:* This software runs in the backend and it storages all the sensing data comming from different sensing nodes.



To get more details about them you can check the specific folder that you find in src/ folder

Installation
=

```
$ git clone http://gitlab.com/electrosense/sensing/
$ cd sensing/
$ cmake .
$ make
```

To compile this software you will need the following software installed:

* itpp
* fftw
* rtl-sdr
* curses
* ice
* boost
* zlib
