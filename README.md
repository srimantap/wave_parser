# wave_parser
A basic wav file parser to display the header information. This only handles Data chunk and LIST Info Chunk header on a wav file.
This project is using meson build system. And Check library for UT.

## How to Build
To build the project, use the following command:
```
meson setup builddir
cd builddir
meson compile
meson install
```
To run the unit test
```
meson test
```

## Help Text
To get the help text 
```
./wave_parser -h
```

## Debug Info
There are 5 levels of debug information you can get. The default is the log messages to display the header output from the input wav file.

## Running the program
```
./wave_parser -v2 -f <path to wave file>
