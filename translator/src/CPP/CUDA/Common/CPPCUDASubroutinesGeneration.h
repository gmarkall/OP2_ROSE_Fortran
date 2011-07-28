/*
 * Written by Adam Betts and Carlo Bertolli
 */

#ifndef CPP_CUDA_SUBROUTINES_GENERATION_H
#define CPP_CUDA_SUBROUTINES_GENERATION_H

#include <CPPSubroutinesGeneration.h>

class CPPCUDASubroutinesGeneration: public CPPSubroutinesGeneration
{
  private:

    virtual CPPHostSubroutine *
    createSubroutines (CPPParallelLoop * parallelLoop,
        std::string const & userSubroutineName);

  public:

    CPPCUDASubroutinesGeneration (SgProject * project,
        CPPProgramDeclarationsAndDefinitions * declarations);

};

#endif