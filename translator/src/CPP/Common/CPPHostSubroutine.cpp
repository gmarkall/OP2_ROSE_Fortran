#include <CPPHostSubroutine.h>
#include <CPPOpenCLReductionSubroutine.h>

void
CPPHostSubroutine::createFormalParameterDeclarations ()
{
  using boost::iequals;
  using SageBuilder::buildVariableDeclaration;
  using SageBuilder::buildIntType;
  using SageInterface::appendStatement;
  using std::vector;
  using std::string;
  using std::map;

  Debug::getInstance ()->debugMessage (
      "Creating host subroutine formal parameters", Debug::FUNCTION_LEVEL,
      __FILE__, __LINE__);

  /*
   * ======================================================
   * Add name of user kernel
   * ======================================================
   */

  string const & kernelVariableName =
      OP2::VariableNames::getUserSubroutineName ();

  variableDeclarations->add (
      kernelVariableName,
      RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          kernelVariableName, buildPointerType (buildCharType ()),
          subroutineScope, formalParameters));

  /*
   * ======================================================
   * Add OP_SET
   * ======================================================
   */

  string const & opSetVariableName = OP2::VariableNames::getOpSetName ();

  variableDeclarations->add (
      opSetVariableName,
      RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
          opSetVariableName, FortranTypesBuilder::buildClassDeclaration (
              OP2::OP_SET, subroutineScope)->get_type (), subroutineScope,
          formalParameters, 1, INTENT_IN));

  /*
   * ======================================================
   * Add OP_DAT, indirection, OP_MAP, access arguments for
   * each OP_DAT argument group
   * ======================================================
   */

  for (unsigned int i = 1; i <= parallelLoop->getNumberOfOpDatArgumentGroups (); ++i)
  {
    string const & opDatvariableName = OP2::VariableNames::getOpDatName (i);

    variableDeclarations->add (
        opDatvariableName,
        RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
            opDatvariableName, FortranTypesBuilder::buildClassDeclaration (
                OP2::OP_DAT, subroutineScope)->get_type (), subroutineScope,
            formalParameters));

    string const & indirectionVariableName =
        OP2::VariableNames::getOpIndirectionName (i);

    variableDeclarations->add (
        indirectionVariableName,
        RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
            indirectionVariableName,
            FortranTypesBuilder::getFourByteInteger (), subroutineScope,
            formalParameters));

    string const & opMapVariableName = OP2::VariableNames::getOpMapName (i);

    variableDeclarations->add (
        opMapVariableName,
        RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
            opMapVariableName, FortranTypesBuilder::buildClassDeclaration (
                OP2::OP_MAP, subroutineScope)->get_type (), subroutineScope,
            formalParameters));

    string const & accessVariableName = OP2::VariableNames::getOpAccessName (i);

    variableDeclarations->add (
        accessVariableName,
        RoseStatementsAndExpressionsBuilder::appendVariableDeclarationAsFormalParameter (
            accessVariableName, FortranTypesBuilder::getFourByteInteger (),
            subroutineScope, formalParameters));
  }
}

CPPHostSubroutine::CPPHostSubroutine (std::string const & subroutineName,
    std::string const & userSubroutineName,
    std::string const & kernelSubroutineName, CPPParallelLoop * parallelLoop,
    SgScopeStatement * moduleScope) :
  HostSubroutine <SgFunctionDeclaration> (subroutineName, userSubroutineName,
      kernelSubroutineName, parallelLoop)
{
  using SageBuilder::buildVoidType;
  using SageBuilder::buildDefiningFunctionDeclaration;
  using SageInterface::appendStatement;

  subroutineHeaderStatement = buildDefiningFunctionDeclaration (
      this->subroutineName.c_str (), buildVoidType (), formalParameters,
      moduleScope);

  subroutineScope = subroutineHeaderStatement->get_definition ()->get_body ();

  appendStatement (subroutineHeaderStatement, moduleScope);
}
