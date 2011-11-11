#include <CPPOpenMPHostSubroutine.h>
#include <CPPParallelLoop.h>
#include <CPPOpenMPKernelSubroutine.h>
#include <RoseStatementsAndExpressionsBuilder.h>
#include <CompilerGeneratedNames.h>
#include <OP2Definitions.h>
#include <OpenMP.h>
#include <Exceptions.h>

SgBasicBlock *
CPPOpenMPHostSubroutine::createThreadSpecificVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OpenMP;
  using namespace LoopVariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating declaration needed per thread", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  SgBasicBlock * block = buildBasicBlock ();

  variableDeclarations->add (getIterationCounterVariableName (1),
      RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIterationCounterVariableName (1), buildIntType (), block));

  variableDeclarations->add (sliceStart,
      RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
          sliceStart, buildIntType (), block));

  variableDeclarations->add (sliceEnd,
      RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (sliceEnd,
          buildIntType (), block));

  return block;
}

SgBasicBlock *
CPPOpenMPHostSubroutine::createInitialiseNumberOfThreadsStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OpenMP;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating statements to initialise the number of threads",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgBasicBlock * block = buildBasicBlock ();

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      variableDeclarations->getReference (numberOfThreads),
      OpenMP::createGetMaximumNumberOfThreadsCallStatement (subroutineScope));

  appendStatement (assignmentStatement1, block);

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      variableDeclarations->getReference (numberOfThreads), buildIntVal (1));

  appendStatement (assignmentStatement2, block);

  addTextForUnparser (assignmentStatement1, OpenMP::getIfDirectiveString (),
      AstUnparseAttribute::e_before);

  addTextForUnparser (assignmentStatement2, OpenMP::getElseDirectiveString (),
      AstUnparseAttribute::e_before);

  addTextForUnparser (assignmentStatement2, OpenMP::getEndIfDirectiveString (),
      AstUnparseAttribute::e_after);

  return block;
}

SgBasicBlock *
CPPOpenMPHostSubroutine::createOpDatTypeCastStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OP2VariableNames;
  using namespace OP2::RunTimeVariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating statements to type cast OP_DATs into their correct types",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgBasicBlock * block = buildBasicBlock ();

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    SgDotExp * dotExpression = buildDotExp (variableDeclarations->getReference (
        getOpDatName (i)), buildOpaqueVarRefExp (dataOnHost, subroutineScope));

    SgCastExp * castExpression = buildCastExp (dotExpression, buildPointerType (
        parallelLoop->getOpDatBaseType (i)));

    SgExprStatement * assignmentStatement = buildAssignStatement (
        variableDeclarations->getReference (getOpDatLocalName (i)),
        castExpression);

    appendStatement (assignmentStatement, subroutineScope);
  }

  return block;
}

void
CPPOpenMPHostSubroutine::createOpDatTypeCastVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating local variable declarations to enable OP_DATs to be type cast into their correct types",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    std::string const & variableName = getOpDatLocalName (i);

    variableDeclarations->add (variableName,
        RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
            variableName,
            buildPointerType (parallelLoop->getOpDatBaseType (i)),
            subroutineScope));
  }
}

void
CPPOpenMPHostSubroutine::createReductionEpilogueStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OP2VariableNames;
  using namespace LoopVariableNames;
  using namespace ReductionVariableNames;
  using namespace OpenMP;

  Debug::getInstance ()->debugMessage (
      "Creating reduction epilogue statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isReductionRequired (i))
    {
      SgBasicBlock * loopBody = buildBasicBlock ();

      /*
       * ======================================================
       * New statement
       * ======================================================
       */

      SgMultiplyOp * arrayIndexExpression = buildMultiplyOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), buildIntVal (64));

      SgPntrArrRefExp * arrayExpression1 = buildPntrArrRefExp (
          variableDeclarations->getReference (getReductionArrayHostName (i)),
          arrayIndexExpression);

      SgPntrArrRefExp * arrayExpression2 = buildPntrArrRefExp (
          variableDeclarations->getReference (getOpDatLocalName (i)),
          buildIntVal (0));

      SgPlusAssignOp * assignmentStatement = buildPlusAssignOp (
          arrayExpression2, arrayExpression1);

      appendStatement (buildExprStatement (assignmentStatement), loopBody);

      /*
       * ======================================================
       * For loop statement
       * ======================================================
       */

      SgExprStatement * initialisationExpression = buildAssignStatement (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), buildIntVal (0));

      SgLessThanOp * upperBoundExpression = buildLessThanOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), variableDeclarations->getReference (numberOfThreads));

      SgPlusPlusOp * strideExpression = buildPlusPlusOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)));

      SgForStatement * forLoopStatement = buildForStatement (
          initialisationExpression, buildExprStatement (upperBoundExpression),
          strideExpression, loopBody);

      appendStatement (forLoopStatement, subroutineScope);
    }
  }
}

void
CPPOpenMPHostSubroutine::createReductionPrologueStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace OP2VariableNames;
  using namespace LoopVariableNames;
  using namespace ReductionVariableNames;
  using namespace OpenMP;

  Debug::getInstance ()->debugMessage (
      "Creating reduction prologue statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isReductionRequired (i))
    {
      SgBasicBlock * loopBody = buildBasicBlock ();

      /*
       * ======================================================
       * New statement
       * ======================================================
       */

      SgMultiplyOp * arrayIndexExpression = buildMultiplyOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), buildIntVal (64));

      SgPntrArrRefExp * arrayExpression = buildPntrArrRefExp (
          variableDeclarations->getReference (getReductionArrayHostName (i)),
          arrayIndexExpression);

      SgExpression * rhsExpression;

      if (isSgTypeInt (parallelLoop->getOpDatBaseType (i)))
      {
        rhsExpression = buildIntVal (0);
      }
      else if (isSgTypeFloat (parallelLoop->getOpDatBaseType (i)))
      {
        rhsExpression = buildFloatVal (0);
      }
      else if (isSgTypeDouble (parallelLoop->getOpDatBaseType (i)))
      {
        rhsExpression = buildDoubleVal (0);
      }
      else
      {
        throw Exceptions::ParallelLoop::UnsupportedBaseTypeException (
            "Reduction type not supported");
      }

      SgExprStatement * assignmentStatement = buildAssignStatement (
          arrayExpression, rhsExpression);

      appendStatement (assignmentStatement, loopBody);

      /*
       * ======================================================
       * For loop statement
       * ======================================================
       */

      SgExprStatement * initialisationExpression = buildAssignStatement (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), buildIntVal (0));

      SgLessThanOp * upperBoundExpression = buildLessThanOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)), variableDeclarations->getReference (numberOfThreads));

      SgPlusPlusOp * strideExpression = buildPlusPlusOp (
          variableDeclarations->getReference (getIterationCounterVariableName (
              2)));

      SgForStatement * forLoopStatement = buildForStatement (
          initialisationExpression, buildExprStatement (upperBoundExpression),
          strideExpression, loopBody);

      appendStatement (forLoopStatement, subroutineScope);
    }
  }
}

void
CPPOpenMPHostSubroutine::createReductionDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;
  using namespace LoopVariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating local variable declarations needed for reductions",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  variableDeclarations->add (
      getIterationCounterVariableName (2),
      RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIterationCounterVariableName (2), buildIntType (), subroutineScope));

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isReductionRequired (i))
    {
      std::string const & variableName = getReductionArrayHostName (i);

      SgMultiplyOp * arrayIndexExpression = buildMultiplyOp (buildIntVal (64),
          buildIntVal (64));

      variableDeclarations->add (variableName,
          RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, buildArrayType (parallelLoop->getOpDatBaseType (i),
                  arrayIndexExpression), subroutineScope));
    }
  }
}

void
CPPOpenMPHostSubroutine::createOpenMPLocalVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace LoopVariableNames;
  using namespace OpenMP;

  Debug::getInstance ()->debugMessage (
      "Creating OpenMP local variable declarations", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  variableDeclarations->add (numberOfThreads,
      RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
          numberOfThreads, buildIntType (), subroutineScope));
}

CPPOpenMPHostSubroutine::CPPOpenMPHostSubroutine (
    SgScopeStatement * moduleScope,
    CPPOpenMPKernelSubroutine * calleeSubroutine,
    CPPParallelLoop * parallelLoop) :
  CPPHostSubroutine (moduleScope, calleeSubroutine, parallelLoop)
{
}
