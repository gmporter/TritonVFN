TritonVFN
=========

An easy-to-use wrapper library around the Myricom Kernel-bypass (SNF) functionality

Dependencies:
 - cmake
 - libgtest-dev
 - libgflags-dev

To build the code:
  * $ mkdir Release
  * $ cd Release
  * build $ cmake -DCMAKE_BUILD_TYPE=Release ../src
  * build $ ..
  * $ mkdir Debug
  * $ cd Debug
  * build $ cmake -DCMAKE_BUILD_TYPE=Debug ../src
  * build $ ..
  * build $ make -j
  * $ applications/my_app/my_app --help
