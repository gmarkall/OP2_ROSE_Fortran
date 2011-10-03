#include <Debug.h>
#include <CPPOpenCLHostSubroutineIndirectLoop.h>
#include <CPPOpenCLStatementsAndExpressionsBuilder.h>
//#include <CPPTypesBuilder.h>
#include <RoseStatementsAndExpressionsBuilder.h>
#include <CommonNamespaces.h>
#include <RoseHelper.h>
#include <CPPPlan.h>

using namespace SageBuilder;
//TODO: remove
/*
 * ======================================================
 * Private functions
 * ======================================================
 */

SgStatement *
CPPOpenCLHostSubroutineIndirectLoop::createKernelFunctionCallStatement ()
{
  using SageBuilder::buildFunctionCallStmt;
  using SageBuilder::buildFunctionCallExp;
  using SageBuilder::buildExprListExp;
  using SageBuilder::buildVoidType;
  using SageBuilder::buildVarRefExp;

  SgExprListExp * actualParameters = buildExprListExp ();

  std::vector <std::pair <SgExpression *, SgExpression *> > kernelArguments;

  actualParameters->append_expression (buildVarRefExp (
      moduleDeclarations->getDimensionsVariableDeclaration ()));

  actualParameters->append_expression (buildVarRefExp (
      moduleDeclarations->getDataSizesVariableDeclaration ()));

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false
        && parallelLoop->isIndirect (i))
    {
      actualParameters->append_expression (
          buildVarRefExp (variableDeclarations->get (
              OP2::VariableNames::getOpDatDeviceName (i))));

      actualParameters->append_expression (buildVarRefExp (
          variableDeclarations->get (
              OP2::VariableNames::getLocalToGlobalMappingName (i))));
    }
  }

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIndirect (i))
    {
      actualParameters->append_expression (buildVarRefExp (
          variableDeclarations->get (
              OP2::VariableNames::getGlobalToLocalMappingName (i))));
    }
  }

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false && (parallelLoop->isDirect (
        i) || parallelLoop->isGlobal (i)))
    {
      actualParameters->append_expression (
          buildVarRefExp (variableDeclarations->get (
              OP2::VariableNames::getOpDatDeviceName (i))));
    }
  }

  actualParameters->append_expression (buildDotExp (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::plan)), buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pindSizes))));

  /*
   buildVarRefExp (
   variableDeclarations->get (PlanFunction::CPP::pindSizes)));
   */

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pindOffs)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pblkMap)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::poffset)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pnelems)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pnthrcol)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::pthrcol)));

  actualParameters->append_expression (buildVarRefExp (
      variableDeclarations->get (PlanFunction::CPP::blockOffset)));

  SgExprStatement * callStatement = buildFunctionCallStmt (kernelSubroutineName
      + "<<<" + RoseHelper::getFirstVariableName (variableDeclarations->get (
      OpenCL::CPP::blocksPerGrid)) + ", " + RoseHelper::getFirstVariableName (
      variableDeclarations->get (OpenCL::CPP::threadsPerBlock)) + ", "
      + RoseHelper::getFirstVariableName (variableDeclarations->get (
          OpenCL::CPP::sharedMemorySize)) + ">>>", buildVoidType (),
      actualParameters, subroutineScope);

  return callStatement;
}

void
CPPOpenCLHostSubroutineIndirectLoop::createPlanFunctionExecutionStatements ()
{
  using SageBuilder::buildPntrArrRefExp;
  using SageBuilder::buildIntVal;
  using SageBuilder::buildDotExp;
  using SageBuilder::buildAddOp;
  using SageBuilder::buildSubtractOp;
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildOpaqueVarRefExp;
  using SageBuilder::buildAssignOp;
  using SageBuilder::buildAssignStatement;
  using SageBuilder::buildBasicBlock;
  using SageInterface::appendStatement;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating plan function execution statements", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  SgExpression * block_offset_ref = buildVarRefExp (variableDeclarations->get (
      PlanFunction::CPP::blockOffset));
  SgExpression * col_ref = buildVarRefExp (variableDeclarations->get (
      OP2::VariableNames::colour1));
  SgExpression * Plan_ref = buildVarRefExp (variableDeclarations->get (
      PlanFunction::CPP::actualPlan));
  SgExpression * nblocks_ref = buildVarRefExp (variableDeclarations->get (
      OpenCL::CPP::blocksPerGrid));
  SgExpression * nthreads_ref = buildVarRefExp (variableDeclarations->get (
      OpenCL::CPP::threadsPerBlock));
  SgExpression * nthreadstot_ref = buildVarRefExp (variableDeclarations->get (
      OpenCL::CPP::totalThreads));
  SgExpression * nshared_ref = buildVarRefExp (variableDeclarations->get (
      OpenCL::CPP::sharedMemorySize));
  //SgExpression * reduct_size_ref = buildVarRefExp (variableDeclarations->get (ReductionSubroutine::reductionArraySize));
  //SgExpression * offset_s_ref = buildVarRefExp (variableDeclarations->get (DirectLoop::CPP::KernelSubroutine::warpScratchpadSize));
  SgExpression * OP_block_size_ref = buildVarRefExp (variableDeclarations->get (
      OpenCL::CPP::totalThreads));

  SgStatement * tempStatement = NULL;

  /*
   * ======================================================
   * block_offset = 0
   * ======================================================
   */

  tempStatement = buildAssignStatement (block_offset_ref, buildIntVal (0));

  appendStatement (tempStatement, subroutineScope);

  /*
   * ======================================================
   * Plan = op_plan_get( name, set, part_size, nargs, args, ninds, inds )
   * ======================================================
   */

  //XXX

  /* 
   * ======================================================
   * BEGIN for ( col=0; col < Plan->ncolors; col++ )
   * ======================================================
   */

  SgStatement * initialisationExpression1 = buildExprStatement (buildAssignOp (
      col_ref, buildIntVal (0)));

  SgStatement * testExpression1 = buildExprStatement (buildLessThanOp (col_ref,
      buildArrowExp (Plan_ref, buildOpaqueVarRefExp ("ncolors"))));

  SgExpression * incrementExpression1 = buildPlusPlusOp (col_ref);

  SgBasicBlock * loopBody1 = buildBasicBlock ();

  SgForStatement * forStatement1 = buildForStatement (
      initialisationExpression1, testExpression1, incrementExpression1,
      loopBody1);

  appendStatement (forStatement1, subroutineScope);

  /*
   * ======================================================
   * nthread = 128 //TODO: change in order to wrap with OP_BLOCK_SIZE (how?)
   * ======================================================
   */

  tempStatement = buildAssignStatement (nthreads_ref, OP_block_size_ref);

  appendStatement (tempStatement, loopBody1);

  /*
   * ======================================================
   * nblocks = Plan->ncolblk[col]
   * ======================================================
   */
  tempStatement = buildAssignStatement (nblocks_ref, buildPntrArrRefExp (
      buildArrowExp (Plan_ref, buildOpaqueVarRefExp ("ncolblk")), col_ref));

  appendStatement (tempStatement, loopBody1);

  /*
   * ======================================================
   * ntotthread = nblocks * nthread
   * ======================================================
   */

  tempStatement = buildAssignStatement (nthreadstot_ref, buildMultiplyOp (
      nblocks_ref, nthreads_ref));

  appendStatement (tempStatement, loopBody1);

  /* ======================================================
   * nshared = Plan->nshared
   * ======================================================
   */
  tempStatement = buildAssignStatement (nshared_ref, buildArrowExp (Plan_ref,
      buildOpaqueVarRefExp ("nshared")));

  appendStatement (tempStatement, loopBody1);

  appendStatement (createKernelFunctionCallStatement (), loopBody1);

  appendStatement (
      CPPOpenCLStatementsAndExpressionsBuilder::generateBarrierStatement (),
      loopBody1);

  /* 
   * ======================================================
   * END for ( col=0; col < plan->ncolors; col++ )
   * ======================================================
   */

  /* 
   * ======================================================
   * block_offset += nblocks
   * ======================================================
   */

  tempStatement = buildExprStatement (buildPlusAssignOp (block_offset_ref,
      nblocks_ref));

  appendStatement (tempStatement, subroutineScope);

}

void
CPPOpenCLHostSubroutineIndirectLoop::createVariablesSizesInitialisationStatements ()
{
  using SageBuilder::buildEqualityOp;
  using SageBuilder::buildBoolValExp;
  using SageBuilder::buildBasicBlock;
  using SageBuilder::buildDotExp;
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildAssignStatement;
  using SageBuilder::buildPntrArrRefExp;
  using SageBuilder::buildIntVal;
  using SageInterface::appendStatement;
  using std::string;
  using std::vector;

  Debug::getInstance ()->debugMessage (
      "Creating statements to initialise variable sizes",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgBasicBlock * ifBody = buildBasicBlock ();

  SgExprStatement * assignmentStatement1 = buildAssignStatement (
      buildVarRefExp (
          moduleDeclarations->getFirstExecutionBooleanDeclaration ()),
      buildBoolValExp (false));

  appendStatement (assignmentStatement1, ifBody);

  unsigned int countIndirectArgs = 1;

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false
        && parallelLoop->isIndirect (i))
    {
      SgVarRefExp * dataSizesReferences = buildVarRefExp (
          moduleDeclarations->getDataSizesVariableDeclaration ());

      SgVarRefExp * fieldReference = buildVarRefExp (
          dataSizesDeclaration->getFieldDeclarations ()->get (
              OP2::VariableNames::getLocalToGlobalMappingSizeName (i)));

      SgDotExp * fieldSelectionExpression = buildDotExp (dataSizesReferences,
          fieldReference);

      SgVarRefExp * pnindirect_Reference = buildVarRefExp (
          variableDeclarations->get (PlanFunction::CPP::pnindirect));

      SgPntrArrRefExp * arrayIndexExpression = buildPntrArrRefExp (
          pnindirect_Reference, buildIntVal (countIndirectArgs));

      SgExprStatement * assignmentStatement = buildAssignStatement (
          fieldSelectionExpression, arrayIndexExpression);

      appendStatement (assignmentStatement, ifBody);

      countIndirectArgs++;
    }
  }

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isIndirect (i))
    {
      SgVarRefExp * dataSizesReferences = buildVarRefExp (
          moduleDeclarations->getDataSizesVariableDeclaration ());

      SgVarRefExp * fieldReference = buildVarRefExp (
          dataSizesDeclaration->getFieldDeclarations ()->get (
              OP2::VariableNames::getGlobalToLocalMappingSizeName (i)));

      SgDotExp * fieldSelectionExpression = buildDotExp (dataSizesReferences,
          fieldReference);

      SgExprStatement * assignmentStatement = buildAssignStatement (
          fieldSelectionExpression, buildVarRefExp (variableDeclarations->get (
              OP2::VariableNames::getGlobalToLocalMappingSizeName (i))));

      appendStatement (assignmentStatement, ifBody);
    }
  }

  vector <string> planFunctionSizeVariables;

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pblkMapSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pindOffsSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pindSizesSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pnelemsSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pnthrcolSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::poffsetSize);

  planFunctionSizeVariables.push_back (PlanFunction::CPP::pthrcolSize);

  for (vector <string>::iterator it = planFunctionSizeVariables.begin (); it
      != planFunctionSizeVariables.end (); ++it)
  {
    SgVarRefExp * dataSizesReferences = buildVarRefExp (
        moduleDeclarations->getDataSizesVariableDeclaration ());

    SgVarRefExp * fieldReference = buildVarRefExp (
        dataSizesDeclaration->getFieldDeclarations ()->get (*it));

    SgDotExp * fieldSelectionExpression = buildDotExp (dataSizesReferences,
        fieldReference);

    SgExprStatement * assignmentStatement = buildAssignStatement (
        fieldSelectionExpression, buildVarRefExp (variableDeclarations->get (
            *it)));

    appendStatement (assignmentStatement, ifBody);
  }

  SgEqualityOp * ifGuardExpression = buildEqualityOp (buildVarRefExp (
      moduleDeclarations->getFirstExecutionBooleanDeclaration ()),
      buildBoolValExp (true));

  SgIfStmt * ifStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          ifGuardExpression, ifBody);

  appendStatement (ifStatement, subroutineScope);
}

void
CPPOpenCLHostSubroutineIndirectLoop::createExecutionPlanDeclarations ()
{
  using SageBuilder::buildPointerType;
  using SageBuilder::buildVariableDeclaration;
  using SageInterface::appendStatement;
  using std::map;
  using std::string;
  using std::vector;

  Debug::getInstance ()->debugMessage ("Creating execution plan variables",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  /*
   * ======================================================
   * Create pointer to the execution plan
   * ======================================================
   */

  variableDeclarations->add (PlanFunction::CPP::actualPlan,
      CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (
          PlanFunction::CPP::actualPlan, buildPointerType (
              buildClassDeclaration ("op_plan", subroutineScope)->get_type ()),
          subroutineScope));

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    if (parallelLoop->isDuplicateOpDat (i) == false
        && parallelLoop->isIndirect (i))
    {
      string const variableName =
          OP2::VariableNames::getLocalToGlobalMappingName (i);

      variableDeclarations->add (variableName,
          CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, buildArrayType (buildIntType ()), subroutineScope,
              1, DEVICE));
    }
  }

  /*
   * ======================================================
   * Create arrays for OP_DAT, OP_INDIRECTION,
   * OP_MAP, OP_ACCESS arguments. These arrays are filled up
   * with the actual values of the OP_DAT, OP_INDIRECTION,
   * OP_MAP, OP_ACCESS that are passed to the OP_PAR_LOOP;
   * these arrays are then given to the plan function.
   * Note, therefore, that the size of the arrays is exactly
   * the number of OP_DAT argument groups.
   * There is an additional array 'inds' storing which
   * OP_DAT arguments are accessed through an indirection
   * ======================================================
   */

  vector <string> integerArrays;

  integerArrays.push_back (PlanFunction::CPP::args);

  //fourByteIntegerArrays.push_back (PlanFunction::CPP::idxs);

  //fourByteIntegerArrays.push_back (PlanFunction::CPP::maps);

  //fourByteIntegerArrays.push_back (PlanFunction::CPP::accesses);

  integerArrays.push_back (PlanFunction::CPP::inds);

  for (vector <string>::iterator it = integerArrays.begin (); it
      != integerArrays.end (); ++it)
  {
    variableDeclarations->add (*it,
        CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (*it,
            buildArrayType (buildIntType (), buildIntVal (
                parallelLoop->getNumberOfOpDatArgumentGroups ())),
            subroutineScope));
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
      string const variableName =
          OP2::VariableNames::getGlobalToLocalMappingName (i);

      variableDeclarations->add (variableName,
          CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, buildArrayType (buildShortType ()),
              subroutineScope, 1, DEVICE));
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
      string const variableName =
          OP2::VariableNames::getGlobalToLocalMappingSizeName (i);

      variableDeclarations->add (variableName,
          CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (
              variableName, buildIntType (), subroutineScope));
    }
  }

  /*
   * ======================================================
   * The plan function fills up a 'struct' which has a
   * number of integer fields. These fields need to be accessed
   * on the CPP side, so create local variables that
   * enable the data to be transferred accordingly
   * ======================================================
   */

  vector <string> integerVariables;

  integerVariables.push_back (OP2::VariableNames::colour1);

  integerVariables.push_back (CommonVariableNames::iterationCounter1);

  integerVariables.push_back (PlanFunction::CPP::argsNumber);

  integerVariables.push_back (PlanFunction::CPP::indsNumber);

  integerVariables.push_back (PlanFunction::CPP::blockOffset);

  integerVariables.push_back (PlanFunction::CPP::pindSizesSize);

  integerVariables.push_back (PlanFunction::CPP::pindOffsSize);

  integerVariables.push_back (PlanFunction::CPP::pblkMapSize);

  integerVariables.push_back (PlanFunction::CPP::poffsetSize);

  integerVariables.push_back (PlanFunction::CPP::pnelemsSize);

  integerVariables.push_back (PlanFunction::CPP::pnthrcolSize);

  integerVariables.push_back (PlanFunction::CPP::pthrcolSize);

  for (vector <string>::iterator it = integerVariables.begin (); it
      != integerVariables.end (); ++it)
  {
    variableDeclarations->add (*it,
        CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (*it,
            buildIntType (), subroutineScope));
  }

  /*
   * ======================================================
   * The plan function fills up a 'struct' which has a
   * number of integer array fields. These fields need to be
   * accessed on the CPP side ON THE HOST, so create local
   * variables that enable the data to be transferred accordingly
   * ======================================================
   */

  vector <string> integerPointerVariables;

  integerPointerVariables.push_back (PlanFunction::CPP::ncolblk);

  integerPointerVariables.push_back (PlanFunction::CPP::pnindirect);

  for (vector <string>::iterator it = integerPointerVariables.begin (); it
      != integerPointerVariables.end (); ++it)
  {
    variableDeclarations->add (*it,
        CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (*it,
            buildPointerType (buildArrayType (buildIntType ())),
            subroutineScope));
  }

#if 0
  /*
   * ======================================================
   * The plan function fills up a 'struct' which has a
   * number of array integer fields. These fields need to be accessed
   * on the CPP side ON THE DEVICE, so create local variables
   * that enable the data to be transferred accordingly
   * ======================================================
   */

  vector <string> deviceIntegerArrayVariables;

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pindSizes);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pindOffs);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pblkMap);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::poffset);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pnelems);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pnthrcol);

  deviceIntegerArrayVariables.push_back (PlanFunction::CPP::pthrcol);

  for (vector <string>::iterator it = deviceIntegerArrayVariables.begin (); it
      != deviceIntegerArrayVariables.end (); ++it)
  {
    variableDeclarations->add (*it,
        CPPStatementsAndExpressionsBuilder::appendVariableDeclaration (*it,
            buildArrayType(buildIntType()),
            subroutineScope,
            1,
            DEVICE));
  }
#endif
}

void
CPPOpenCLHostSubroutineIndirectLoop::createStatements ()
{
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildBoolValExp;
  using SageBuilder::buildEqualityOp;
  using SageInterface::appendStatement;

  SgExprStatement * tempStatement = NULL;

  SgEqualityOp * ifGuardExpression = buildEqualityOp (buildVarRefExp (
      moduleDeclarations->getFirstExecutionBooleanDeclaration ()),
      buildBoolValExp (true));

  SgBasicBlock * ifBody = buildBasicBlock (); //createFirstTimeExecutionStatements ();

  appendStatement (createPlanFunctionParametersPreparationStatements (
      (CPPParallelLoop *) parallelLoop, variableDeclarations), ifBody);

  SgFunctionCallExp * planFunctionCallExpression =
      createPlanFunctionCallExpression (subroutineScope, variableDeclarations);

  //appendStatement (createInitialiseConstantsCallStatement (), ifBody);

  SgIfStmt * ifStatement =
      RoseStatementsAndExpressionsBuilder::buildIfStatementWithEmptyElse (
          ifGuardExpression, ifBody);

  appendStatement (ifStatement, subroutineScope);

  //appendStatement (createTransferOpDatStatements (), subroutineScope);

  if (parallelLoop->isReductionRequired () == true)
  {
    createReductionPrologueStatements ();
  }

  //appendStatement (createConvertPositionInPMapsStatements (allDeclarations,
  //   (CPPParallelLoop *) parallelLoop, subroutineScope), subroutineScope);

  //appendStatement (createConvertPlanFunctionParametersStatements (
  //    allDeclarations, (CPPParallelLoop *) parallelLoop, subroutineScope),
  //    subroutineScope);

  createVariablesSizesInitialisationStatements ();

  createPlanFunctionExecutionStatements ();

  if (parallelLoop->isReductionRequired () == true)
  {
    createReductionEpilogueStatements ();
  }
}

void
CPPOpenCLHostSubroutineIndirectLoop::createLocalVariableDeclarations ()
{
  //createDataMarshallingLocalVariableDeclarations ();

  //createOpenCLKernelLocalVariableDeclarations ();

  createExecutionPlanDeclarations ();

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

CPPOpenCLHostSubroutineIndirectLoop::CPPOpenCLHostSubroutineIndirectLoop (
    std::string const & subroutineName, std::string const & userSubroutineName,
    std::string const & kernelSubroutineName, CPPParallelLoop * parallelLoop,
    SgScopeStatement * moduleScope,
    //    CPPInitialiseConstantsSubroutine * initialiseConstantsSubroutine,
    CPPOpenCLDataSizesDeclarationIndirectLoop * dataSizesDeclaration,
    CPPOpDatDimensionsDeclaration * opDatDimensionsDeclaration) :
  CPPOpenCLHostSubroutine (subroutineName, userSubroutineName,
      kernelSubroutineName, parallelLoop, moduleScope)
{
  Debug::getInstance ()->debugMessage (
      "Creating host subroutine of indirect loop", Debug::CONSTRUCTOR_LEVEL,
      __FILE__, __LINE__);

  createFormalParameterDeclarations ();

  createLocalVariableDeclarations ();

  createStatements ();
}