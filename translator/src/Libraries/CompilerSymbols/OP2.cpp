


/*  Open source copyright declaration based on BSD open source template:
 *  http://www.opensource.org/licenses/bsd-license.php
 * 
 * Copyright (c) 2011-2012, Adam Betts, Carlo Bertolli
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <OP2.h>
#include <rose.h>

SgFunctionCallExp *
OP2::Macros::createRoundUpCallStatement (SgScopeStatement * scope,
    SgExpression * parameterExpression)
{
  using namespace SageBuilder;
  using std::string;

  SgExprListExp * actualParameters = buildExprListExp (parameterExpression);

  string const functionName = "ROUND_UP";

  return buildFunctionCallExp (functionName, buildVoidType (),
      actualParameters, scope);
}

SgFunctionCallExp *
OP2::Macros::createMaxCallStatement (SgScopeStatement * scope,
    SgExpression * parameterExpression1, SgExpression * parameterExpression2)
{
  using namespace SageBuilder;
  using std::string;

  SgExprListExp * actualParameters = buildExprListExp (parameterExpression1,
      parameterExpression2);

  string const functionName = "MAX";

  return buildFunctionCallExp (functionName, buildVoidType (),
      actualParameters, scope);
}

SgFunctionCallExp *
OP2::Macros::createMinCallStatement (SgScopeStatement * scope,
    SgExpression * parameterExpression1, SgExpression * parameterExpression2)
{
  using namespace SageBuilder;
  using std::string;

  SgExprListExp * actualParameters = buildExprListExp (parameterExpression1,
      parameterExpression2);

  string const functionName = "MIN";

  return buildFunctionCallExp (functionName, buildVoidType (),
      actualParameters, scope);
}


std::string const
OP2::PreprocessorDirectives::getIfPrintOpDatDirectiveString ()
{
  return "\n#ifdef PRINT_OUTPUT_DAT\n";
}

std::string const
OP2::PreprocessorDirectives::getEndIfPrintOpDatDirectiveString ()
{
  return "\n#endif \n";
}


