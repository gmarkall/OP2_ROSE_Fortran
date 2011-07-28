#include <CPPOpenCLKernelSubroutine.h>

/*
 * ======================================================
 * Protected functions
 * ======================================================
 */

CPPOpenCLKernelSubroutine::CPPOpenCLKernelSubroutine (
    std::string const & subroutineName, std::string const & userSubroutineName,
    SgScopeStatement * moduleScope, CPPParallelLoop * parallelLoop) :
  CPPKernelSubroutine (subroutineName, userSubroutineName, moduleScope,
      parallelLoop)
{
}