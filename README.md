# Ash - Nash Equilibria Finder
by Eugen Sawin <esawin@me73.com>

## License
The MIT License

## Requirements
* POSIX.1b-compliant operating system (librt)
* GNU GCC 4.6 or newer
* GNU Make
* Python 2.7 or newer (only for style checking)

## Dependencies
### Required
* lpsolve55 (http://lpsolve.sourceforge.net/5.5 or `$ sudo apt-get install liblpsolve55-dev`)
* gflags (http://code.google.com/p/gflags or `$ make gflags`)

### Optional
* gtest (http://code.google.com/p/googletest, only for testing)
* gperftools (http://code.google.com/p/gperftools, only for profiling)
* cpplint (`$ make cpplint`, only for style checking)

## Building gflags
The repository contains a slightly modified gflags version with less verbose
help output.  
If you want to use the provided version instead, you need to build gflags
locally:

    $ make gflags

and then activate the two lines in the makefile, which are commented out.

Alternatively you can build all dependencies at once:

    $ make depend

## Building Ash (depends on gflags)
To build Ash use:

    $ make

For performance measuring use the more optimised version:

    $ make opt

## Using Ash
Just use:

    $ ash game.nfg

To show the full usage and flags help use:

    $ ash -help

## Running Ash performance tests
To run performance tests on some benchmark problems use:

    $ make perftest

All test parameters can be changed on invocation like this:

    $ make perftest LOG=log/perftest.txt

## Testing Ash (depends on gtest)
To build and run the unit tests use

    $ make check

## Profiling Ash (depends on gperftools)
To build Ash with profiling turned on use:

    $ make profile

## Getting cpplint
Code style checking depends on a modified version of Google's cpplint.  
Get it via
  
    $ make cpplint

## Checking style (depends on cpplint)
To test code style conformance with the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) use:

    $ make checkstyle

