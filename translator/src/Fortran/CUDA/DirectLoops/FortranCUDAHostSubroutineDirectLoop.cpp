#include <FortranCUDAHostSubroutineDirectLoop.h>
#include <FortranTypesBuilder.h>
#include <RoseStatementsAndExpressionsBuilder.h>
#include <RoseHelper.h>
#include <Debug.h>
#include <CUDA.h>

/*
 * ======================================================
 * Private functions
 * ======================================================
 */

SgStatement *
FortranCUDAHostSubroutineDirectLoop::createKernelFunctionCallStatement ()
{
  using SageBuilder::buildDotExp;
  using SageBuilder::buildOpaqueVarRefExp;
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildFunctionCallStmt;
  using SageBuilder::buildExprListExp;
  using SageBuilder::buildVoidType;
  using SageInterface::appendStatement;

  Debug::getInstance ()->debugMessage (
      "Creating statement to call CUDA kernel", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  SgExprListExp * actualParameters = buildExprListExp ();

  actualParameters->append_expression (buildVarRefExp (
      moduleDeclarations->getDimensionsVariableDeclaration ()));

  actualParameters->append_expression (buildVarRefExp (
      moduleDeclarations->getDataSizesVariableDeclaration ()));

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->isReductionRequired (i))
      {
        actualParameters->append_expression (buildVarRefExp (
            moduleDeclarations->getReductionArrayDeviceDeclaration (i)));
      }
      else if (parallelLoop->isGlobalScalar (i))
      {
        actualParameters->append_expression (buildVarRefExp (
            variableDeclarations->get (OP2::VariableNames::getOpDatName (i))));
      }
      else
      {
        actualParameters->append_expression (buildVarRefExp (
            variableDeclarations->get (OP2::VariableNames::getOpDatDeviceName (
                i))));
      }
    }
  }

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (OP2::VariableNames::offset)));

  SgExpression * dotExpression = buildDotExp (buildVarRefExp (
      variableDeclarations->get (OP2::VariableNames::getOpSetName ())),
      buildOpaqueVarRefExp (OP2::VariableNames::size, subroutineScope));

  actualParameters->append_expression (dotExpression);

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (OP2::VariableNames::warpSize)));

  if (parallelLoop->isReductionRequired () == true)
  {
    actualParameters->append_expression (buildVarRefExp (
        variableDeclarations->get (OP2::VariableNames::offset)));
  }

  SgExprStatement * callStatement = buildFunctionCallStmt (kernelSubroutineName
      + "<<<" + RoseHelper::getFirstVariableName (variableDeclarations->get (
      CUDA::blocksPerGrid)) + ", " + RoseHelper::getFirstVariableName (
      variableDeclarations->get (CUDA::threadsPerBlock)) + ", "
      + RoseHelper::getFirstVariableName (variableDeclarations->get (
          CUDA::sharedMemorySize)) + ">>>", buildVoidType (), actualParameters,
      subroutineScope);

  return callStatement;
}

void
FortranCUDAHostSubroutineDirectLoop::createVariableSizesInitialisationStatements ()
{
  using SageBuilder::buildOpaqueVarRefExp;
  using SageBuilder::buildDotExp;
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildAssignStatement;
  using SageInterface::appendStatement;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating CUDA kernel prologue statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  /*
   * ======================================================
   * In direct loops, sizes are only related to OP_DAT
   * variables
   * ======================================================
   */

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      string const & variableName = OP2::VariableNames::getOpDatSizeName (i);

      SgDotExp * dotExpression = buildDotExp (buildVarRefExp (
          moduleDeclarations->getDataSizesVariableDeclaration ()),
          buildOpaqueVarRefExp (variableName, subroutineScope));

      /*
       * ======================================================
       * The size value changes for reduction variables
       * ======================================================
       */

      if (parallelLoop->isReductionRequired (i) == false)
      {
        SgExprStatement * assignmentStatement = buildAssignStatement (
            dotExpression, buildVarRefExp (
                dataSizesDeclaration->getFieldDeclarations ()->get (
                    variableName)));

        appendStatement (assignmentStatement, subroutineScope);
      }
      else
      {
        SgExprStatement * assignmentStatement = buildAssignStatement (
            dotExpression, buildVarRefExp (variableDeclarations->get (
                OP2::VariableNames::threadItems)));

        appendStatement (assignmentStatement, subroutineScope);
      }
    }
  }
}

void
FortranCUDAHostSubroutineDirectLoop::createCUDAKernelInitialisationStatements ()
{
  using SageBuilder::buildOpaqueVarRefExp;
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildIntVal;
  using SageBuilder::buildAssignStatement;
  using SageBuilder::buildMultiplyOp;
  using SageBuilder::buildExprListExp;
  using SageBuilder::buildFunctionCallExp;
  using SageInterface::appendStatement;
  using std::string;

  /*
   * ======================================================
   * The following values are copied from Mike Giles'
   * implementation and may be subject to future changes
   * ======================================================
   */
  int const nblocks = 200;
  int const nthreads = 128;

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (CUDA::blocksPerGrid)),
      buildIntVal (nblocks));

  appendStatement (assignmentStatement1, subroutineScope);

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (CUDA::threadsPerBlock)),
      buildIntVal (nthreads));

  appendStatement (assignmentStatement2, subroutineScope);

  SgExprStatement * assignmentStatement3 =
      buildAssignStatement (buildVarRefExp (variableDeclarations->get (
          OP2::VariableNames::warpSize)), buildOpaqueVarRefExp (
          OP2::VariableNames::warpSizeMacro, subroutineScope));

  appendStatement (assignmentStatement3, subroutineScope);

  SgExprStatement * assignmentStatement4 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (CUDA::sharedMemorySize)),
      buildIntVal (0));

  appendStatement (assignmentStatement4, subroutineScope);

  /*
   * ======================================================
   * Computing value for nshared: an input OP_DAT is copied
   * to shared memory only if its dimension is larger than
   * 1 or it is not encapsulating global data.
   * Therefore, the maximum size of shared memory is
   * equal to maximum size * dimension OP_DAT copied on
   * shared memory, multiplied by the number of threads
   * ======================================================
   */
  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false)
    {
      if (parallelLoop->getOpDatDimension (i) > 1
          && parallelLoop->getOpMapValue (i) != GLOBAL)
      {
        SgVarRefExp * parameterExpression1 = buildVarRefExp (
            variableDeclarations->get (CUDA::sharedMemorySize));

        SgExpression * parameterExpression2 = buildMultiplyOp (buildIntVal (
            parallelLoop->getOpDatDimension (i)),
            FortranStatementsAndExpressionsBuilder::getFortranKindOfOpDat (
                parallelLoop->getOpDatType (i)));

        SgExprListExp * actualParameters = buildExprListExp (
            parameterExpression1, parameterExpression2);

        SgFunctionSymbol * maxFunctionSymbol =
            FortranTypesBuilder::buildNewFortranFunction ("max",
                subroutineScope);

        SgFunctionCallExp * maxFunctionCall = buildFunctionCallExp (
            maxFunctionSymbol, actualParameters);

        SgExprStatement * assignmentStatement =
            buildAssignStatement (buildVarRefExp (variableDeclarations->get (
                CUDA::sharedMemorySize)), maxFunctionCall);

        appendStatement (assignmentStatement, subroutineScope);
      }
    }
  }

  SgMultiplyOp * multiplyExpression5 =
      buildMultiplyOp (buildVarRefExp (variableDeclarations->get (
          CUDA::sharedMemorySize)), buildOpaqueVarRefExp (
          OP2::VariableNames::warpSizeMacro, subroutineScope));

  SgExprStatement * assignmentStatement5 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (OP2::VariableNames::offset)),
      multiplyExpression5);

  appendStatement (assignmentStatement5, subroutineScope);

  SgExpression * multiplyExpression6 = buildMultiplyOp (buildVarRefExp (
      variableDeclarations->get (CUDA::sharedMemorySize)), buildVarRefExp (
      variableDeclarations->get (CUDA::threadsPerBlock)));

  SgExprStatement * assignmentStatement6 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (CUDA::sharedMemorySize)),
      multiplyExpression6);

  appendStatement (assignmentStatement6, subroutineScope);
}

void
FortranCUDAHostSubroutineDirectLoop::createCUDAKernelLocalVariableDeclarationsForDirectLoop ()
{
  using SageBuilder::buildVariableDeclaration;
  using SageBuilder::buildIntVal;
  using SageBuilder::buildIntType;
  using SageBuilder::buildAssignInitializer;
  using SageInterface::appendStatement;

  Debug::getInstance ()->debugMessage (
      "Creating CUDA configuration parameters", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  SgVariableDeclaration * variableDeclaration1 = buildVariableDeclaration (
      OP2::VariableNames::offset, FortranTypesBuilder::getFourByteInteger (),
      buildAssignInitializer (buildIntVal (0), buildIntType ()),
      subroutineScope);

  variableDeclarations->add (OP2::VariableNames::offset, variableDeclaration1);

  variableDeclaration1->get_declarationModifier ().get_accessModifier ().setUndefined ();

  appendStatement (variableDeclaration1, subroutineScope);

  SgVariableDeclaration * variableDeclaration2 = buildVariableDeclaration (
      OP2::VariableNames::warpSize, FortranTypesBuilder::getFourByteInteger (),
      buildAssignInitializer (buildIntVal (0), buildIntType ()),
      subroutineScope);

  variableDeclarations->add (OP2::VariableNames::warpSize, variableDeclaration2);

  variableDeclaration2->get_declarationModifier ().get_accessModifier ().setUndefined ();

  appendStatement (variableDeclaration2, subroutineScope);
}

void
FortranCUDAHostSubroutineDirectLoop::createStatements ()
{
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildBoolValExp;
  using SageBuilder::buildEqualityOp;
  using SageBuilder::buildAssignStatement;
  using SageInterface::appendStatement;

  SgEqualityOp * ifGuardExpression = buildEqualityOp (buildVarRefExp (
      moduleDeclarations->getFirstExecutionBooleanDeclaration ()),
      buildBoolValExp (true));

  SgBasicBlock * ifBody = createFirstTimeExecutionStatements ();

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      buildVarRefExp (
          moduleDeclarations->getFirstExecutionBooleanDeclaration ()),
      buildBoolValExp (false));

  appendStatement (assignmentStatement1, ifBody);

  SgIfStmt * ifStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          ifGuardExpression, ifBody);

  appendStatement (ifStatement, subroutineScope);

  createCUDAKernelInitialisationStatements ();

  appendStatement (createTransferOpDatStatements (), subroutineScope);

  if (parallelLoop->isReductionRequired () == true)
  {
    createReductionPrologueStatements ();
  }

  createVariableSizesInitialisationStatements ();

  appendStatement (createKernelFunctionCallStatement (), subroutineScope);

  SgExprStatement * assignmentStatement2 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (CUDA::threadSynchRet)),
      CUDA::createHostThreadSynchronisationCallStatement (subroutineScope));

  appendStatement (assignmentStatement2, subroutineScope);

  if (parallelLoop->isReductionRequired () == true)
  {
    createReductionEpilogueStatements ();
  }
}

void
FortranCUDAHostSubroutineDirectLoop::createLocalVariableDeclarations ()
{
  createDataMarshallingLocalVariableDeclarations ();

  createCUDAKernelLocalVariableDeclarations ();

  createCUDAKernelLocalVariableDeclarationsForDirectLoop ();

  if (parallelLoop->isReductionRequired () == true)
  {
    createReductionLocalVariableDeclarations ();
  }
}

/*
 * ======================================================
 * Public functions
 * ======================================================
 */

FortranCUDAHostSubroutineDirectLoop::FortranCUDAHostSubroutineDirectLoop (
    std::string const & subroutineName, std::string const & userSubroutineName,
    std::string const & kernelSubroutineName,
    FortranParallelLoop * parallelLoop, SgScopeStatement * moduleScope,
    FortranCUDADataSizesDeclarationDirectLoop * dataSizesDeclaration,
    FortranOpDatDimensionsDeclaration * opDatDimensionsDeclaration,
    FortranCUDAModuleDeclarations * moduleDeclarations) :
  FortranCUDAHostSubroutine (subroutineName, userSubroutineName,
      kernelSubroutineName, parallelLoop, moduleScope, dataSizesDeclaration,
      opDatDimensionsDeclaration, moduleDeclarations)
{
  Debug::getInstance ()->debugMessage (
      "Creating host subroutine of direct loop", Debug::CONSTRUCTOR_LEVEL,
      __FILE__, __LINE__);

  createFormalParameterDeclarations ();

  createLocalVariableDeclarations ();

  createStatements ();
}
