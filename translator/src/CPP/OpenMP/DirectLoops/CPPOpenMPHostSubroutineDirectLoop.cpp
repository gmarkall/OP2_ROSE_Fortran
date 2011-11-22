#include "CPPOpenMPHostSubroutineDirectLoop.h"
#include "CPPOpenMPKernelSubroutine.h"
#include "RoseStatementsAndExpressionsBuilder.h"
#include "RoseHelper.h"
#include "CompilerGeneratedNames.h"
#include "OpenMP.h"
#include "OP2.h"

SgStatement *
CPPOpenMPHostSubroutineDirectLoop::createKernelFunctionCallStatement ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;
  using namespace LoopVariableNames;

  Debug::getInstance ()->debugMessage ("Creating call to OpenMP kernel",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgExprListExp * actualParameters = buildExprListExp ();

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDirect (i))
    {
      actualParameters->append_expression (variableDeclarations->getReference (
          getOpDatLocalName (i)));
    }
    else if (parallelLoop->isReductionRequired (i))
    {
      SgMultiplyOp * multiplyExpression = buildMultiplyOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              1)), buildIntVal (64));

      SgAddOp * addExpression = buildAddOp (variableDeclarations->getReference (
          getReductionArrayHostName (i)), multiplyExpression);

      actualParameters->append_expression (addExpression);
    }
  }

  actualParameters->append_expression (variableDeclarations->getReference (
      OpenMP::sliceStart));

  actualParameters->append_expression (variableDeclarations->getReference (
      OpenMP::sliceEnd));

  SgFunctionCallExp * functionCallExpression = buildFunctionCallExp (
      calleeSubroutine->getSubroutineName (), buildVoidType (),
      actualParameters, subroutineScope);

  return buildExprStatement (functionCallExpression);
}

void
CPPOpenMPHostSubroutineDirectLoop::createOpenMPLoopStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OP2VariableNames;
  using namespace LoopVariableNames;
  using namespace OP2::RunTimeVariableNames;
  using namespace OpenMP;

  Debug::getInstance ()->debugMessage ("Creating OpenMP for loop statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgBasicBlock * loopBody = buildBasicBlock ();

  /*
   * ======================================================
   * New statement
   * ======================================================
   */

  SgArrowExp * arrowExpression1 = buildArrowExp (
      variableDeclarations->getReference (set), buildOpaqueVarRefExp (size,
          subroutineScope));

  SgMultiplyOp * multiplyExpression1 = buildMultiplyOp (arrowExpression1,
      variableDeclarations->getReference (getIterationCounterVariableName (1)));

  SgDivideOp * divideExpression1 = buildDivideOp (multiplyExpression1,
      variableDeclarations->getReference (numberOfThreads));

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      variableDeclarations->getReference (sliceStart), divideExpression1);

  appendStatement (assignmentStatement1, loopBody);

  /*
   * ======================================================
   * New statement
   * ======================================================
   */

  SgArrowExp * arrowExpression2 = buildArrowExp (
      variableDeclarations->getReference (set), buildOpaqueVarRefExp (size,
          subroutineScope));

  SgAddOp * addExpression2 = buildAddOp (variableDeclarations->getReference (
      getIterationCounterVariableName (1)), buildIntVal (1));

  SgMultiplyOp * multiplyExpression2 = buildMultiplyOp (arrowExpression2,
      addExpression2);

  SgDivideOp * divideExpression2 = buildDivideOp (multiplyExpression2,
      variableDeclarations->getReference (numberOfThreads));

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      variableDeclarations->getReference (sliceEnd), divideExpression2);

  appendStatement (assignmentStatement2, loopBody);

  /*
   * ======================================================
   * New statement
   * ======================================================
   */

  appendStatement (createKernelFunctionCallStatement (), loopBody);

  /*
   * ======================================================
   * For loop statement
   * ======================================================
   */

  SgExprStatement * initialisationExpression = buildAssignStatement (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      buildIntVal (0));

  SgLessThanOp * upperBoundExpression = buildLessThanOp (
      variableDeclarations->getReference (getIterationCounterVariableName (1)),
      variableDeclarations->getReference (numberOfThreads));

  SgPlusPlusOp * strideExpression = buildPlusPlusOp (
      variableDeclarations->getReference (getIterationCounterVariableName (1)));

  SgForStatement * forLoopStatement = buildForStatement (
      initialisationExpression, buildExprStatement (upperBoundExpression),
      strideExpression, loopBody);

  appendStatement (forLoopStatement, subroutineScope);

  /*
   * ======================================================
   * The OpenMP directive
   * ======================================================
   */

  std::vector <SgVarRefExp *> privateVariableReferences;

  privateVariableReferences.push_back (variableDeclarations->getReference (
      OpenMP::sliceStart));

  privateVariableReferences.push_back (variableDeclarations->getReference (
      OpenMP::sliceEnd));

  privateVariableReferences.push_back (variableDeclarations->getReference (
      getIterationCounterVariableName (1)));

  addTextForUnparser (forLoopStatement,
      OpenMP::getParallelLoopDirectiveString () + OpenMP::getPrivateClause (
          privateVariableReferences), AstUnparseAttribute::e_before);
}

void
CPPOpenMPHostSubroutineDirectLoop::createStatements ()
{
  using namespace SageInterface;

  Debug::getInstance ()->debugMessage ("Creating statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  appendStatementList (
      createInitialiseNumberOfThreadsStatements ()->getStatementList (),
      subroutineScope);

  appendStatementList (createOpDatTypeCastStatements ()->getStatementList (),
      subroutineScope);

  if (parallelLoop->isReductionRequired ())
  {
    createReductionPrologueStatements ();
  }

  createOpenMPLoopStatements ();

  if (parallelLoop->isReductionRequired ())
  {
    createReductionEpilogueStatements ();
  }
}

void
CPPOpenMPHostSubroutineDirectLoop::createLocalVariableDeclarations ()
{
  Debug::getInstance ()->debugMessage ("Creating local variable declarations",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  createOpenMPLocalVariableDeclarations ();

  createOpDatTypeCastVariableDeclarations ();

  if (parallelLoop->isReductionRequired ())
  {
    createReductionDeclarations ();
  }
}

CPPOpenMPHostSubroutineDirectLoop::CPPOpenMPHostSubroutineDirectLoop (
    SgScopeStatement * moduleScope,
    CPPOpenMPKernelSubroutine * calleeSubroutine,
    CPPParallelLoop * parallelLoop) :
  CPPOpenMPHostSubroutine (moduleScope, calleeSubroutine, parallelLoop)
{
  Debug::getInstance ()->debugMessage (
      "Creating host subroutine of direct loop", Debug::CONSTRUCTOR_LEVEL,
      __FILE__, __LINE__);

  createFormalParameterDeclarations ();

  createLocalVariableDeclarations ();

  createStatements ();
}
