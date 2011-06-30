#ifndef FORTRAN_CUDA_HOST_SUBROUTINE_H
#define FORTRAN_CUDA_HOST_SUBROUTINE_H

#include <FortranHostSubroutine.h>
#include <FortranCUDAInitialiseConstantsSubroutine.h>
#include <FortranCUDAKernelSubroutine.h>
#include <FortranOpDatDimensionsDeclaration.h>
#include <FortranParallelLoop.h>

class FortranCUDAHostSubroutine: public FortranHostSubroutine
{
  protected:

    FortranCUDAInitialiseConstantsSubroutine * initialiseConstantsSubroutine;

    FortranOpDatDimensionsDeclaration * opDatDimensionsDeclaration;

  protected:

    /*
     * ======================================================
     * Returns a statement which represents a CUDA thread
     * synchronisation call
     * ======================================================
     */
    SgStatement *
    createThreadSynchroniseCallStatement ();

    /*
     * ======================================================
     * Returns a statement which calls the initialise
     * constants subroutine
     * ======================================================
     */
    SgStatement *
    createInitialiseConstantsCallStatement ();

    virtual void
    createReductionEpilogueStatements ();

    virtual void
    createReductionPrologueStatements ();

    virtual void
    createReductionLocalVariableDeclarations ();

    /*
     * ======================================================
     * Creates the statements needed after the kernel has
     * completed
     * ======================================================
     */
    void
    createCUDAKernelEpilogueStatements ();

    /*
     * ======================================================
     * Creates the statements needed before the kernel is
     * launched
     * ======================================================
     */
    void
    createCUDAKernelPrologueStatements ();

    virtual void
    createTransferOpDatStatements (SgScopeStatement * statementScope);

    void
    createOpDatDimensionInitialisationStatements ();

    void
    createCUDAKernelLocalVariableDeclarations ();

    void
    createDataMarshallingLocalVariableDeclarations ();

        FortranCUDAHostSubroutine (
            std::string const & subroutineName,
            std::string const & userSubroutineName,
            std::string const & kernelSubroutineName,
            FortranParallelLoop * parallelLoop,
            SgScopeStatement * moduleScope,
            FortranCUDAInitialiseConstantsSubroutine * initialiseConstantsSubroutine,
            FortranOpDatDimensionsDeclaration * opDatDimensionsDeclaration);
};

#endif
