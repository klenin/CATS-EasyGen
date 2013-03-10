##Project Structure
<dl>
  <dt>./src/parser</dt>
  <dd>Formal description parser.</dd>
  <dt>./src/validator</dt>
  <dd>Input/output data validator.</dd>
  <dt>./src/validator</dt>
  <dd>Input/output data validator.</dd>
  <dt>./src/generator</dt>
  <dd>Input/output data generator</dd>
  <dt>./src/example</dt>
  <dd>Generator example.</dd>
  <dt>./interfaces/perl</dt>
  <dd>Perl interface to parser and validator (uses XS).</dd>
</dl>

Also see http://imcs.dvgu.ru/works/work?wid=2562 (in Russian).

##Build Requirements
### C code
    * CMake[cmake download] 2.6+
    * flex[flex download] 2.5+
### Perl interface
    * Perl
    * h2xs (comes with Perl)
    * NMake (only for Windows)

##Build Instructions
### C code
#### Unix or MSYS

    make clean
    make all
    make test

#### MinGW

    mingw32-make clean
    mingw32-make all
    mingw32-make test

### Perl interface
#### Windows

    nmake clean
    nmake all
    cd build
    nmake interface-perl
    cd ../lib/perl/CATS-FormalInput
    nmake install

[flex download]: http://flex.sourceforge.net/#downloads
[cmake download]: http://www.cmake.org/cmake/resources/software.html
