#pragma once
#ifndef FORTRAN_CUDA_MODULE_DECLARATIONS_H
#define FORTRAN_CUDA_MODULE_DECLARATIONS_H

#include <FortranModuleDeclarations.h>

class FortranCUDAOpDatCardinalitiesDeclaration;
class FortranOpDatDimensionsDeclaration;

class FortranCUDAModuleDeclarations: public FortranModuleDeclarations
{
  protected:

    FortranCUDAOpDatCardinalitiesDeclaration * dataSizesDeclaration;

    FortranOpDatDimensionsDeclaration * dimensionsDeclaration;

  public:

    FortranCUDAModuleDeclarations (FortranParallelLoop * parallelLoop,
        SgScopeStatement * moduleScope,
        FortranCUDAOpDatCardinalitiesDeclaration * dataSizesDeclaration,
        FortranOpDatDimensionsDeclaration * dimensionsDeclaration);
};

#endif
