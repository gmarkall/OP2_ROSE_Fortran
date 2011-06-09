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

    FortranCUDADataSizesDeclarationIndirectLoop
        * dataSizesDeclaration;

    std::map <std::string, unsigned int> positionOf_nbytes;

    /*
     * ======================================================
     * Remaining local variables
     * ======================================================
     */

    std::map <unsigned int, SgVariableDeclaration *> localVariables_indArgSizes;

    std::map <unsigned int, SgVariableDeclaration *>
        localVariables_incrementAccessMaps;

    std::map <unsigned int, SgVariableDeclaration *>
        localVariables_incrementAccessArrays;

  private:

    SgStatement *
    createUserSubroutineCall ();

    void
    applyPointeredIncrementsOrWrites ();

    void
    createPlanWhileLoop ();

    void
    createAutosharedWhileLoops ();

    void
    initialiseLocalVariables ();

    void
    createThreadZeroStatements ();

    void
    createPlanlocalVariableDeclarations ();

    void
    create_OP_DAT_localVariableDeclarations ();

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
        FortranCUDADataSizesDeclarationIndirectLoop * dataSizesDeclaration);

};

#endif
