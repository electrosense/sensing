CollectorIQ
=

*CollectorIQ* usually runs in the backend and it storages the sensing spectrum data sent by the sensors. Each sensor is identified by the id set in the sensing command. The data storage can be saved in one unique file or different files. The structure of the files is ITPP format with two variables: 'freq' for the frequency and 'capbuf' for the IQ samples. 


Usage
=

```
$ ./CollectorIQ --help
Usage: CollectorIQ [options] 
Allowed options:
  --help                     produce help message
  -p [ --port ] arg (=10001) port to listen Spectrum interface
  -s [ --single-file ]       Save the spectrum data in a single file 
```
