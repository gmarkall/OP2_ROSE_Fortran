#include "FortranOpenMPKernelSubroutineIndirectLoop.h"
#include "FortranParallelLoop.h"
#include "FortranStatementsAndExpressionsBuilder.h"
#include "FortranTypesBuilder.h"
#include "CompilerGeneratedNames.h"
#include "PlanFunctionNames.h"
#include "RoseHelper.h"
#include "Debug.h"
#include "OpenMP.h"

SgStatement *
FortranOpenMPKernelSubroutineIndirectLoop::createUserSubroutineCallStatement ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace LoopVariableNames;

  Debug::getInstance ()->debugMessage ("Creating call to user kernel",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgExprListExp * actualParameters = buildExprListExp ();

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isReductionRequired (i) == false)
      {
        if (parallelLoop->isIncremented (i))
        {
          actualParameters->append_expression (
              variableDeclarations->getReference (getOpDatLocalName (i)));
        }
        else
        {
        }
      }
      else
      {
        actualParameters->append_expression (
            variableDeclarations->getReference (getOpDatName (i)));
      }
    }
  }

  SgFunctionSymbol * userSubroutineSymbol =
      FortranTypesBuilder::buildNewFortranSubroutine (
          userSubroutine->getSubroutineName (), subroutineScope);

  SgFunctionCallExp * userSubroutineCall = buildFunctionCallExp (
      userSubroutineSymbol, actualParameters);

  return buildExprStatement (userSubroutineCall);
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createExecutionLoopStatements ()
{

}

void
FortranOpenMPKernelSubroutineIndirectLoop::createInitialiseIncrementAccessVariablesStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace PlanFunctionVariableNames;
  using namespace OP2VariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating statements to initialse thread-specific variables needed for incremented OP_DATs",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Statement to initialise ceiling of number of active
   * threads
   * ======================================================
   */

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      variableDeclarations->getReference (numberOfActiveThreadsCeiling),
      variableDeclarations->getReference (numberOfActiveThreads));

  appendStatement (assignmentStatement1, subroutineScope);

  /*
   * ======================================================
   * Statement to initialise number of colours
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression2 = buildPntrArrRefExp (
      variableDeclarations->getReference (
          getNumberOfThreadColoursPerBlockArrayName ()),
      variableDeclarations->getReference (OpenMP::threadBlockID));

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      variableDeclarations->getReference (numberOfColours), arrayExpression2);

  appendStatement (assignmentStatement2, subroutineScope);
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createInitialiseThreadVariablesStatements ()
{
  using namespace SageBuilder;
  using namespace SageInterface;
  using namespace PlanFunctionVariableNames;
  using namespace OP2VariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating statements to initialse thread-specific variables",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Statement to initialise thread-block ID
   * ======================================================
   */

  SgAddOp * addExpression1 = buildAddOp (variableDeclarations->getReference (
      blockID), variableDeclarations->getReference (blockOffset));

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      variableDeclarations->getReference (OpenMP::threadBlockID),
      addExpression1);

  appendStatement (assignmentStatement1, subroutineScope);

  /*
   * ======================================================
   * Statement to initialise number of active threads
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression2 = buildPntrArrRefExp (
      variableDeclarations->getReference (
          getNumberOfSetElementsPerBlockArrayName ()),
      variableDeclarations->getReference (OpenMP::threadBlockID));

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      variableDeclarations->getReference (numberOfActiveThreads),
      arrayExpression2);

  appendStatement (assignmentStatement2, subroutineScope);

  /*
   * ======================================================
   * Statement to initialise offset into shared memory
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression3 = buildPntrArrRefExp (
      variableDeclarations->getReference (getOffsetIntoBlockSizeName ()),
      variableDeclarations->getReference (OpenMP::threadBlockID));

  SgExprStatement * assignmentStatement3 = buildAssignStatement (
      variableDeclarations->getReference (OpenMP::threadBlockOffset),
      arrayExpression3);

  appendStatement (assignmentStatement3, subroutineScope);
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createStatements ()
{
  using namespace SageInterface;

  Debug::getInstance ()->debugMessage ("Creating statements",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  createInitialiseThreadVariablesStatements ();

  if (parallelLoop->hasIncrementedOpDats ())
  {
    createInitialiseIncrementAccessVariablesStatements ();
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createIncrementAccessLocalVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace PlanFunctionVariableNames;
  using boost::lexical_cast;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating local variable declarations needed for incremented OP_DATS",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIncremented (i))
    {
      Debug::getInstance ()->debugMessage (
          "Creating local variables for incremented OP_DAT " + lexical_cast <
              string> (i), Debug::INNER_LOOP_LEVEL, __FILE__, __LINE__);

      string const variableName1 = getOpDatLocalName (i);

      variableDeclarations->add (variableName1,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName1,
              FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
                  parallelLoop->getOpDatBaseType (i), buildIntVal (0),
                  buildIntVal (parallelLoop->getOpDatDimension (i) - 1)),
              subroutineScope));

      string const variableName2 = getIncrementAccessMapName (i);

      variableDeclarations->add (variableName2,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName2, FortranTypesBuilder::getFourByteInteger (),
              subroutineScope));
    }
  }

  variableDeclarations->add (numberOfColours,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          numberOfColours, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope));

  variableDeclarations->add (numberOfActiveThreadsCeiling,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          numberOfActiveThreadsCeiling,
          FortranTypesBuilder::getFourByteInteger (), subroutineScope));

  variableDeclarations ->add (colour1,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          colour1, FortranTypesBuilder::getFourByteInteger (), subroutineScope));

  variableDeclarations ->add (colour2,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          colour2, FortranTypesBuilder::getFourByteInteger (), subroutineScope));
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createSharedVariableDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using std::find;
  using std::vector;
  using std::string;

  Debug::getInstance ()->debugMessage ("Creating shared variable declarations",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  vector <string> autosharedNames;

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isIndirect (i))
      {
        string const & variableName = getIndirectOpDatSharedMemoryName (i);

        variableDeclarations->add (variableName,
            FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName, buildPointerType (
                    FortranTypesBuilder::getArray_RankOne (
                        parallelLoop->getOpDatBaseType (i))), subroutineScope));

        string const & sharedVariableName = getSharedMemoryDeclarationName (
            parallelLoop->getOpDatBaseType (i),
            parallelLoop->getSizeOfOpDat (i));

        if (find (autosharedNames.begin (), autosharedNames.end (),
            sharedVariableName) == autosharedNames.end ())
        {
          Debug::getInstance ()->debugMessage (
              "Creating declaration with name '" + sharedVariableName
                  + "' for OP_DAT '" + parallelLoop->getOpDatVariableName (i)
                  + "'", Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

          SgSubtractOp * upperBound = buildSubtractOp (buildIntVal (8000),
              buildIntVal (1));

          variableDeclarations->add (
              sharedVariableName,
              FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                  sharedVariableName, FortranTypesBuilder::getArray_RankOne (
                      parallelLoop->getOpDatBaseType (i), 0, upperBound),
                  subroutineScope, 1, TARGET));

          autosharedNames.push_back (sharedVariableName);
        }
      }
    }
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createExecutionLocalVariableDeclarations ()
{
  using namespace LoopVariableNames;
  using namespace OP2VariableNames;
  using namespace PlanFunctionVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating local variable declarations needed to execute kernel",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  variableDeclarations->add (OpenMP::threadBlockOffset,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          OpenMP::threadBlockOffset,
          FortranTypesBuilder::getFourByteInteger (), subroutineScope));

  variableDeclarations->add (OpenMP::threadBlockID,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          OpenMP::threadBlockID, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope));

  variableDeclarations->add (numberOfActiveThreads,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          numberOfActiveThreads, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope));

  variableDeclarations ->add (getIterationCounterVariableName (1),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIterationCounterVariableName (1),
          FortranTypesBuilder::getFourByteInteger (), subroutineScope));

  variableDeclarations ->add (getIterationCounterVariableName (2),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
          getIterationCounterVariableName (2),
          FortranTypesBuilder::getFourByteInteger (), subroutineScope));
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createIndirectOpDatCardinalityLocalVariableDeclarations ()
{
  using namespace PlanFunctionVariableNames;
  using boost::lexical_cast;
  using std::string;

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isIndirect (i))
      {
        Debug::getInstance ()->debugMessage (
            "Creating cardinality argument for indirect OP_DAT "
                + lexical_cast <string> (i), Debug::INNER_LOOP_LEVEL, __FILE__,
            __LINE__);

        string const variableName = getIndirectOpDatCardinalityName (i);

        variableDeclarations->add (variableName,
            FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName, FortranTypesBuilder::getFourByteInteger (),
                subroutineScope));
      }
    }
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createRoundUpLocalVariableDeclarations ()
{
  using namespace OP2VariableNames;
  using boost::lexical_cast;
  using std::string;

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isIndirect (i))
      {
        Debug::getInstance ()->debugMessage ("Creating round up declaration "
            + lexical_cast <string> (i), Debug::INNER_LOOP_LEVEL, __FILE__,
            __LINE__);

        string const & variableName = getRoundUpVariableName (i);

        variableDeclarations->add (variableName,
            FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName, FortranTypesBuilder::getFourByteInteger (),
                subroutineScope));
      }
    }
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createNumberOfBytesPerOpDatLocalVariableDeclarations ()
{
  using namespace OP2VariableNames;
  using boost::lexical_cast;
  using std::string;

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    string const & variableName = getNumberOfBytesVariableName (i);

    if (parallelLoop->isIndirect (i))
    {
      if (parallelLoop->isDuplicateOpDat (i) == false)
      {
        Debug::getInstance ()->debugMessage (
            "Creating number of bytes declaration for OP_DAT " + lexical_cast <
                string> (i), Debug::INNER_LOOP_LEVEL, __FILE__, __LINE__);

        SgVariableDeclaration * variableDeclaration =
            FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
                variableName, FortranTypesBuilder::getFourByteInteger (),
                subroutineScope);

        variableDeclarations->add (variableName, variableDeclaration);

        numberOfBytesDeclarations[parallelLoop->getOpDatVariableName (i)]
            = variableDeclaration;
      }
      else
      {
        Debug::getInstance ()->debugMessage (
            "Number of bytes declaration NOT needed for OP_DAT "
                + lexical_cast <string> (i), Debug::INNER_LOOP_LEVEL, __FILE__,
            __LINE__);

        variableDeclarations ->add (variableName,
            numberOfBytesDeclarations[parallelLoop->getOpDatVariableName (i)]);
      }
    }
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createLocalVariableDeclarations ()
{
  Debug::getInstance ()->debugMessage ("Creating local variable declarations",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  createExecutionLocalVariableDeclarations ();

  createSharedVariableDeclarations ();

  createNumberOfBytesPerOpDatLocalVariableDeclarations ();

  createRoundUpLocalVariableDeclarations ();

  createIndirectOpDatCardinalityLocalVariableDeclarations ();

  if (parallelLoop->hasIncrementedOpDats ())
  {
    createIncrementAccessLocalVariableDeclarations ();
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createPlanFormalParameterDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace PlanFunctionVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating plan function formal parameter declarations",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgAsteriskShapeExp * upperBoundExpression = new SgAsteriskShapeExp (
      RoseHelper::getFileInfo ());

  variableDeclarations->add (
      getIndirectOpDatsNumberOfElementsArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getIndirectOpDatsNumberOfElementsArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getIndirectOpDatsOffsetArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getIndirectOpDatsOffsetArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getColourToBlockArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getColourToBlockArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getOffsetIntoBlockSizeName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getOffsetIntoBlockSizeName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getNumberOfSetElementsPerBlockArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getNumberOfSetElementsPerBlockArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getNumberOfThreadColoursPerBlockArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getNumberOfThreadColoursPerBlockArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      getThreadColourArrayName (),
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          getThreadColourArrayName (),
          FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
              FortranTypesBuilder::getFourByteInteger (), buildIntVal (0),
              upperBoundExpression), subroutineScope, formalParameters));

  variableDeclarations->add (
      blockOffset,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          blockOffset, FortranTypesBuilder::getFourByteInteger (),
          subroutineScope, formalParameters));

  variableDeclarations->add (
      blockID,
      FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          blockID, FortranTypesBuilder::getFourByteInteger (), subroutineScope,
          formalParameters));
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createOpDatFormalParameterDeclarations ()
{
  using namespace SageBuilder;
  using namespace OP2VariableNames;
  using namespace PlanFunctionVariableNames;
  using std::string;

  Debug::getInstance ()->debugMessage ("Creating OP_DAT formal parameters",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      string const & variableName = getOpDatName (i);

      SgAsteriskShapeExp * upperBoundExpression = new SgAsteriskShapeExp (
          RoseHelper::getFileInfo ());

      variableDeclarations->add (
          variableName,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
              variableName,
              FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
                  parallelLoop->getOpDatBaseType (i), buildIntVal (0),
                  upperBoundExpression), subroutineScope, formalParameters));
    }
  }

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isIndirect (i))
      {
        string const & variableName = getLocalToGlobalMappingName (i);

        SgAsteriskShapeExp * upperBoundExpression = new SgAsteriskShapeExp (
            RoseHelper::getFileInfo ());

        variableDeclarations->add (
            variableName,
            FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
                variableName,
                FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
                    FortranTypesBuilder::getFourByteInteger (),
                    buildIntVal (0), upperBoundExpression), subroutineScope,
                formalParameters));
      }
    }
  }

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIndirect (i))
    {
      string const & variableName = getGlobalToLocalMappingName (i);

      SgAsteriskShapeExp * upperBoundExpression = new SgAsteriskShapeExp (
          RoseHelper::getFileInfo ());

      variableDeclarations->add (
          variableName,
          FortranStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
              variableName,
              FortranTypesBuilder::getArray_RankOne_WithLowerAndUpperBounds (
                  FortranTypesBuilder::getTwoByteInteger (), buildIntVal (0),
                  upperBoundExpression), subroutineScope, formalParameters));
    }
  }
}

void
FortranOpenMPKernelSubroutineIndirectLoop::createFormalParameterDeclarations ()
{
  using namespace PlanFunctionVariableNames;

  Debug::getInstance ()->debugMessage (
      "Creating formal parameter declarations", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  createOpDatFormalParameterDeclarations ();

  createPlanFormalParameterDeclarations ();
}

FortranOpenMPKernelSubroutineIndirectLoop::FortranOpenMPKernelSubroutineIndirectLoop (
    SgScopeStatement * moduleScope,
    Subroutine <SgProcedureHeaderStatement> * userSubroutine,
    FortranParallelLoop * parallelLoop) :
  FortranOpenMPKernelSubroutine (moduleScope, userSubroutine, parallelLoop)
{
  Debug::getInstance ()->debugMessage (
      "OpenMP kernel subroutine creation for indirect loop",
      Debug::CONSTRUCTOR_LEVEL, __FILE__, __LINE__);

  createFormalParameterDeclarations ();

  createLocalVariableDeclarations ();

  createStatements ();
}
