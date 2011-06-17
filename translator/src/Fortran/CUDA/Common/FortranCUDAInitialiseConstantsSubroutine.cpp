#include <boost/algorithm/string/predicate.hpp>
#include <FortranCUDAInitialiseConstantsSubroutine.h>
#include <FortranStatementsAndExpressionsBuilder.h>
#include <FortranTypesBuilder.h>
#include <ROSEHelper.h>
#include <CommonNamespaces.h>

/*
 * ======================================================
 * Private functions
 * ======================================================
 */

void
FortranCUDAInitialiseConstantsSubroutine::declareConstants ()
{
  using boost::iequals;
  using std::string;
  using std::map;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::air_const]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::air_const;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::alpha]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::alpha;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::cfl]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::cfl;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::eps]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::eps;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::gam]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::gam;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::gm1]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::gm1;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::mach]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::mach;

  constantVariableNames[IndirectLoop::Fortran::ConstantSuffixes::qinf]
      = variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::qinf;

  for (map <string, string>::const_iterator it = constantVariableNames.begin (); it
      != constantVariableNames.end (); ++it)
  {
    if (iequals (it->first, IndirectLoop::Fortran::ConstantSuffixes::qinf)
        == false)
    {
      /*
       * ======================================================
       * All the constants are double-precision floats except
       * for the 'qinf' variable
       * ======================================================
       */

      SgVariableDeclaration * variableDeclaration =
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              it->second, FortranTypesBuilder::getDoublePrecisionFloat (),
              moduleScope, 1, CONSTANT);

      variableDeclarations->add (it->second, variableDeclaration);
    }
    else
    {
      /*
       * ======================================================
       * The 'qinf' variable is an array of double-precision
       * floats
       * ======================================================
       */
      SgVariableDeclaration * variableDeclaration =
          FortranStatementsAndExpressionsBuilder::appendVariableDeclaration (
              it->second, FortranTypesBuilder::getArray_RankOne (
                  FortranTypesBuilder::getDoublePrecisionFloat (), 1, 4),
              moduleScope, 1, CONSTANT);

      variableDeclarations->add (it->second, variableDeclaration);
    }
  }
}

void
FortranCUDAInitialiseConstantsSubroutine::createStatements ()
{
  using SageBuilder::buildVarRefExp;
  using SageBuilder::buildExprStatement;
  using SageBuilder::buildAssignStatement;
  using SageBuilder::buildSubtractOp;
  using SageBuilder::buildMultiplyOp;
  using SageBuilder::buildDivideOp;
  using SageBuilder::buildAddOp;
  using SageBuilder::buildPntrArrRefExp;
  using SageBuilder::buildFloatVal;
  using SageBuilder::buildIntVal;
  using SageBuilder::buildFunctionCallExp;
  using SageBuilder::buildExprListExp;
  using SageInterface::appendStatement;

  /*
   * ======================================================
   * New statement: gam = 1.4
   * ======================================================
   */

  SgExprStatement * assignmentStatement1 =
      buildAssignStatement (buildVarRefExp (
          variableDeclarations->get (variableNamePrefix
              + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
              + IndirectLoop::Fortran::ConstantSuffixes::gam)), buildFloatVal (
          1.4));

  appendStatement (assignmentStatement1, subroutineScope);

  /*
   * ======================================================
   * New statement: gm1 = 1.4 - 1.0
   * ======================================================
   */

  SgExprStatement * assignmentStatement2 =
      buildAssignStatement (buildVarRefExp (
          variableDeclarations->get (variableNamePrefix
              + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
              + IndirectLoop::Fortran::ConstantSuffixes::gm1)), buildFloatVal (
          0.4));

  appendStatement (assignmentStatement2, subroutineScope);

  /*
   * ======================================================
   * New statement: cfl = 0.9
   * ======================================================
   */

  SgExprStatement * assignmentStatement3 =
      buildAssignStatement (buildVarRefExp (
          variableDeclarations->get (variableNamePrefix
              + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
              + IndirectLoop::Fortran::ConstantSuffixes::cfl)), buildFloatVal (
          0.9));

  appendStatement (assignmentStatement3, subroutineScope);

  /*
   * ======================================================
   * New statement: eps = 0.05
   * ======================================================
   */

  SgExprStatement * assignmentStatement4 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::eps)),
      buildFloatVal (0.05));

  appendStatement (assignmentStatement4, subroutineScope);

  /*
   * ======================================================
   * New statement: mach = 0.4
   * ======================================================
   */

  SgExprStatement * assignmentStatement5 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::mach)),
      buildFloatVal (0.4));

  appendStatement (assignmentStatement5, subroutineScope);

  /*
   * ======================================================
   * New statement: alpha = 3.0 * atan(1.0) / 45.0
   * ======================================================
   */

  SgExprStatement * assignmentStatement6 = buildAssignStatement (
      buildVarRefExp (variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::alpha)), buildFloatVal (
          0.052360));

  appendStatement (assignmentStatement6, subroutineScope);

  /*
   * ======================================================
   * New statement: qinf(1) = r
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression7 = buildPntrArrRefExp (buildVarRefExp (
      variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::qinf)), buildIntVal (1));

  SgExprStatement * assignmentStatement7 = buildAssignStatement (
      arrayExpression7, buildFloatVal (1.000000));

  appendStatement (assignmentStatement7, subroutineScope);

  /*
   * ======================================================
   * New statement: qinf(2) = r * u
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression8 = buildPntrArrRefExp (buildVarRefExp (
      variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::qinf)), buildIntVal (2));

  SgExprStatement * assignmentStatement8 = buildAssignStatement (
      arrayExpression8, buildFloatVal (0.473286));

  appendStatement (assignmentStatement8, subroutineScope);

  /*
   * ======================================================
   * New statement: qinf(3) = 0.0
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression9 = buildPntrArrRefExp (buildVarRefExp (
      variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::qinf)), buildIntVal (3));

  SgExprStatement * assignmentStatement9 = buildAssignStatement (
      arrayExpression9, buildFloatVal (0.0));

  appendStatement (assignmentStatement9, subroutineScope);

  /*
   * ======================================================
   * New statement: qinf(4) = r * e
   * ======================================================
   */

  SgPntrArrRefExp * arrayExpression10 = buildPntrArrRefExp (buildVarRefExp (
      variableDeclarations->get (variableNamePrefix
          + IndirectLoop::Fortran::ConstantSuffixes::constantsSeparator
          + IndirectLoop::Fortran::ConstantSuffixes::qinf)), buildIntVal (4));

  SgExprStatement * assignmentStatement10 = buildAssignStatement (
      arrayExpression10, buildFloatVal (2.612000));

  appendStatement (assignmentStatement10, subroutineScope);
}

void
FortranCUDAInitialiseConstantsSubroutine::createLocalVariableDeclarations ()
{

}

void
FortranCUDAInitialiseConstantsSubroutine::createFormalParameterDeclarations ()
{

}

/*
 * ======================================================
 * Public functions
 * ======================================================
 */

void
FortranCUDAInitialiseConstantsSubroutine::generateSubroutine ()
{
  using SageBuilder::buildProcedureHeaderStatement;
  using SageBuilder::buildVoidType;
  using SageInterface::appendStatement;
  using SageInterface::addTextForUnparser;

  subroutineHeaderStatement = buildProcedureHeaderStatement (
      this->subroutineName.c_str (), buildVoidType (), formalParameters,
      SgProcedureHeaderStatement::e_subroutine_subprogram_kind, moduleScope);

  appendStatement (subroutineHeaderStatement, moduleScope);

  addTextForUnparser (subroutineHeaderStatement, "attributes(host) ",
      AstUnparseAttribute::e_before);

  subroutineScope = subroutineHeaderStatement->get_definition ()->get_body ();

  createStatements ();
}

std::map <std::string, std::string>::const_iterator
FortranCUDAInitialiseConstantsSubroutine::getFirstConstantName ()
{
  return constantVariableNames.begin ();
}

std::map <std::string, std::string>::const_iterator
FortranCUDAInitialiseConstantsSubroutine::getLastConstantName ()
{
  return constantVariableNames.end ();
}

FortranCUDAInitialiseConstantsSubroutine::FortranCUDAInitialiseConstantsSubroutine (
    std::string const & subroutineName, SgScopeStatement * moduleScope) :
  Subroutine (subroutineName + "_initialiseConstants"), FortranSubroutine (),
      variableNamePrefix (subroutineName), moduleScope (moduleScope)
{
  declareConstants ();
}