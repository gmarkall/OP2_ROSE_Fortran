#include <CPPModuleDeclarations.h>
#include <ScopedVariableDeclarations.h>
#include <Debug.h>
#include <CommonNamespaces.h>
#include <rose.h>

/*
 * ======================================================
 * Protected functions
 * ======================================================
 */

namespace
{
  std::string const doInitialiseConstants = "doInitialiseConstants";
}

void
CPPModuleDeclarations::createInitialiseConstantsBooleanDeclaration ()
{
  using SageBuilder::buildVariableDeclaration;
  using SageBuilder::buildBoolType;
  using SageBuilder::buildBoolValExp;
  using SageBuilder::buildAssignInitializer;
  using SageInterface::appendStatement;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating initialise constants boolean at module scope",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  SgVariableDeclaration * variableDeclaration = buildVariableDeclaration (
      doInitialiseConstants, buildBoolType (), buildAssignInitializer (
          buildBoolValExp (true), buildBoolType ()), moduleScope);

  variableDeclarations->add (doInitialiseConstants, variableDeclaration);

  variableDeclaration->get_declarationModifier ().get_accessModifier ().setUndefined ();

  appendStatement (variableDeclaration, moduleScope);
}

void
CPPModuleDeclarations::createFirstExecutionBooleanDeclaration ()
{
  using SageBuilder::buildVariableDeclaration;
  using SageBuilder::buildBoolType;
  using SageBuilder::buildBoolValExp;
  using SageBuilder::buildAssignInitializer;
  using SageInterface::appendStatement;
  using std::string;

  Debug::getInstance ()->debugMessage (
      "Creating first time execution boolean at module scope",
      Debug::FUNCTION_LEVEL, __FILE__, __LINE__);

  std::string const & variableName =
      OP2::VariableNames::getFirstTimeExecutionVariableDeclarationName (
          userSubroutineName);

  SgVariableDeclaration * variableDeclaration = buildVariableDeclaration (
      variableName, buildBoolType (), buildAssignInitializer (buildBoolValExp (
          true), buildBoolType ()), moduleScope);

  variableDeclarations->add (variableName, variableDeclaration);

  variableDeclaration->get_declarationModifier ().get_accessModifier ().setUndefined ();

  appendStatement (variableDeclaration, moduleScope);
}

CPPModuleDeclarations::CPPModuleDeclarations (
    std::string const & userSubroutineName, CPPParallelLoop * parallelLoop,
    SgScopeStatement * moduleScope) :
  userSubroutineName (userSubroutineName), parallelLoop (parallelLoop),
      moduleScope (moduleScope)
{
  variableDeclarations = new ScopedVariableDeclarations ();

  createFirstExecutionBooleanDeclaration ();

  createInitialiseConstantsBooleanDeclaration ();
}

/*
 * ======================================================
 * Public functions
 * ======================================================
 */

SgVariableDeclaration *
CPPModuleDeclarations::getInitialiseConstantsBooleanDeclaration ()
{
  return variableDeclarations->get (doInitialiseConstants);
}

SgVariableDeclaration *
CPPModuleDeclarations::getFirstExecutionBooleanDeclaration ()
{
  std::string const & variableName =
      OP2::VariableNames::getFirstTimeExecutionVariableDeclarationName (
          userSubroutineName);

  return variableDeclarations->get (variableName);
}

SgVariableDeclaration *
CPPModuleDeclarations::getGlobalOpDatDeclaration (
    unsigned int OP_DAT_ArgumentGroup)
{
  return variableDeclarations->get (OP2::VariableNames::getOpDatGlobalName (
      OP_DAT_ArgumentGroup));
}

ScopedVariableDeclarations *
CPPModuleDeclarations::getAllDeclarations ()
{
  return variableDeclarations;
}
