/*
 * Written by Adam Betts and Carlo Bertolli
 *
 * This class models the kernel subroutine for an indirect loop.
 * Its declarations and statements do the following:
 *
 */

#ifndef FORTRAN_CUDA_KERNEL_SUBROUTINE_INDIRECT_LOOP_H
#define FORTRAN_CUDA_KERNEL_SUBROUTINE_INDIRECT_LOOP_H

#include <FortranCUDAKernelSubroutine.h>
#include <FortranCUDADataSizesDeclarationIndirectLoop.h>

class FortranCUDAKernelSubroutineIndirectLoop: public FortranCUDAKernelSubroutine
{
  private:

    FortranCUDADataSizesDeclarationIndirectLoop * dataSizesDeclaration;

    std::map <std::string, unsigned int> positionOfNbytes;

  private:

    virtual SgStatement *
    createUserSubroutineCallStatement ();

    virtual void
    createExecutionLoopStatements ();

    void
    createPointeredIncrementsOrWritesStatements ();

    void
    createAutoSharedWhileLoopStatements ();

    void
    createInitialiseLocalVariablesStatements ();

    void
    createThreadZeroStatements ();

    void
    createPlanFormalParameterDeclarations ();

    virtual void
    createOpDatFormalParameterDeclarations ();

    virtual void
    createStatements ();

    virtual void
    createLocalVariableDeclarations ();

    virtual void
    createFormalParameterDeclarations ();

  public:

    FortranCUDAKernelSubroutineIndirectLoop (
        std::string const & subroutineName,
        std::string const & userSubroutineName, ParallelLoop * parallelLoop,
        SgScopeStatement * moduleScope,
        FortranCUDADataSizesDeclarationIndirectLoop * dataSizesDeclaration,
        FortranOpDatDimensionsDeclaration * opDatDimensionsDeclaration);

};

#endif
