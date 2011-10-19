#!/usr/bin/python

import os
import sys

# Add the 'src' directory to the module search and PYTHONPATH
sys.path.append(sys.path[0] + os.sep + "src")
	
import glob
from optparse import OptionParser
from subprocess import Popen, PIPE
from Debug import Debug

# The command-line parser and its options
parser = OptionParser(add_help_option=False)

openclFlag        = "--opencl"
cudaFlag          = "--cuda"
openmpFlag        = "--openmp"
allBackendOptions = [openclFlag, cudaFlag, openmpFlag]

helpShortFlag = "-h"

parser.add_option("--clean",
                  action="store_true",
                  dest="clean",
                  help="Remove generated files. Use this flag in conjunction with one of %s to remove files generated for that particular backend." % allBackendOptions,
                  default=False)

parser.add_option("-C",
		  "--compile",
                  action="store_true",
                  dest="compile",
                  help="Runs the source-to-source compiler.",
                  default=False)

parser.add_option(cudaFlag,
                  action="store_true",
                  dest="cuda",
                  help="Generate code for CUDA backend.",
                  default=False)

parser.add_option(openclFlag,
                  action="store_true",
                  dest="opencl",
                  help="Generate code for OpenCL backend.",
                  default=False)

parser.add_option("-d",
                  "--debug",
                  action="store",
                  dest="debug",
		  type="int",
                  help="Set the debug parameter of the compiler. [Default is %default].",
                  default=0,
		  metavar="<INT>")

parser.add_option(openmpFlag,
                  action="store_true",
                  dest="openmp",
                  help="Generate code for OpenMP backend.",
                  default=False)

parser.add_option(helpShortFlag,
                  "--help",
                  action="help",
                  help="Display this help message.")

parser.add_option("-v",
                 "--verbose",
                 action="store_true",
                 dest="verbose",
                 help="Be verbose.",
                 default=False)

parser.add_option("-f",
                 "--format",
                 action="store",
		 type="int",
                 dest="format",
		 metavar="<INT>",
                 help="Format the generated code.")

parser.add_option("-M",
                 "--makefile",
                 action="store_true",
                 dest="makefile",
                 help="Generate a Makefile for the target backend.",
                 default=False)

(opts, args) = parser.parse_args(sys.argv[1:])

debug = Debug(opts.verbose)

# Cleans out files generated during the compilation process
def clean ():
	filesToRemove = []
	filesToRemove.extend(glob.glob('*_postprocessed.[fF?]*'))
	filesToRemove.extend(glob.glob('*.rmod'))
	filesToRemove.extend(glob.glob('*.mod'))
	filesToRemove.extend(glob.glob('hs_err_pid*.log'))
	filesToRemove.extend(glob.glob('~*'))

	if opts.cuda or opts.openmp:
		filesToRemove.extend(glob.glob('rose*.[fF?]*')) 

	for file in filesToRemove:
		if os.path.exists(file):
			debug.verboseMessage("Removing file: '" + file + "'") 
			os.remove(file)

def outputStdout (stdoutLines):
	print('==================================== STANDARD OUTPUT ===========================================')
	for line in stdoutLines.splitlines():
		print(line)
	print('================================================================================================')

# Runs the compiler
def compile ():
	from string import split

	allBackends       = (opts.cuda, opts.openmp, opts.opencl)
	backendsSelected  = [] 

	for backend in allBackends:
		if backend:
			backendsSelected.append(backend)

	if len(backendsSelected) == 0:
		debug.exitMessage("You must specify one of %s on the command line." % allBackendOptions)
	elif len(backendsSelected) > 1:
		debug.exitMessage("You specified multiple backends on the command line. Please only specify one of these." % backendsSelected)

	configFile = 'config'
	if not os.path.isfile(configFile):
		debug.exitMessage("Unable to find configuration file '%s' with the path to source-to-source translator and files to translate." % (configFile))

	translatorEnvVariable = 'IMPERIAL_TRANSLATOR_HOME'
	translatorHome        = split(os.environ.get(translatorEnvVariable), os.pathsep)[0]

	if translatorHome is None:
		debug.exitMessage("Unable to find the root directory of the source-to-source translator. Please set environment variable '%s'" % translatorEnvVariable)
	elif not os.path.isdir(translatorHome):
		debug.exitMessage("The source-to-source translator path '%s' is not a directory" % (translatorHome))

	translatorPath = translatorHome + os.sep + 'translator' + os.sep + 'bin' + os.sep + 'translator'
	op2Path        = translatorHome + os.sep + 'support' + os.sep + 'Fortran'

	filesToCompile = []

	for line in open(configFile, 'r'):
		line = line.strip()
		words = line.split('=')
		if line.startswith('files'):
			files = words[1].strip().split(' ')
			for f in files:
				f = f.strip()
				filesToCompile.append(f)
				if not os.path.isfile(f):
					debug.exitMessage("File '" + f + "' does not exist.")

	if not filesToCompile:
		debug.exitMessage("You did not specify which files need to compiled. Use files=<list/of/files> in the configuration file.")

	cmd = translatorPath + ' -d ' + str(opts.debug) + ' '

	if opts.cuda:
		cmd += cudaFlag + ' '
	elif opts.openmp:
		cmd += openmpFlag + ' '
	elif opts.opencl:
		cmd += openclFlag + ' '

	auxiliaryFiles = ['ISO_C_BINDING.F95', 'OP2.F95']
	for f in auxiliaryFiles:
		cmd += op2Path + os.sep + f + ' '

	for f in filesToCompile:
		cmd += f + ' '

	debug.verboseMessage("Running: '" + cmd + "'")

	# Run the compiler in a bash shell as it needs the environment variables such as
	# LD_LIBRARY_PATH
	proc = Popen(cmd,
	     	     shell=True,
             	     executable='/bin/bash',
             	     stderr=PIPE,
             	     stdout=PIPE)

	# Grab both standard output and standard error streams from the process
	stdoutLines, stderrLines = proc.communicate()

	# If a non-zero return code is detected then the compiler choked
	if proc.returncode != 0:
		print('Problem running compiler.')

		outputStdout (stdoutLines)
		
		print('==================================== STANDARD ERROR ============================================')
		lines = stderrLines.splitlines()
		print(lines[len(lines)-1])
		print('================================================================================================') 
        	exit(1)

	if opts.debug > 0:
		outputStdout (stdoutLines)

def generateBackendMakefile (files):
	from FileDependencies import getBaseFileName, determineModuleDependencies
	from Graph import Graph

	debug.verboseMessage("Generating Makefile for backend")

	# Work out the dependencies between modules 
	g = determineModuleDependencies(files)

	# Create the Makefile  
	CUDAMakefile = open("Makefile.CUDA", "w")
	   
	# Makefile variables
	CUDAMakefile.write("CC      = pgcc\n")
	CUDAMakefile.write("FC      = pgfortran\n")
	CUDAMakefile.write("LD      = $(FC)\n")
	CUDAMakefile.write("DEBUG   = -g\n")
	CUDAMakefile.write("LDFLAGS = -Mcuda=cuda3.1 -g\n")
	CUDAMakefile.write("OUT     = airfoil_cuda\n\n")

	# PHONY targets
	CUDAMakefile.write("# Phony targets\n")
	CUDAMakefile.write(".PHONY: all clean\n\n")
	CUDAMakefile.write("all: $(OUT)\n\n")
	CUDAMakefile.write("clean:\n\t")
	CUDAMakefile.write("rm -f *.o *.mod *.MOD $(OUT)\n\n")

	# Object files to be created
	CUDAMakefile.write("# C, Fortran, and CUDA Fortran object files\n")
	CUDAMakefile.write("C_OBJS       := $(patsubst %.c,%.o,$(wildcard *.c))\n")
	CUDAMakefile.write("FORTRAN_OBJS := $(patsubst %.F95,%.o,$(wildcard *.F95))\n")
	CUDAMakefile.write("CUDA_OBJS    := $(patsubst %.CUF,%.o,$(wildcard *.CUF))\n\n")

	# File suffixes to recognise
	CUDAMakefile.write("# Clear out all suffixes\n")
	CUDAMakefile.write(".SUFFIXES:\n")
	CUDAMakefile.write("# List only suffixes we use\n")
	CUDAMakefile.write(".SUFFIXES: .o .c .F95 .CUF\n\n")

	# New suffix rules to generate object files from .F95, .CUF, .c files
	CUDAMakefile.write("# Suffix rules\n")
	CUDAMakefile.write(".c.o:\n\t")
	CUDAMakefile.write("@echo \"\\n===== BUILDING OBJECT FILE $@ =====\"\n\t")
	CUDAMakefile.write("$(CC) $(DEBUG) -c $< -o $@\n\n")

	CUDAMakefile.write(".F95.o:\n\t")
	CUDAMakefile.write("@echo \"\\n===== BUILDING OBJECT FILE $@ =====\"\n\t")
	CUDAMakefile.write("$(FC) $(DEBUG) -c $< -o $@\n\n")

	CUDAMakefile.write(".CUF.o:\n\t")
	CUDAMakefile.write("@echo \"\\n===== BUILDING OBJECT FILE $@ =====\"\n\t")
	CUDAMakefile.write("$(FC) $(DEBUG) -c $< -o $@\n\n")

	# How to link object files together
	CUDAMakefile.write("# Link target\n")
	CUDAMakefile.write("$(OUT): $(C_OBJS) $(FORTRAN_OBJS) $(CUDA_OBJS)\n\t")
	CUDAMakefile.write("@echo \"\\n===== LINKING $(OUT) =====\"\n\t")
	CUDAMakefile.write("$(FC) $(LDFLAGS) $(DEBUG) $(C_OBJS) $(FORTRAN_OBJS) $(CUDA_OBJS) -o $(OUT)\n\n")

	# The Fortran and CUDA object files depend on the C object files
	CUDAMakefile.write("# Dependencies\n")
	CUDAMakefile.write("$(FORTRAN_OBJS) $(CUDA_OBJS): $(C_OBJS)\n\n")

	CUDAMakefile.write("# Per-file dependencies\n")
	# Loop through each vertex in the graph
	for v in g.getVertices():
	    dependencies = []
	    # Discover non-dummy dependencies in the graph. This is 
	    # because we currently add a dummy start vertex to the graph
	    # and connect it to every vertex without predecessors.
	    # (This is needed for the topological sort.)
	    for p in v.getPredecessors():
		if p != Graph.dummyFileName:
		    dependencies.append(p)
		            
	    if len(dependencies) > 0:
		CUDAMakefile.write(getBaseFileName(v.getFileName()) + ".o: ")
		for d in dependencies:
		    CUDAMakefile.write(getBaseFileName(d) + ".o ")
		CUDAMakefile.write("\n\n")

	CUDAMakefile.close()

	return CUDAMakefile.name

if opts.clean:
	clean()

if opts.format:
	if opts.format < 40:
		debug.exitMessage("Formatting length must be positive number greater than or equal to 40. Currently set to " + str(opts.format))

if opts.compile:
	compile()

	# The files generated by our compiler
	files = glob.glob(os.getcwd() + os.sep + "rose_*")
	
	if opts.format > 0:
		from FormatFortranCode import FormatFortranCode			
		f = FormatFortranCode (files, opts.format) 
	
	if opts.makefile:
		files.append (generateBackendMakefile (files))

if not opts.clean and not opts.compile:
	debug.exitMessage("No actions selected. Use %s for options." % helpShortFlag)
