#pragma once
#ifndef FORTRAN_OPENMP_KERNEL_SUBROUTINE_INDIRECT_LOOP_H
#define FORTRAN_OPENMP_KERNEL_SUBROUTINE_INDIRECT_LOOP_H

#include <FortranOpenMPKernelSubroutine.h>

class FortranOpenMPKernelSubroutineIndirectLoop: public FortranOpenMPKernelSubroutine
{
  private:

    std::map <std::string, SgVariableDeclaration *> numberOfBytesDeclarations;

  private:

    void
    createInitialiseIncrementAccessVariablesStatements ();

    void
    createInitialiseThreadVariablesStatements ();

    void
    createIncrementAccessLocalVariableDeclarations ();

    void
    createSharedVariableDeclarations ();

    void
    createIndirectOpDatCardinalityLocalVariableDeclarations ();

    void
    createRoundUpLocalVariableDeclarations ();

    void
    createNumberOfBytesPerOpDatLocalVariableDeclarations ();

    void
    createExecutionLocalVariableDeclarations ();

    virtual SgStatement *
    createUserSubroutineCallStatement ();

    virtual void
    createExecutionLoopStatements ();

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

    FortranOpenMPKernelSubroutineIndirectLoop (SgScopeStatement * moduleScope,
        Subroutine <SgProcedureHeaderStatement> * userSubroutine,
        FortranParallelLoop * parallelLoop);
};

#endif
