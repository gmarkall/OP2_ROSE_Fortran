#include <FortranOpDatDimensionsDeclaration.h>
#include <FortranTypesBuilder.h>
#include <FortranStatementsAndExpressionsBuilder.h>
#include <FortranParallelLoop.h>
#include <ScopedVariableDeclarations.h>
#include <RoseStatementsAndExpressionsBuilder.h>
#include <CommonNamespaces.h>
#include <Debug.h>
#include <boost/lexical_cast.hpp>
#include <rose.h>

/*
 * ======================================================
 * Private functions
 * ======================================================
 */

void
FortranOpDatDimensionsDeclaration::addFields ()
{
  using boost::lexical_cast;
  using SageInterface::appendStatement;
  using SageBuilder::buildVariableDeclaration;
  using std::string;

  fieldDeclarations = new ScopedVariableDeclarations ();

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    /*
     * ======================================================
     * We do NOT need a dimensions field when OP_GBL data
     * is read
     * ======================================================
     */

    if (parallelLoop->isGlobalRead (i) == false)
    {
      Debug::getInstance ()->debugMessage (
          "Adding dimensions field for OP_DAT " + lexical_cast <string> (i),
          Debug::INNER_LOOP_LEVEL, __FILE__, __LINE__);

      string const & variableName = OP2::VariableNames::getOpDatDimensionName (
          i);

      SgVariableDeclaration * fieldDeclaration = buildVariableDeclaration (
          variableName, FortranTypesBuilder::getFourByteInteger (), NULL,
          moduleScope);

      fieldDeclaration->get_declarationModifier ().get_accessModifier ().setUndefined ();

      typeStatement->get_definition ()->append_member (fieldDeclaration);

      fieldDeclarations->add (variableName, fieldDeclaration);
    }
  }
}

void
FortranOpDatDimensionsDeclaration::addTypeDeclaration ()
{
  using SageInterface::appendStatement;

  typeStatement = RoseStatementsAndExpressionsBuilder::buildTypeDeclaration (
      parallelLoop->getUserSubroutineName () + "_opDatDimensions", moduleScope);

  typeStatement->get_declarationModifier ().get_accessModifier ().setUndefined ();

  appendStatement (typeStatement, moduleScope);
}

/*
 * ======================================================
 * Public functions
 * ======================================================
 */

SgClassType *
FortranOpDatDimensionsDeclaration::getType ()
{
  return typeStatement->get_type ();
}

SgVariableDeclaration *
FortranOpDatDimensionsDeclaration::getOpDatDimensionField (
    unsigned int OP_DAT_ArgumentGroup)
{
  return fieldDeclarations->get (OP2::VariableNames::getOpDatDimensionName (
      OP_DAT_ArgumentGroup));
}

FortranOpDatDimensionsDeclaration::FortranOpDatDimensionsDeclaration (
    FortranParallelLoop * parallelLoop, SgScopeStatement * moduleScope) :
  parallelLoop (parallelLoop), moduleScope (moduleScope)
{
  Debug::getInstance ()->debugMessage (
      "Creating OP_DAT dimensions type declaration", Debug::CONSTRUCTOR_LEVEL,
      __FILE__, __LINE__);

  addTypeDeclaration ();

  addFields ();
}
