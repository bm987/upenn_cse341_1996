A Compiler Project for Translating a C Subset to SPARC Assembly Language
------------------------------------------------------------------------

My compiler project from 20 years ago!  Too bad we didn't have LLVM back then...

See http://repository.upenn.edu/cis_reports/289/ (included here as docs/compiler_project_paper.pdf) for more information.

Compiling and Running on Linux
------------------------------

On a modern Linux distribution, you just need gcc, make, flex-devel, bison-devel. To make and compile the sample quicksort program in "sample1":

   make clean
   make
   cd sample1
   ../mcc fquick.c
   cat fquick.s

Of course, it generates SPARC assembly... so it won't be very useful on an x86 CPU.  Maybe it wouldn't need too many changes to work on a RISC CPU like an ARM.

