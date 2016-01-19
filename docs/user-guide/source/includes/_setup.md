# Setup

## Dependencies

* GDAL
* GEOS
* Erlang
* ``rebar3``
* GNU Make

You will need to have the GDAL and GEOS libraries installed. Linux GIS packages usually provide GDAL 1.11.x, as such that is the version supported (tested) by this library. Similarly for GEOS 3.4.2. The LFE in this project was written against Erlang 18.1, though every effort was made to ensure backwards-compatibility through version 15 of Erlang. If you find otherwise, that is a bug and I'd appreciate a issue getting created for it :-)

## Installation

### Linux/BSD

> Build it with:

```bash
$ make
```

> Run tests with:

```bash
$ make check
```

With the Linux/BSD dependencies installed, "installation" is as simple as building lgeo in your local directory, per the examples to the right.


### On Windows

> Windows ENV variables so that GDAL and Erlang can be found:

```cmd
SET INCLUDE=%INCLUDE%;C:\cygwin\opt\gdal\include;C:\cygwin\opt\gdal\include\gdal
SET LIB=%LIB%;C:\cygwin\opt\gdal\lib
SET PATH=%PATH%;C:\cygwin\opt\gdal\bin;C:\erl5.9.1\bin

SET INCLUDE=%INCLUDE%;C:\cygwin\opt\geos\include
SET LIB=%LIB%;C:\cygwin\opt\geos\lib
SET PATH=%PATH%;C:\cygwin\opt\geos\bin
```

> And finally compile the whole thing:

```bash
$ make
```

You need to have GDAL installed (e.g., `C:\cygwin\opt\gdal`) as well as GEOS (e.g., `C:\cygwin\opt\geos`).

Open a shell which has all compilers and the MSVC environment set up (e.g. the
Windows SDK 7.1 Command Prompt). Then follow the examples to the right.


## Configuration

TBD
