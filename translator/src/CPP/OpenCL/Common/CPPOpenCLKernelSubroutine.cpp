#include "CPPOpenCLKernelSubroutine.h"
#include "CPPOpenCLUserSubroutine.h"
#include "CPPReductionSubroutines.h"
#include "RoseStatementsAndExpressionsBuilder.h"
#include "CompilerGeneratedNames.h"
#include "OpenCL.h"

void
CPPOpenCLKernelSubroutine::createReductionPrologueStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating reduction prologue statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isReductionRequired (i))
    {
      if (parallelLoop->isArray (i) || parallelLoop->isPointer (i))
      {
        SgBasicBlock * loopBody = buildBasicBlock ();

        SgPntrArrRefExp * arrayExpression = buildPntrArrRefExp (
            variableDeclarations->getReference (getOpDatLocalName (i)),
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)));

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

        SgAssignOp * initializationExpression = buildAssignOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)), buildIntVal (0));

        SgLessThanOp * upperBoundExpression = buildLessThanOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)), buildIntVal (
                parallelLoop->getOpDatDimension (i)));

        SgPlusPlusOp * strideExpression = buildPlusPlusOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)));

        SgForStatement * forStatement = buildForStatement (buildExprStatement (
            initializationExpression),
            buildExprStatement (upperBoundExpression), strideExpression,
            loopBody);

        appendStatement (forStatement, subroutineScope);
      }
      else
      {
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
            variableDeclarations->getReference (getOpDatLocalName (i)),
            rhsExpression);

        appendStatement (assignmentStatement, subroutineScope);
      }
    }
  }
}

void
CPPOpenCLKernelSubroutine::createReductionEpilogueStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace ReductionVariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating reduction epilogue statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isReductionRequired (i))
    {
      if (parallelLoop->isArray (i) || parallelLoop->isPointer (i))
      {
        SgBasicBlock * loopBody = buildBasicBlock ();

        SgPntrArrRefExp * arrayExpression1 = buildPntrArrRefExp (
            variableDeclarations->getReference (getOpDatLocalName (i)),
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)));

        SgMultiplyOp * multiplyExpression = buildMultiplyOp (
            OpenCL::getWorkGroupIDCallStatement (subroutineScope), buildIntVal (
                parallelLoop->getOpDatDimension (i)));

        SgAddOp * addExpression = buildAddOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)), multiplyExpression);

        SgPntrArrRefExp * arrayExpression2 =
            buildPntrArrRefExp (variableDeclarations->getReference (
                getReductionArrayDeviceName (i)), addExpression);

        SgAddressOfOp * addressExpression = buildAddressOfOp (arrayExpression2);

        /*
         * ======================================================
         * Reduction operation parameter
         * ======================================================
         */

        SgIntVal * reductionType;

        if (parallelLoop->isIncremented (i))
        {
          reductionType = buildIntVal (INCREMENT);
        }
        else if (parallelLoop->isMaximised (i))
        {
          reductionType = buildIntVal (MAXIMUM);
        }
        else if (parallelLoop->isMinimised (i))
        {
          reductionType = buildIntVal (MINIMUM);
        }

        ROSE_ASSERT (reductionType != NULL);

        /*
         * ======================================================
         * Create reduction function call
         * ======================================================
         */

        SgExprListExp * actualParameters = buildExprListExp (addressExpression,
            arrayExpression1, reductionType,
            variableDeclarations->getReference (
                ReductionVariableNames::getTemporaryReductionArrayName (i)));

        SgFunctionSymbol
            * reductionFunctionSymbol =
                isSgFunctionSymbol (
                    reductionSubroutines->getHeader (
                        parallelLoop->getReductionTuple (i))->get_symbol_from_symbol_table ());

        ROSE_ASSERT (reductionFunctionSymbol != NULL);

        SgFunctionCallExp * reductionFunctionCall = buildFunctionCallExp (
            reductionFunctionSymbol, actualParameters);

        appendStatement (buildExprStatement (reductionFunctionCall), loopBody);

        SgAssignOp * initializationExpression = buildAssignOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)), buildIntVal (0));

        SgLessThanOp * upperBoundExpression = buildLessThanOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)), buildIntVal (
                parallelLoop->getOpDatDimension (i)));

        SgPlusPlusOp * strideExpression = buildPlusPlusOp (
            variableDeclarations->getReference (
                getIterationCounterVariableName (1)));

        SgForStatement * forStatement = buildForStatement (buildExprStatement (
            initializationExpression),
            buildExprStatement (upperBoundExpression), strideExpression,
            loopBody);

        appendStatement (forStatement, subroutineScope);
      }
      else
      {
        SgMultiplyOp * multiplyExpression = buildMultiplyOp (
            OpenCL::getWorkGroupIDCallStatement (subroutineScope), buildIntVal (
                parallelLoop->getOpDatDimension (i)));

        SgPntrArrRefExp * arrayExpression2 =
            buildPntrArrRefExp (variableDeclarations->getReference (
                getReductionArrayDeviceName (i)), multiplyExpression);

        SgAddressOfOp * addressExpression = buildAddressOfOp (arrayExpression2);

        /*
         * ======================================================
         * Reduction operation parameter
         * ======================================================
         */

        SgIntVal * reductionType;

        if (parallelLoop->isIncremented (i))
        {
          reductionType = buildIntVal (INCREMENT);
        }
        else if (parallelLoop->isMaximised (i))
        {
          reductionType = buildIntVal (MAXIMUM);
        }
        else if (parallelLoop->isMinimised (i))
        {
          reductionType = buildIntVal (MINIMUM);
        }

        ROSE_ASSERT (reductionType != NULL);

        /*
         * ======================================================
         * Create reduction function call
         * ======================================================
         */

        SgExprListExp * actualParameters = buildExprListExp (addressExpression,
            variableDeclarations->getReference (getOpDatLocalName (i)),
            reductionType, variableDeclarations->getReference (
                ReductionVariableNames::getTemporaryReductionArrayName (i)));

        SgFunctionSymbol
            * reductionFunctionSymbol =
                isSgFunctionSymbol (
                    reductionSubroutines->getHeader (
                        parallelLoop->getReductionTuple (i))->get_symbol_from_symbol_table ());

        ROSE_ASSERT (reductionFunctionSymbol != NULL);

        SgFunctionCallExp * reductionFunctionCall = buildFunctionCallExp (
            reductionFunctionSymbol, actualParameters);

        appendStatement (buildExprStatement (reductionFunctionCall),
            subroutineScope);
      }
    }
  }
}

void
CPPOpenCLKernelSubroutine::createReductionVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating declarations needed for reduction", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isReductionRequired (i))
      {
        string const & variableName1 = getOpDatLocalName (i);

        variableDeclarations ->add (variableName1,
            RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName1, buildArrayType (parallelLoop->getOpDatBaseType (
                    i), buildIntVal (parallelLoop->getOpDatDimension (i))),
                subroutineScope));

        string const & variableName2 =
            ReductionVariableNames::getTemporaryReductionArrayName (i);

        SgVariableDeclaration * variableDeclaration2 =
            RoseStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName2, buildArrayType (parallelLoop->getOpDatBaseType (
                    i), buildIntVal (2048)), subroutineScope);

        variableDeclaration2->get_declarationModifier ().get_storageModifier ().setOpenclLocal ();

        variableDeclarations->add (variableName2, variableDeclaration2);
      }
    }
  }
}

CPPOpenCLKernelSubroutine::CPPOpenCLKernelSubroutine (
    SgScopeStatement * moduleScope, CPPOpenCLUserSubroutine * userSubroutine,
    CPPParallelLoop * parallelLoop,
    CPPReductionSubroutines * reductionSubroutines) :
  CPPKernelSubroutine (moduleScope, userSubroutine, parallelLoop)
{
  this->reductionSubroutines = reductionSubroutines;

  subroutineHeaderStatement->get_functionModifier ().setOpenclKernel ();
}
