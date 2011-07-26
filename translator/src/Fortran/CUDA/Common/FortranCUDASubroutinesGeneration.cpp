#include <FortranCUDASubroutinesGeneration.h>
#include <FortranCUDAModuleDeclarationsIndirectLoop.h>
#include <FortranCUDAInitialiseConstantsSubroutine.h>
#include <FortranCUDAKernelSubroutineDirectLoop.h>
#include <FortranCUDAKernelSubroutineIndirectLoop.h>
#include <FortranCUDAHostSubroutineDirectLoop.h>
#include <FortranCUDAHostSubroutineIndirectLoop.h>
#include <FortranCUDAUserSubroutine.h>
#include <RoseHelper.h>

/*
 * ======================================================
 * Private functions
 * ======================================================
 */

FortranHostSubroutine *
FortranCUDASubroutinesGeneration::createSubroutines (
    FortranParallelLoop * parallelLoop, std::string const & userSubroutineName)
{
  /*
   * ======================================================
   * Create the type representing the dimensions of each
   * OP_DAT
   * ======================================================
   */

  FortranOpDatDimensionsDeclaration * dimensionsDeclaration =
      new FortranOpDatDimensionsDeclaration (userSubroutineName, parallelLoop,
          moduleScope);

  if (parallelLoop->isReductionRequired ())
  {
    parallelLoop->generateReductionSubroutines (moduleScope);
  }

  FortranCUDAUserSubroutine * userDeviceSubroutine;

  FortranCUDAKernelSubroutine * kernelSubroutine;

  FortranCUDAHostSubroutine * hostSubroutine;

  if (parallelLoop->isDirectLoop ())
  {
    FortranCUDADataSizesDeclarationDirectLoop * dataSizesDeclaration =
        new FortranCUDADataSizesDeclarationDirectLoop (userSubroutineName,
            parallelLoop, moduleScope);

    FortranCUDAInitialiseConstantsSubroutine * initialiseConstantsSubroutine =
        new FortranCUDAInitialiseConstantsSubroutine (userSubroutineName,
            moduleScope, declarations);

    FortranCUDAModuleDeclarations * moduleDeclarations =
        new FortranCUDAModuleDeclarations (userSubroutineName, parallelLoop,
            moduleScope, dataSizesDeclaration, dimensionsDeclaration);

    initialiseConstantsSubroutine->generateSubroutine ();

    userDeviceSubroutine = new FortranCUDAUserSubroutine (userSubroutineName,
        moduleScope, initialiseConstantsSubroutine, declarations, parallelLoop);

    kernelSubroutine = new FortranCUDAKernelSubroutineDirectLoop (
        userSubroutineName, userDeviceSubroutine->getSubroutineName (),
        parallelLoop, moduleScope, dataSizesDeclaration, dimensionsDeclaration);

    hostSubroutine = new FortranCUDAHostSubroutineDirectLoop (
        userSubroutineName, userSubroutineName,
        kernelSubroutine->getSubroutineName (), parallelLoop, moduleScope,
        initialiseConstantsSubroutine, dataSizesDeclaration,
        dimensionsDeclaration, moduleDeclarations);
  }
  else
  {
    FortranCUDADataSizesDeclarationIndirectLoop * dataSizesDeclaration =
        new FortranCUDADataSizesDeclarationIndirectLoop (userSubroutineName,
            parallelLoop, moduleScope);

    FortranCUDAInitialiseConstantsSubroutine * initialiseConstantsSubroutine =
        new FortranCUDAInitialiseConstantsSubroutine (userSubroutineName,
            moduleScope, declarations);

    FortranCUDAModuleDeclarationsIndirectLoop * moduleDeclarations =
        new FortranCUDAModuleDeclarationsIndirectLoop (userSubroutineName,
            parallelLoop, moduleScope, dataSizesDeclaration,
            dimensionsDeclaration);

    initialiseConstantsSubroutine->generateSubroutine ();

    userDeviceSubroutine = new FortranCUDAUserSubroutine (userSubroutineName,
        moduleScope, initialiseConstantsSubroutine, declarations, parallelLoop);

    kernelSubroutine = new FortranCUDAKernelSubroutineIndirectLoop (
        userSubroutineName, userDeviceSubroutine->getSubroutineName (),
        parallelLoop, moduleScope, dataSizesDeclaration, dimensionsDeclaration);

    hostSubroutine = new FortranCUDAHostSubroutineIndirectLoop (
        userSubroutineName, userSubroutineName,
        kernelSubroutine->getSubroutineName (), parallelLoop, moduleScope,
        initialiseConstantsSubroutine, dataSizesDeclaration,
        dimensionsDeclaration, moduleDeclarations);
  }

  return hostSubroutine;
}

void
FortranCUDASubroutinesGeneration::createModuleDeclarations ()
{
  using std::map;
  using std::string;

  /*
   * ======================================================
   * First declare the type declarations
   * ======================================================
   */

  for (map <string, FortranParallelLoop *>::const_iterator it =
      declarations->firstParallelLoop (); it
      != declarations->lastParallelLoop (); ++it)
  {
    string const userSubroutineName = it->first;

    FortranParallelLoop * parallelLoop = it->second;

    dimensionsDeclarations[userSubroutineName]
        = new FortranOpDatDimensionsDeclaration (userSubroutineName,
            parallelLoop, moduleScope);

    if (parallelLoop->isDirectLoop ())
    {
      dataSizesDeclarations[userSubroutineName]
          = new FortranCUDADataSizesDeclarationDirectLoop (userSubroutineName,
              parallelLoop, moduleScope);
    }
    else
    {
      dataSizesDeclarations[userSubroutineName]
          = new FortranCUDADataSizesDeclarationIndirectLoop (
              userSubroutineName, parallelLoop, moduleScope);
    }
  }

  /*
   * ======================================================
   * Now declare the variables
   * ======================================================
   */

  for (map <string, FortranParallelLoop *>::const_iterator it =
      declarations->firstParallelLoop (); it
      != declarations->lastParallelLoop (); ++it)
  {
    string const userSubroutineName = it->first;

    FortranParallelLoop * parallelLoop = it->second;

    if (parallelLoop->isDirectLoop ())
    {
      moduleDeclarations[userSubroutineName]
          = new FortranCUDAModuleDeclarations (userSubroutineName,
              parallelLoop, moduleScope,
              dataSizesDeclarations[userSubroutineName],
              dimensionsDeclarations[userSubroutineName]);
    }
    else
    {
      moduleDeclarations[userSubroutineName]
          = new FortranCUDAModuleDeclarationsIndirectLoop (
              userSubroutineName,
              parallelLoop,
              moduleScope,
              static_cast <FortranCUDADataSizesDeclarationIndirectLoop *> (dataSizesDeclarations[userSubroutineName]),
              dimensionsDeclarations[userSubroutineName]);
    }
  }
}

void
FortranCUDASubroutinesGeneration::addLibraries ()
{
  using std::string;
  using std::vector;
  using SageInterface::appendStatement;

  Debug::getInstance ()->debugMessage (
      "Adding 'use' statements to CUDA module", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  vector <string> libs;
  libs.push_back (Libraries::ISO_C_BINDING);
  libs.push_back (Libraries::OP2_C);
  libs.push_back (Libraries::cudaConfigurationParams);
  libs.push_back (Libraries::CUDAFOR);

  for (vector <string>::const_iterator it = libs.begin (); it != libs.end (); ++it)
  {
    SgUseStatement* useStatement = new SgUseStatement (
        RoseHelper::getFileInfo (), *it, false);

    useStatement->set_definingDeclaration (useStatement);

    appendStatement (useStatement, moduleScope);
  }
}

/*
 * ======================================================
 * Public functions
 * ======================================================
 */

FortranCUDASubroutinesGeneration::FortranCUDASubroutinesGeneration (
    SgProject * project,
    FortranProgramDeclarationsAndDefinitions * declarations) :
  FortranSubroutinesGeneration (declarations, "cuda_code.CUF")
{
  addLibraries ();

  createModuleDeclarations ();

  unparse ();
}
