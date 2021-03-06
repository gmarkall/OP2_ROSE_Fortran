


/*  Open source copyright declaration based on BSD open source template:
 *  http://www.opensource.org/licenses/bsd-license.php
 * 
 * Copyright (c) 2011-2012, Adam Betts, Carlo Bertolli
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "FortranOpenMPModuleDeclarationsIndirectLoop.h"
#include "FortranParallelLoop.h"
#include "FortranTypesBuilder.h"
#include "FortranStatementsAndExpressionsBuilder.h"
#include "ScopedVariableDeclarations.h"
#include "Debug.h"
#include "CompilerGeneratedNames.h"
#include "PlanFunctionNames.h"
#include <rose.h>

void
FortranOpenMPModuleDeclarationsIndirectLoop::createFirstTimeExecutionDeclaration ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace BooleanVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating first time execution boolean at module scope",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgVariableDeclaration * variableDeclaration =
      buildVariableDeclaration (getFirstTimeExecutionVariableName (
          parallelLoop->getUserSubroutineName ()), buildBoolType (),
          buildAssignInitializer (buildBoolValExp (true), buildBoolType ()),
          moduleScope);

  variableDeclaration->get_declarationModifier ().get_accessModifier ().setUndefined ();

  variableDeclarations->add (getFirstTimeExecutionVariableName (
      parallelLoop->getUserSubroutineName ()), variableDeclaration);

  appendStatement (variableDeclaration, moduleScope);
}

void
FortranOpenMPModuleDeclarationsIndirectLoop::createExecutionPlanDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace PlanFunctionVariableNames;
  using std::map;
  using std::string;
  using std::vector;

  Debug::getInstance ()->debugMessage (
      "Creating execution plan variables at module scope",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Create pointers to the execution plan on the C and
   * Fortran side
   * ======================================================
   */

  SgType * c_ptrType = FortranTypesBuilder::buildClassDeclaration ("c_ptr",
      moduleScope)->get_type ();

  variableDeclarations ->add (getPlanReturnVariableName (
      parallelLoop->getUserSubroutineName ()),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getPlanReturnVariableName (parallelLoop->getUserSubroutineName ()),
          c_ptrType, moduleScope));

  SgType * op_planType = FortranTypesBuilder::buildClassDeclaration ("op_plan",
      moduleScope)->get_type ();

  variableDeclarations->add (getActualPlanVariableName (
      parallelLoop->getUserSubroutineName ()),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getActualPlanVariableName (parallelLoop->getUserSubroutineName ()),
          buildPointerType (op_planType), moduleScope));

  variableDeclarations->add (getIndirectOpDatsLocalToGlobalMappingName (
      parallelLoop->getUserSubroutineName ()),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIndirectOpDatsLocalToGlobalMappingName (
              parallelLoop->getUserSubroutineName ()), buildPointerType (
              FortranTypesBuilder::getArray_RankOne (c_ptrType)), moduleScope));

  variableDeclarations->add (getOpDatsGlobalToLocalMappingName (
      parallelLoop->getUserSubroutineName ()),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getOpDatsGlobalToLocalMappingName (
              parallelLoop->getUserSubroutineName ()), buildPointerType (
              FortranTypesBuilder::getArray_RankOne (c_ptrType)), moduleScope));

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isIndirect (i))
      {
        string const variableName = getLocalToGlobalMappingName (i,
            parallelLoop->getUserSubroutineName ());

        variableDeclarations->add (variableName,
            FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName, buildPointerType (
                    FortranTypesBuilder::getArray_RankOne (
                        FortranTypesBuilder::getFourByteInteger ())),
                moduleScope));
      }
    }
  }

  /*
   * ======================================================
   * Create variables modelling the indirect mappings to
   * local indices in shared memory
   * ======================================================
   */

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIndirect (i))
    {
      string const variableName = getGlobalToLocalMappingName (i,
          parallelLoop->getUserSubroutineName ());

      variableDeclarations->add (variableName,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, buildPointerType (
                  FortranTypesBuilder::getArray_RankOne (
                      FortranTypesBuilder::getTwoByteInteger ())), moduleScope));
    }
  }

  /*
   * ======================================================
   * Create variables modelling the number of elements
   * in each block of an indirect mapping
   * ======================================================
   */

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIndirect (i))
    {
      string const variableName = getGlobalToLocalMappingSizeName (i,
          parallelLoop->getUserSubroutineName ());

      variableDeclarations->add (variableName,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, FortranTypesBuilder::getFourByteInteger (),
              moduleScope));
    }
  }

  map <string, string> planVariablesWithSizes;

  /*
   * ======================================================
   * Colour to block array declarations: 'blkmap'
   * ======================================================
   */

  planVariablesWithSizes[getColourToBlockArrayName (
      parallelLoop->getUserSubroutineName ())] = getColourToBlockSizeName (
      parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Number of thread colours per block declarations: 'nthrcol'
   * ======================================================
   */

  planVariablesWithSizes[getNumberOfThreadColoursPerBlockArrayName (
      parallelLoop->getUserSubroutineName ())]
      = getNumberOfThreadColoursPerBlockSizeName (
          parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Thread colour declarations: 'thrcol'
   * ======================================================
   */

  planVariablesWithSizes[getThreadColourArrayName (
      parallelLoop->getUserSubroutineName ())] = getThreadColourSizeName (
      parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Thread colour declarations: 'ind_sizes'
   * ======================================================
   */

  planVariablesWithSizes[getIndirectOpDatsNumberOfElementsArrayName (
      parallelLoop->getUserSubroutineName ())]
      = getIndirectOpDatsNumberOfElementsSizeName (
          parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Indirect OP_DAT offsets declarations: 'ind_offs'
   * ======================================================
   */

  planVariablesWithSizes[getIndirectOpDatsOffsetArrayName (
      parallelLoop->getUserSubroutineName ())]
      = getIndirectOpDatsOffsetSizeName (parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Offset into block declarations: 'offset'
   * ======================================================
   */

  planVariablesWithSizes[getOffsetIntoBlockArrayName (
      parallelLoop->getUserSubroutineName ())] = getOffsetIntoBlockSizeName (
      parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Number of OP_SET elements per block declarations: 'nelems'
   * ======================================================
   */

  planVariablesWithSizes[getNumberOfSetElementsPerBlockArrayName (
      parallelLoop->getUserSubroutineName ())]
      = getNumberOfSetElementsPerBlockSizeName (
          parallelLoop->getUserSubroutineName ());

  /*
   * ======================================================
   * Add the declarations
   * ======================================================
   */

  for (map <string, string>::iterator it = planVariablesWithSizes.begin (); it
      != planVariablesWithSizes.end (); ++it)
  {
    std::string const & variableName1 = it->first;
    std::string const & variableName2 = it->second;

    variableDeclarations->add (variableName1,
        FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
            variableName1, buildPointerType (
                FortranTypesBuilder::getArray_RankOne (
                    FortranTypesBuilder::getFourByteInteger ())), moduleScope));

    variableDeclarations->add (variableName2,
        FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
            variableName2, FortranTypesBuilder::getFourByteInteger (),
            moduleScope));
  }

  /*
   * ======================================================
   * Thread colour declarations: 'ncolblk'
   * ======================================================
   */

  std::string const & variableName1 = getColourToNumberOfBlocksArrayName (
      parallelLoop->getUserSubroutineName ());

  variableDeclarations->add (variableName1,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          variableName1, buildPointerType (
              FortranTypesBuilder::getArray_RankOne (
                  FortranTypesBuilder::getFourByteInteger ())), moduleScope));

  /*
   * ======================================================
   * Indirect OP_DAT array declarations: 'nindirect'
   * ======================================================
   */

  std::string const & variableName2 = getIndirectOpDatsArrayName (
      parallelLoop->getUserSubroutineName ());

  variableDeclarations->add (variableName2,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          variableName2, buildPointerType (
              FortranTypesBuilder::getArray_RankOne (
                  FortranTypesBuilder::getFourByteInteger ())), moduleScope));
}

FortranOpenMPModuleDeclarationsIndirectLoop::FortranOpenMPModuleDeclarationsIndirectLoop (
    FortranParallelLoop * parallelLoop, SgScopeStatement * moduleScope) :
  FortranOpenMPModuleDeclarations (parallelLoop, moduleScope)
{
  createFirstTimeExecutionDeclaration ();

  createExecutionPlanDeclarations ();
}
