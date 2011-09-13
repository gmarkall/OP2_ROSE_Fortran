import re
import os

class FormatFortranCode():

	def __init__ (self, files, maxLineLength=120):		
		# Maximum line length, as requested by the user	
        	self.maxLineLength = maxLineLength

		# Files to process		
		self.files = files

		self.__format__()

	def __writeLine__ (self, f, line, indent):
		# Allows pretty-printing indentation
		indentPrefix = " " * indent
	
		# Edit the line so that all extra whitespace is removed and prepend the indent prefix
		line = line.strip()
		line = indentPrefix + line

		while len(line.rstrip()) > 0:
			# While the line is not empty, break it up into substrings adhering to line length
			# and print these smaller strings
			upperIndex = self.maxLineLength - 1
			f.write(line[:upperIndex])

			remainingLine = line[upperIndex:]
			line          = indentPrefix + remainingLine 

			if len(line.rstrip()) > 0:
				# If the remaining line is not empty then need
				# to print '&' at the end of current line and 
				# before the 1st character on the next one
				f.write("&")
				line = indentPrefix + "&" + remainingLine

			f.write("\n")
	
	def __format__ (self):
		end_regex        = re.compile("\s*end\s", re.IGNORECASE)
		subroutine_regex = re.compile("\ssubroutine\s", re.IGNORECASE)	
		do_regex         = re.compile("\s*do\s", re.IGNORECASE)
		select_regex     = re.compile("\s*select\s", re.IGNORECASE)
		module_regex     = re.compile("\s*module\s", re.IGNORECASE)
		program_regex    = re.compile("\s*program\s", re.IGNORECASE)
		# Have to match any character before the 'IF' because it can be on the same line as a 'CASE' statement
		if_regex         = re.compile(".*if\s", re.IGNORECASE) 
		type_regex       = re.compile("\s*type\s", re.IGNORECASE)
		call_regex       = re.compile("\s*call\s", re.IGNORECASE)
		implicit_regex   = re.compile("\s*implicit none\s", re.IGNORECASE)
		contains_regex   = re.compile("\s*contains\s", re.IGNORECASE)

		for fileName in self.files:
			moduleOrProgramFound = False
			newLineNeeded        = False
			callFound            = False
			indent               = 0
			f2                   = open("_" + os.path.basename(fileName), "w")
			f                    = open(fileName, "r")

			for line in f:
				if line.strip() and (line.strip()[0] == '#' or line.strip()[0] == '!'):
					# Only output comments within the module or program scope
					if moduleOrProgramFound:
						f2.write(line)

				elif end_regex.match(line):
					if not type_regex.search(line):
						indent = indent - 2
					self.__writeLine__(f2, line, indent)
					f2.write("\n")
					newLineNeeded = False
	
				elif module_regex.match(line) or program_regex.match(line) or subroutine_regex.search(line) or \
					do_regex.match(line) or if_regex.match(line) or select_regex.match(line):
				
					if module_regex.match(line) or program_regex.match(line):
						moduleOrProgramFound = True

					if newLineNeeded:
						f2.write("\n")

					self.__writeLine__(f2, line, indent)
	
					indent = indent + 2	
					newLineNeeded = True

				elif not re.compile("\n").match(line):
					if implicit_regex.match(line) or contains_regex.match(line):
						if not newLineNeeded:
							f2.write("\n")
						self.__writeLine__(f2, line, indent)
					else:
						self.__writeLine__(f2, line, indent)	
						newLineNeeded = True				

			f.close()
			f2.close()
			os.rename(f2.name,f.name)
