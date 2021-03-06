


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


#include <FortranCUDAReductionSubroutine.h>
#include <Reduction.h>
#include <FortranStatementsAndExpressionsBuilder.h>
#include <FortranTypesBuilder.h>
#include <RoseStatementsAndExpressionsBuilder.h>
#include <CompilerGeneratedNames.h>
#include <RoseHelper.h>
#include <Debug.h>
#include <CUDA.h>
#include <boost/lexical_cast.hpp>

void
FortranCUDAReductionSubroutine::createThreadZeroReductionStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;

  /*
   * ======================================================
   * The increment reduction case
   * ======================================================
   */

  SgPntrArrRefExp * subscriptExpression1 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression2 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression3 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), buildIntVal (0));

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      subscriptExpression1, buildAddOp (subscriptExpression2,
          subscriptExpression3));

  SgIntVal * incrementCaseExpression = buildIntVal (INCREMENT);

  SgCaseOptionStmt * incrementCaseStatement = buildCaseOptionStmt (
      incrementCaseExpression, buildBasicBlock (assignmentStatement1));

  /*
   * ======================================================
   * The minimum reduction case
   * ======================================================
   */

  SgPntrArrRefExp * subscriptExpression4 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression5 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), buildIntVal (0));

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      subscriptExpression4, subscriptExpression5);

  SgPntrArrRefExp * subscriptExpression6 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression7 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), buildIntVal (0));

  SgLessThanOp * minimumGuardExpression = buildLessThanOp (
      subscriptExpression7, subscriptExpression6);

  SgBasicBlock * minimumBody = buildBasicBlock (assignmentStatement2);

  SgIfStmt * minimumIfStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          minimumGuardExpression, minimumBody);

  SgIntVal * minimumCaseExpression = buildIntVal (MINIMUM);

  SgCaseOptionStmt * minimumCaseStatement = buildCaseOptionStmt (
      minimumCaseExpression, buildBasicBlock (minimumIfStatement));

  /*
   * ======================================================
   * The maximum reduction case
   * ======================================================
   */

  SgPntrArrRefExp * subscriptExpression8 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression9 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), buildIntVal (0));

  SgExprStatement * assignmentStatement3 = buildAssignStatement (
      subscriptExpression8, subscriptExpression9);

  SgPntrArrRefExp * subscriptExpression10 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionResult), buildIntVal (1));

  SgPntrArrRefExp * subscriptExpression11 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), buildIntVal (0));

  SgGreaterThanOp * maximumGuardExpression = buildGreaterThanOp (
      subscriptExpression11, subscriptExpression10);

  SgBasicBlock * maximumBody = buildBasicBlock (assignmentStatement3);

  SgIfStmt * maximumIfStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          maximumGuardExpression, maximumBody);

  SgIntVal * maximumCaseExpression = buildIntVal (MAXIMUM);

  SgCaseOptionStmt * maximumCaseStatement = buildCaseOptionStmt (
      maximumCaseExpression, buildBasicBlock (maximumIfStatement));

  /*
   * ======================================================
   * Switch statement on the reduction type with corresponding
   * cases
   * ======================================================
   */

  SgBasicBlock * switchBody = buildBasicBlock ();

  appendStatement (incrementCaseStatement, switchBody);

  appendStatement (minimumCaseStatement, switchBody);

  appendStatement (maximumCaseStatement, switchBody);

  SgSwitchStatement * switchStatement = buildSwitchStatement (
      variableDeclarations->getReference (reductionOperation), switchBody);

  /*
   * ======================================================
   * If statement determining whether this is thread 0
   * ======================================================
   */

  SgBasicBlock * ifBody = buildBasicBlock ();

  appendStatement (switchStatement, ifBody);

  SgExpression * ifGuardExpression = buildEqualityOp (
      variableDeclarations->getReference (threadID), buildIntVal (0));

  SgIfStmt * ifStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          ifGuardExpression, ifBody);

  appendStatement (ifStatement, subroutineScope);
}

void
FortranCUDAReductionSubroutine::createReductionStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;

  /*
   * ======================================================
   * The increment reduction case
   * ======================================================
   */

  SgPntrArrRefExp * arrayIndexExpressionA1 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName),
      variableDeclarations->getReference (threadID));

  SgAddOp * addExpressionA1 = buildAddOp (variableDeclarations->getReference (
      threadID), variableDeclarations->getReference (
      getIterationCounterVariableName (1)));

  SgPntrArrRefExp * arrayIndexExpressionA2 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), addExpressionA1);

  SgAddOp * addExpression3 = buildAddOp (arrayIndexExpressionA1,
      arrayIndexExpressionA2);

  SgExprStatement * assignmentStatementA = buildAssignStatement (
      arrayIndexExpressionA1, addExpression3);

  SgIntVal * incrementCaseExpression = buildIntVal (INCREMENT);

  SgCaseOptionStmt * incrementCaseStatement = buildCaseOptionStmt (
      incrementCaseExpression, buildBasicBlock (assignmentStatementA));

  /*
   * ======================================================
   * The minimum reduction case
   * ======================================================
   */

  SgPntrArrRefExp * arrayIndexExpressionB1 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName),
      variableDeclarations->getReference (threadID));

  SgAddOp * addExpressionB1 = buildAddOp (variableDeclarations->getReference (
      threadID), variableDeclarations->getReference (
      getIterationCounterVariableName (1)));

  SgPntrArrRefExp * arrayIndexExpressionB2 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), addExpressionB1);

  SgExprStatement * assignmentStatementB = buildAssignStatement (
      arrayIndexExpressionB1, arrayIndexExpressionB2);

  SgLessThanOp * minimumGuardExpression = buildLessThanOp (
      arrayIndexExpressionB2, arrayIndexExpressionB1);

  SgBasicBlock * minimumBody = buildBasicBlock (assignmentStatementB);

  SgIfStmt * minimumIfStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          minimumGuardExpression, minimumBody);

  SgIntVal * minimumCaseExpression = buildIntVal (MINIMUM);

  SgCaseOptionStmt * minimumCaseStatement = buildCaseOptionStmt (
      minimumCaseExpression, buildBasicBlock (minimumIfStatement));

  /*
   * ======================================================
   * The maximum reduction case
   * ======================================================
   */

  SgPntrArrRefExp * arrayIndexExpressionC1 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName),
      variableDeclarations->getReference (threadID));

  SgAddOp * addExpressionC1 = buildAddOp (variableDeclarations->getReference (
      threadID), variableDeclarations->getReference (
      getIterationCounterVariableName (1)));

  SgPntrArrRefExp * arrayIndexExpressionC2 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName), addExpressionC1);

  SgExprStatement * assignmentStatementC = buildAssignStatement (
      arrayIndexExpressionC1, arrayIndexExpressionC2);

  SgGreaterThanOp * maximumGuardExpression = buildGreaterThanOp (
      arrayIndexExpressionC2, arrayIndexExpressionC1);

  SgBasicBlock * maximumBody = buildBasicBlock (assignmentStatementC);

  SgIfStmt * maximumIfStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          maximumGuardExpression, maximumBody);

  SgIntVal * maximumCaseExpression = buildIntVal (MAXIMUM);

  SgCaseOptionStmt * maximumCaseStatement = buildCaseOptionStmt (
      maximumCaseExpression, buildBasicBlock (maximumIfStatement));

  /*
   * ======================================================
   * Switch statement on the reduction type with corresponding
   * cases
   * ======================================================
   */

  SgBasicBlock * switchBody = buildBasicBlock ();

  appendStatement (incrementCaseStatement, switchBody);

  appendStatement (minimumCaseStatement, switchBody);

  appendStatement (maximumCaseStatement, switchBody);

  SgSwitchStatement * switchStatement = buildSwitchStatement (
      variableDeclarations->getReference (reductionOperation), switchBody);

  /*
   * ======================================================
   * If statement controlling whether to index array, depending
   * on whether the thread ID is less than the loop counter
   * ======================================================
   */

  SgBasicBlock * ifBlock = buildBasicBlock (switchStatement);

  SgExpression * ifGuardExpression = buildLessThanOp (
      variableDeclarations->getReference (threadID),
      variableDeclarations->getReference (getIterationCounterVariableName (1)));

  SgIfStmt * ifStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          ifGuardExpression, ifBlock);

  /*
   * ======================================================
   * Build the loop
   * ======================================================
   */

  SgExprListExp * actualParametersItCountReassign = buildExprListExp (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      buildIntVal (-1));

  SgFunctionSymbol * shiftFunctionSymbol =
      FortranTypesBuilder::buildNewFortranFunction ("ishft", subroutineScope);

  SgFunctionCallExp * shiftFunctionCall = buildFunctionCallExp (
      shiftFunctionSymbol, actualParametersItCountReassign);

  SgExprStatement * reassignIterationCounter = buildAssignStatement (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      shiftFunctionCall);

  SgBasicBlock * whileLoopBody = buildBasicBlock ();

  appendStatement (buildExprStatement (
      CUDA::createDeviceThreadSynchronisationCallStatement (subroutineScope)),
      whileLoopBody);

  appendStatement (ifStatement, whileLoopBody);

  appendStatement (reassignIterationCounter, whileLoopBody);

  SgExpression * upperBoundExpression = buildGreaterThanOp (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      buildIntVal (0));

  SgWhileStmt * whileLoopStatement = buildWhileStmt (upperBoundExpression,
      whileLoopBody);

  whileLoopStatement->set_has_end_statement (true);

  appendStatement (whileLoopStatement, subroutineScope);
}

void
FortranCUDAReductionSubroutine::createSharedVariableInitialisationStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating shared variable initialisation statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgPntrArrRefExp * arrayExpression1 = buildPntrArrRefExp (
      variableDeclarations->getReference (sharedVariableName),
      variableDeclarations->getReference (threadID));

  SgPntrArrRefExp * arrayExpression2 = buildPntrArrRefExp (
      variableDeclarations->getReference (reductionInput), buildIntVal (1));

  SgExprStatement * assignStatement1 = buildAssignStatement (arrayExpression1,
      arrayExpression2);

  appendStatement (assignStatement1, subroutineScope);
}

void
FortranCUDAReductionSubroutine::createInitialisationStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;

  Debug::getInstance ()->debugMessage ("Creating initialisation statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Initialise the thread ID
   * ======================================================
   */

  SgSubtractOp * subtractExpression1 = buildSubtractOp (CUDA::getThreadId (
      THREAD_X, subroutineScope), buildIntVal (1));

  SgExprStatement * assignStatement1 = buildAssignStatement (
      variableDeclarations->getReference (threadID), subtractExpression1);

  appendStatement (assignStatement1, subroutineScope);

  SgExprListExp * actualParameters = buildExprListExp (
      CUDA::getThreadBlockDimension (THREAD_X, subroutineScope), buildIntVal (
          -1));

  SgFunctionSymbol * shiftFunctionSymbol =
      FortranTypesBuilder::buildNewFortranFunction ("ishft", subroutineScope);

  SgFunctionCallExp * shiftFunctionCall = buildFunctionCallExp (
      shiftFunctionSymbol, actualParameters);

  SgExprStatement * assignStatement2 = buildAssignStatement (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      shiftFunctionCall);

  appendStatement (assignStatement2, subroutineScope);
}

void
FortranCUDAReductionSubroutine::createStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;

  Debug::getInstance ()->debugMessage ("Creating statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  createInitialisationStatements ();

  appendStatement (buildExprStatement (
      CUDA::createDeviceThreadSynchronisationCallStatement (subroutineScope)),
      subroutineScope);

  createSharedVariableInitialisationStatements ();

  createReductionStatements ();

  appendStatement (buildExprStatement (
      CUDA::createDeviceThreadSynchronisationCallStatement (subroutineScope)),
      subroutineScope);

  createThreadZeroReductionStatements ();

  appendStatement (buildExprStatement (
      CUDA::createDeviceThreadSynchronisationCallStatement (subroutineScope)),
      subroutineScope);
}

void
FortranCUDAReductionSubroutine::createLocalVariableDeclarations ()
{
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage ("Creating local variable declarations",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Create shared memory variable declaration with the same
   * base type and size as the reduction variable
   * ======================================================
   */

  sharedVariableName = getSharedMemoryDeclarationName (
      reduction->getBaseType (), reduction->getVariableSize ());

  SgArrayType * arrayType = FortranTypesBuilder::getArray_RankOne (
      reduction->getBaseType (), 0, new SgAsteriskShapeExp (
          RoseHelper::getFileInfo ()));

  variableDeclarations->add (sharedVariableName,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          sharedVariableName, arrayType, subroutineScope, 1, CUDA_SHARED));

  /*
   * ======================================================
   * Create thread ID and iteration counter variables
   * ======================================================
   */

  variableDeclarations->add (getIterationCounterVariableName (1),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIterationCounterVariableName (1),
          FortranTypesBuilder::getFourByteInteger (), subroutineScope));

  variableDeclarations->add (
      threadID,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          threadID, FortranTypesBuilder::getFourByteInteger (), subroutineScope));
}

void
FortranCUDAReductionSubroutine::createFormalParameterDeclarations ()
{
  using namespace SageBuilder;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating reduction procedure formal parameter", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  /*
   * ======================================================
   * Declare the device variable on which the result of local
   * reductions is stored by the first thread in the block
   * ======================================================
   */
  variableDeclarations->add (
      reductionResult,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          reductionResult, FortranTypesBuilder::getArray_RankOne (
              reduction->getBaseType ()), subroutineScope, formalParameters, 1,
          CUDA_DEVICE));

  /*
   * ======================================================
   * Declare the value of the reduction variable produced by
   * each thread which is a single value. It is declared
   * as a rank-one array with one element to interoperate
   * between scalars and arrays
   * ======================================================
   */

  variableDeclarations->add (
      reductionInput,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          reductionInput, FortranTypesBuilder::getArray_RankOne (
              reduction->getBaseType (), 1, 1), subroutineScope,
          formalParameters));

  /*
   * ======================================================
   * Declare the the warp size: currently, in CUDA Fortran
   * it is not possible to access the warpsize variable value
   * in a device subroutine (only possible in global ones)
   * ======================================================
   */

  /* Carlo: modified 18/01/12 we don't use warpsize in the reduction routine*/
  /*variableDeclarations->add (
      warpSize,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          warpSize, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope, formalParameters, 1, VALUE));
  */
  
  /*
   * ======================================================
   * Declare the reduction operation variable
   * ======================================================
   */

  variableDeclarations->add (
      reductionOperation,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          reductionOperation, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope, formalParameters, 1, VALUE));
}

FortranCUDAReductionSubroutine::FortranCUDAReductionSubroutine (
    SgScopeStatement * moduleScope, Reduction * reduction) :
  Subroutine <SgProcedureHeaderStatement> (reduction->getSubroutineName ()),
      reduction (reduction)
{
  using namespace SageBuilder;
  using namespace SageInterface;

  Debug::getInstance ()->debugMessage ("Creating reduction subroutine",
      Debug::CONSTRUCTOR_LEVEL, __FILE__, __LINE__);

  subroutineHeaderStatement = buildProcedureHeaderStatement (
      this->subroutineName.c_str (), buildVoidType (), formalParameters,
      SgProcedureHeaderStatement::e_subroutine_subprogram_kind, moduleScope);

  subroutineHeaderStatement->get_functionModifier ().setCudaDevice ();

  appendStatement (subroutineHeaderStatement, moduleScope);

  subroutineScope = subroutineHeaderStatement->get_definition ()->get_body ();

  createFormalParameterDeclarations ();

  createLocalVariableDeclarations ();

  createStatements ();
}
