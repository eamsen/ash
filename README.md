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
  * lpsolve55 (not included)
  * gtest (not included, only required for testing)
  * gperftools (not included, only required for profiling)
  * gflags (included)
  * cpplint (included, only required for style checking)

## Building gflags
  Before building Ash, you need to build gflags locally once:
  `$ make gflags`

  Alternatively you can build all dependencies at once:
  `$ make depend`

## Building Ash (depends on gflags)
  To build Ash use:
  `$ make`

  For performance measuring use the more optimised version:
  `$ make opt`

## Running Ash performance tests
  To run performance tests on some benchmark problems use:
  `$ make perftest`

  All test parameters can be changed on invocation like this:
  `$ make perftest LOG=log/perftest.txt`

## Building gtest
  Before building the tests, you need to build gtest locally once:
  `$ make gtest`

## Testing Ash (depends on gtest)
  To build and run the unit tests use:
  `$ make test`

## Profiling Ash (depends on gperftools)
  To build Ash with profiling turned on use:
  `$ make profile`

## Checking style
  To test code style conformance with the [Google C++ Style Guide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml) use:
  `$ make checkstyle`

## Using Ash
  Just use:
  `$ ash game.nfg`

  To show the full usage and flags help use:
  `$ ash -help`
