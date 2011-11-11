#pragma once
#ifndef CPP_OPENMP_HOST_SUBROUTINE_H
#define CPP_OPENMP_HOST_SUBROUTINE_H

#include <CPPHostSubroutine.h>

class CPPOpenMPKernelSubroutine;

class CPPOpenMPHostSubroutine: public CPPHostSubroutine
{
  protected:

    SgBasicBlock *
    createInitialiseNumberOfThreadsStatements ();

    SgBasicBlock *
    createOpDatTypeCastStatements ();

    void
    createOpDatTypeCastVariableDeclarations ();

    virtual void
    createReductionEpilogueStatements ();

    virtual void
    createReductionPrologueStatements ();

    virtual void
    createReductionDeclarations ();

    void
    createOpenMPLocalVariableDeclarations ();

    CPPOpenMPHostSubroutine (SgScopeStatement * moduleScope,
        CPPOpenMPKernelSubroutine * calleeSubroutine,
        CPPParallelLoop * parallelLoop);
};

#endif
