


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


#pragma once
#ifndef FORTRAN_PROGRAM_DECLARATIONS_AND_DEFINITIONS_H
#define FORTRAN_PROGRAM_DECLARATIONS_AND_DEFINITIONS_H

#include <ProgramDeclarationsAndDefinitions.h>

class FortranParallelLoop;

class FortranProgramDeclarationsAndDefinitions: public ProgramDeclarationsAndDefinitions <
    SgProcedureHeaderStatement> ,
    public AstSimpleProcessing
{
  private:

    std::map <std::string, std::string> subroutineToFileName;

    std::map <std::string, std::vector <std::string> > moduleNameToSubroutines;

    std::map <std::string, std::vector <std::string> > fileNameToModuleNames;

    std::map <std::string, std::string> moduleNameToFileName;

    std::string currentModuleName;

  private:

    void
    setOpGblProperties (FortranParallelLoop * parallelLoop,
        std::string const & variableName, int opDatArgumentGroup);

    void
    setOpDatProperties (FortranParallelLoop * parallelLoop,
        std::string const & variableName, int opDatArgumentGroup);

    void
    setOpDatPropertiesGeneric ( FortranParallelLoop * parallelLoop,
        std::string const & variableName, int opDatDimension,
        SgType * opDatBaseType, int OP_DAT_ArgumentGroup);
        
    void
    setParallelLoopAccessDescriptor (FortranParallelLoop * parallelLoop,
        SgExprListExp * actualArguments, unsigned int OP_DAT_ArgumentGroup,
        unsigned int argumentPosition);

    void
    analyseParallelLoopArguments (FortranParallelLoop * parallelLoop,
        SgExprListExp * actualArguments, int numberOfOpArgs);

    virtual void
    visit (SgNode * node);

    bool
    checkModuleExists (std::string const & moduleName);

    int
    getLoopSuffixNumber (std::string const & calleeName);
    
    SgType *
    getTypeFromString (std::string const & opDataBaseTypeString,
      std::string const & variableName);

  public:

    std::vector <std::string>::const_iterator
    getFirstSubroutine (std::string const & moduleName);

    std::vector <std::string>::const_iterator
    getLastSubroutine (std::string const & moduleName);

    std::string const &
    getFileNameForModule (std::string const & moduleName);

    std::string const &
    getFileNameForSubroutine (std::string const & subroutineName);
    
    FortranProgramDeclarationsAndDefinitions (SgProject * project);
};

#endif
