#include <ScopedVariableDeclarations.h>
#include <Exceptions.h>
#include <rose.h>

SgVarRefExp *
ScopedVariableDeclarations::getReference (std::string const & variableName)
{
  using namespace SageBuilder;

  if (theDeclarations.count (variableName) == 0)
  {
    throw Exceptions::CodeGeneration::UnknownVariableException (
        "Unable to find '" + variableName + "' in variable declarations");
  }

  return buildVarRefExp (theDeclarations[variableName]);
}

void
ScopedVariableDeclarations::add (std::string const & variableName,
    SgVariableDeclaration * declaration)
{
  theDeclarations[variableName] = declaration;
}

ScopedVariableDeclarations::ScopedVariableDeclarations ()
{
}
