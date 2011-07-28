#!/usr/bin/python

import os
from subprocess import Popen, PIPE
from optparse import OptionParser
from sys import argv

# The command-line parser and its options
parser = OptionParser(add_help_option=False)

parser.add_option("-o",
                 "--output",
                 action="store_true",
                 dest="output",
                 help="Output files added and those already in the repository.",
                 default=False)

parser.add_option("-v",
                 "--verbose",
                 action="store_true",
                 dest="verbose",
                 help="Be verbose.",
                 default=False)

parser.add_option("-h",
                  "--help",
                  action="help",
                  help="Display this help message.")

parser.add_option("-D",
                  "--directory",
                  action="store",
                  type="string",
                  dest="dir",
                  help="The root directory from which to add source files.",
                  metavar="<DIR>")

(opts, args) = parser.parse_args(argv[1:])

added  = []
inRepo = []

def errorMessage (message):
	print(message)
	exit(1)

def verboseMessage (message):
	if opts.verbose:
		print(message)

def add ():
	absolutePath = os.path.abspath(os.curdir + os.sep + opts.dir)
	verboseMessage("Walking directory '" + absolutePath + "'")

	for path, dirs, files in os.walk(absolutePath):
		for file in files:
			fileBasename, fileExtension = os.path.splitext(file)
			fullPath = os.path.join(path, file)
			
			verboseMessage("Analysing file '" + file)

			if fileExtension == '.cpp' or fileExtension == '.h':
				cmd = 'git ls-files ' + fullPath

				proc = Popen(cmd,
					shell=True,
					executable='/bin/bash',
					stderr=PIPE,
					stdout=PIPE)

				stdout, stderr = proc.communicate()

				if len(stdout) == 0:
					added.append(fullPath)
				
					cmd = 'git add ' + fullPath

					proc = Popen(cmd,
						shell=True,
						executable='/bin/bash',
						stderr=PIPE,
						stdout=PIPE)

					proc.communicate()
		        
				else:
					inRepo.append(fullPath)

def output ():
	if inRepo:
    		print("===== These files already exist in the repository =====")
    		for f in inRepo:
        		print(f)
	if added:
    		print("===== Added the following files =====")
    		for f in added:
        		print(f)

if opts.dir is None:
	errorMessage("You must supply a directory on the command line.")
elif not os.path.isdir(opts.dir):
	errorMessage("The directory '" + opts.dir + "' is not a valid directory")
else:
	add()

	if opts.output:
		output()    

