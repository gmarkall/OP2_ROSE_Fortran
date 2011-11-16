import os
import sys	

# Add the 'src' directory to the module search and PYTHONPATH
sys.path.append(sys.path[0] + os.sep + "src")

from optparse import OptionParser
from Debug import Debug

# Global variables
parser                  = OptionParser(add_help_option=False)
helpShortFlag           = "-h"
generatedFilesDirectory = "generatedFiles"

# Add options to the command line
parser.add_option("--clean",
                  action="store_true",
                  dest="clean",
                  help="Remove generated files.",
                  default=False)

parser.add_option("--run",
                  action="store_true",
                  dest="run",
                  help="Run the tests.",
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

parser.add_option("-T",
		  "--tests",
		  dest="tests",
		  action="append",
		  help="Run only this test number. (To run multiple tests, specify this option multiple times.)")

# Parse command-line options
(opts, args) = parser.parse_args(sys.argv[1:])

# Add debug support
debug = Debug(opts.verbose)

def testInteger(message, data):
	try:
		int(data)
	except ValueError:
		debug.exitMessage(message)

# Ensure the test numbers supplied by the user are indeed integers
if opts.tests:
	for testNum in opts.tests:
		testInteger("Supplied test number '%s' is not an integer" % (testNum), testNum)

# Cleans out files generated during the compilation process
def clean ():
	from glob import glob
	filesToRemove = []
	filesToRemove.extend(glob('*_postprocessed.[fF?]*'))
	filesToRemove.extend(glob('*.rmod'))
	filesToRemove.extend(glob('*.mod'))
	filesToRemove.extend(glob('hs_err_pid*.log'))
	filesToRemove.extend(glob('~*'))
	filesToRemove.extend(glob('rose*.[fF?]*'))
	filesToRemove.extend(glob('rose*.CUF'))
	filesToRemove.append('testReport.txt')

	for file in filesToRemove:
		if os.path.exists(file):
			debug.verboseMessage("Removing file: '" + file + "'")
			os.remove(file)

def generateCUDAMakefile (generatedFiles, testNum):
	from FileDependencies import FortranDependencies
	from Graph import Graph

	debug.verboseMessage("Generating Makefile for CUDA")

	CObjectFiles        = ['op_support.o', 'op_seq.o', 'debug.o']
	fortranSupportFiles = ['cudaConfigurationParams.F95', 'op2_c.F95']
	op2Directory        = "OP2_DIR"
	linkTarget          = "link"
	fortranTarget       = "fortranFiles"
	executableName      = "Test" + testNum

	# Work out the dependencies between modules 
	topSort = FortranDependencies(generatedFiles).getDependencyGraph().getTopologicalSort()

	# Create the Makefile  
	CUDAMakefile = open("Makefile." + testNum, "w")
	   
	# Makefile variables
	CUDAMakefile.write("FC      = pgfortran\n")
	CUDAMakefile.write("FC_OPT  = -Mcuda=cuda3.1 -fast -O2 -Mcuda=ptxinfo -Minform=inform -C -Mchkptr\n")
	CUDAMakefile.write("OUT     = %s\n" % (executableName))
	CUDAMakefile.write("%s = \n\n" % (op2Directory))

	# PHONY targets
	CUDAMakefile.write("# Phony targets\n")
	CUDAMakefile.write(".PHONY: all clean\n\n")
	CUDAMakefile.write("all: %s %s\n\n" % (fortranTarget, linkTarget))
	CUDAMakefile.write("clean:\n\t")
	CUDAMakefile.write("rm -f *.o *.mod *.MOD $(OUT)\n\n")

	# Fortran target
	fortranLine = ""
	for f in fortranSupportFiles:
		fortranLine += f + " "
	for f in topSort:
		fortranLine += os.path.basename(f) + " "

	CUDAMakefile.write(fortranTarget + ": ")
	CUDAMakefile.write(fortranLine + "\n\t")
	CUDAMakefile.write("$(FC) $(FC_OPT) -c ")
	CUDAMakefile.write(fortranLine + "\n\n")

	# Link target
	linkLine = ""	
	for f in CObjectFiles:
		linkLine += "$(%s)/%s " % (op2Directory, f)
	for f in fortranSupportFiles:
		linkLine += f[:-4] + ".o "
	for f in topSort:
		basename = os.path.basename(f)
		linkLine += basename[:-4] + ".o "

	CUDAMakefile.write(linkTarget + ": ")
	CUDAMakefile.write(linkLine + "\n\t")
	CUDAMakefile.write("$(FC) $(FC_OPT) ")
	CUDAMakefile.write(linkLine + "-o " + executableName + "\n\n")

	CUDAMakefile.close()

	return CUDAMakefile.name

def manageNewFiles (timeStamp, testNum):
	from re import compile, IGNORECASE
	from FormatFortranCode import FormatFortranCode
	from glob import glob
	from shutil import move

	cuda_regex = compile(".*cuda.*", IGNORECASE)

	# The files generated by our compiler
	newFiles     = []
	FortranFiles = glob("*.F95")
	for f in FortranFiles:
		if os.path.getctime(f) > timeStamp:
			debug.verboseMessage("File '%s' has been created" % f)
			newFiles.append(f)

	f = FormatFortranCode (newFiles) 

	if not os.path.exists(generatedFilesDirectory):
		os.makedirs(generatedFilesDirectory)

	renamedFiles = []
	j = 0
	for f in newFiles:
		j = j + 1
		destName = generatedFilesDirectory + os.sep + "Test" + testNum + "File" + str(j)
		if cuda_regex.match(f):
			debug.verboseMessage("CUDA file detected. Changing extension.")
			destName += ".CUF"
		else:
			destName += f[-4:]
		debug.verboseMessage("Keeping file '%s'" % (destName))
		move(f, destName)
		renamedFiles.append(destName)

	makefile = generateCUDAMakefile(renamedFiles, testNum)
	destName = generatedFilesDirectory + os.sep + makefile
	debug.verboseMessage("Generating Makefile '%s'" % (destName))				
	os.rename(makefile, destName)

def checkEnvironment ():	
	from string import split

	translatorEnvVariable = 'IMPERIAL_TRANSLATOR_HOME'
	translatorHome        = split(os.environ.get(translatorEnvVariable) or '', os.pathsep)[0]

	if not translatorHome:
		debug.exitMessage("Unable to find the root directory of the source-to-source translator. Please set environment variable '%s'" % translatorEnvVariable)
	elif not os.path.isdir(translatorHome):
		debug.exitMessage("The source-to-source translator path '%s' is not a directory" % (translatorHome))

	translatorPath = translatorHome + os.sep + 'translator' + os.sep + 'bin' + os.sep + 'translator'
	op2Path        = translatorHome + os.sep + 'support' + os.sep + 'Fortran'

	if not os.path.isfile(translatorPath):
		debug.exitMessage("Unable to find the translator binary '" + translatorPath + "'. (Check the git repository directory structure has not changed. If so, modify this script!)")
	if not os.path.isdir(op2Path):
		debug.exitMessage("Unable to find the directory '" + op2Path + "' needed to compile Fortran programs using our translator. (Check the git repository directory structure has not changed. If so, modify this script!)")

	return translatorPath, op2Path

# Writes out a report for failed tests:
def writeTestReport (testReportLines):
	if testReportLines:
		testReport = open("testReport.txt", "w")
		for line in testReportLines:
			testReport.write(line)
		testReport.close()

# Runs the compiler
def runTests ():
	from subprocess import Popen, PIPE
	from time import time
	
	translatorPath, op2Path = checkEnvironment ()

	testsFile = 'testsToRun'
	if not os.path.isfile(testsFile):
		debug.exitMessage("Unable to find file containing tests to run. It should be called '%s'" % (testsFile))

	testReportLines  = []
	for line in open(testsFile, 'r'):
		tokens     = line.split(' ')
		testNum    = tokens[0].strip()
		testResult = int(tokens[1].strip())	
		run        = False

		testInteger("The test number '%s' in the file '%s' is not an integer" % (testNum, testsFile), testNum)

		if opts.tests:
			# If the user has specified to run certain tests then only run those
			if testNum in opts.tests:
				run = True
		else:
			# Otherwise run everything
			run = True
				
		if run:
			debug.verboseMessage("========== Test %s ==========" % (testNum))

			testFiles = []
			for i in range(2, len(tokens)):
				# Tokens 0 and 1 are used above. Source files should start at token 2 (this is checked)
				f = tokens[i].strip()
				if not os.path.isfile(f):
					debug.exitMessage("File '" + f + "' does not exist")	
				else:
					testFiles.append(f)	

			filesToCompile = ['ISO_C_BINDING.F95', 'OP2.F95']

			cmd = translatorPath + ' --CUDA '
			for f in filesToCompile:
				cmd += op2Path + os.sep + f + ' '
			cmd += 'UserKernels.F95 '
			cmd += ''.join(testFiles)

			debug.verboseMessage("Running: '" + cmd + "'")

			# Run the compiler in a bash shell as it needs the environment variables such as
			# LD_LIBRARY_PATH. Also, get the current time; this will be used later to grab the
			# files generated by our translator

			timeStamp = time()

			proc = Popen(cmd,
			     	     shell=True,
			     	     executable='/bin/bash',
			     	     stderr=PIPE,
			     	     stdout=PIPE)

			# Grab both standard output and standard error streams from the process
			stdoutLines, stderrLines = proc.communicate()

			if proc.returncode != 0:
				if testResult == proc.returncode:
					debug.verboseMessage("Test case #%s passed" % (testNum))
				else:
					debug.verboseMessage("Test case #%s did NOT pass" % (testNum))
					testReportLines.append("========== Test %s ==========\n" % (testNum))
					for line in stderrLines.splitlines():
						testReportLines.append(line + "\n")
					testReportLines.append("\n\n")
			else:
				if testResult == 0:				
					debug.verboseMessage("Test case #%s passed" % (testNum))
					manageNewFiles (timeStamp, testNum)
				else:
					debug.verboseMessage("Test case #%s did NOT pass" % (testNum))
	# End of test for loop
	writeTestReport(testReportLines)

if opts.clean:
	clean ()

if opts.run:
	runTests ()

if not opts.clean and not opts.run:
	debug.exitMessage("No actions selected. Use %s for options" % helpShortFlag)
