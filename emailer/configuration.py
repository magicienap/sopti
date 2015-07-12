#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
Emailer.py version 0.1
Copyright (C) 2005 Benjamin Poirier <ben_tha_man@users.sf.net>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Emailer.py is a program to send emails from a database of notification 
requests.
It is part of the SOPTI (School Schedule Optimizer) package written 
by Pierre-Marc Fournier <pmf@users.sf.net>
For more information about emailer.py see the emailer.py file.

If you are editing this file only to change the configuration, please
be aware that you can also use a configuration file. See 
emailer.conf.example for more details.

This module takes care of loading the configuration, storing the
configuration and initializing the logging.

CVS information:
$revision$
$date$
$author$
$name$

"""

import sys
import optparse
import types
import time
import random
import ConfigParser

import singletonmixin
import logging_emailer as logging


class Configuration(singletonmixin.Singleton):
	"""This class defines a container for configuration information,
	this container is a model of a singleton (there can only be one
	configuration for the whole program).
	
	When first instanciated, the configuration will be loaded from
	default values, command line options and a configuration file.
	
	The configuration values can be accessed like a dictionnary.
	It is also possible to set or modify configuration values like in
	a dictionnary.
	"""	
	def __init__(self):
		"""The init method loads the hardcoded configuration that 
		contains default values for everything, alas possibly not 
		very good ones, then updates this data with the information
		read from the command line. At this point the logging module 
		can be initialized and  finally, if the configuration file 
		is found its data is merged.
		"""		
		# The default "hardcoded" configuration is loaded
		version= "0.1"
		name= "emailer.py"
		# The start time is probably not "super" exact but I guess 
		# it's good enough for what it's used for
		startTime= startTime= time.localtime()
		random.seed()
		runNumber= time.strftime("%Y%m%d%H%M%S", startTime) + str(random.randint(1000, 9999))
		
		# ================= Modify the values BELOW ================= #
		# vvvvvvvvvvv to change the default configuration vvvvvvvvvvv #
		configGeneral= {
			"pepper" : "changeme",
			"baseurl" : "http://localhost",}
		
		configDB= {
			"user" : "sopti", 
			"passwd" : "=l@FRo0Koudi", 
			"host" : "127.0.0.1", 
			"db" : "sopti",}
		
		configSMTP= {
			"host" : "localhost", 
			"port" : 25,}
		
		configSender= {
			"address" : "sopti@localhost", 
			"name" : "School Schedule Optimizer",}
		
		configEmailSubject= {
			"singular" : "Le groupe que vous avez demandÃ© est disponible",
			"plural" : "Les groupes que vous avez demandÃ©s sont disponibles",}
		configEmailIntro= {
			"singular" :
			"""Bonjour,
			
			Le groupe que vous avez demandÃ© est maintenant disponible. Voici 
			l'information Ã  propos de ce groupe:
			
			""",
			"plural" :
			"""Bonjour,
			
			Les groupes que vous avez demandÃ©s sont maintenant disponibles. 
			Voici les informations Ã  propos de ces groupes:
			
			""",}
		# %(_info_)s string format elements will be replace with the 
		# appropriate data, _info_ can be "symbol", "title", "name", 
		# "theory_or_lab" and "teacher"
		configEmailCourse= {
			"text" :
			"""%(symbol)s - %(title)s
			Section %(name)s pour la partie %(theory_or_lab)s du cours
			Enseignant(e): %(teacher)s
			
			""",}
		configEmailOutro= {
			"singular" :
			"""Si vous souhaitez toujours vous inscrire Ã  ce groupe veuillez aller 
			faire le changement dans votre dossier Ã©tudiant le plus rapidement
			possible puisque d'autres personnes peuvent aussi s'inscrire.
			
			Vous pouvez accÃ©der au dossier Ã©tudiant Ã  cette adresse:
			https://www4.polymtl.ca/poly/poly.html
			
			Vous pouvez aussi aller vÃ©rifier vos possibilitÃ©s d'horaire Ã  nouveau
			sur le site du gÃ©nÃ©rateur d'horaires:
			%(baseurl)
			
			Si vous avez d'autre demandes en attente que vous dÃ©sirer annuler
			vous pouvez le faire en allant Ã  l'adresse suivante:
			%(baseurl)/email_unsubscribe.php?email=%(email)&hash=%(hash)
			
			Merci d'avoir utilisÃ© le gÃ©nÃ©rateur d'horaires.
			Vous pouvez nous faire part de vos commentaires Ã  l'adresse 
			horaires@step.polymtl.ca
			
			-L'Ã©quipe du gÃ©nÃ©rateur d'horaires
			""",
			"plural" :
			"""Si vous souhaitez toujours vous inscrire Ã  ces groupes veuillez aller 
			faire les changements dans votre dossier Ã©tudiant le plus rapidement
			possible puisque d'autres personnes peuvent aussi s'inscrire.
			
			Vous pouvez accÃ©der au dossier Ã©tudiant Ã  cette adresse:
			https://www4.polymtl.ca/poly/poly.html
			
			Vous pouvez aussi aller vÃ©rifier vos possibilitÃ©s d'horaire Ã  nouveau
			sur le site du gÃ©nÃ©rateur d'horaires:
			%(baseurl)
			
			Si vous avez d'autre demandes en attente que vous dÃ©sirer annuler
			vous pouvez le faire en allant Ã  l'adresse suivante:
			%(baseurl)/email_unsubscribe.php?email=%(email)&hash=%(hash)
			
			Merci d'avoir utilisÃ© le gÃ©nÃ©rateur d'horaires.
			Vous pouvez nous faire part de vos commentaires Ã  l'adresse 
			horaires@step.polymtl.ca
			
			-L'Ã©quipe du gÃ©nÃ©rateur d'horaires
			""",}
		# ^^^^^^^^^^^^^^^^^ Modify the values ABOVE ^^^^^^^^^^^^^^^^^ #
		# =========== to change the default configuration =========== #
		
		configGeneral.update({
			"verbose" : False, 
			"summary" : False, 
			"dry-run" : False, 
			"clearRecipientsRefused" : False, 
			"logFile" : "", 
			# 0 - do not write a log, 1 - critical, 2 - error, 3 - warning, 4 - dbentries, 5 - summary, 6 - info, 7 - debug
			"logLevel" : 5, 
			"configFile" : "",})
		
		self._data= {
			"version" : version, 
			"name" : name, 
			"startTime" : startTime, 
			"runNumber" : runNumber, 
			"DB" : configDB, 
			"SMTP" : configSMTP, 
			"sender" : configSender, 
			"emailSubject" : configEmailSubject, 
			"emailIntro" : configEmailIntro, 
			"emailCourse" : configEmailCourse, 
			"emailOutro" : configEmailOutro, 
			"general" : configGeneral,}
		
		# The configuration from the command line is loaded
		parser= optparse.OptionParser(version= "%prog 0.1")
		
		parser.add_option("--verbose", "-v", action= "store_true", dest= "verbose", 
			help= "generate messages describing what the program is doing")
		parser.add_option("--summary", "-s", action= "store_true", dest= "summary", 
			help= "print a summary of what has been done before exiting")
		parser.add_option("--dry-run", "-d", action= "store_true", dest= "dry-run", 
			help= "run as usual but do not send emails and do not modify the database")
		parser.add_option("--clear-recipients-refused", action= "store_true", dest= "clearRecipientsRefused", 
			help= "remove the notification entries in the database for which the SMTP server returns a recipients refused status")
		parser.add_option("--log-file", "-l", action= "store", type= "string", dest= "logFile", 
			help= "specify the filename of the log file, if not specified no log will be written")
		parser.add_option("--log-level", "-L", action= "store", type= "int", dest= "logLevel", 
			help= """specify the verbosity level of the log file, the higher the number, the more messges you will get 
			(0 - no log, 1 - critical, 2 - error, 3 - warning, 4 - db entries that are erased, 5 - summary (default), 6 - info, 7 - debug)""")
		parser.add_option("--config-file", "-c", action= "store", type= "string", dest= "configFile", 
			help= "specify the filename of the configuration file")
		
		(parsedCLOptions, parsedCLArgs)= parser.parse_args()
		
		if len(parsedCLArgs):
			parser.error("unknown argument %s" % parsedCLArgs[0])
		
		# The configuration data is updated with the command line options
		newData= {"general": {}}
		for option in ["verbose", "summary", "dry-run", "clearRecipientsRefused", "logFile", "logLevel", "configFile"]:
			if getattr(parsedCLOptions, option) is not None:
				newData["general"][option]= getattr(parsedCLOptions, option)
		
		recursiveUpdate(self._data, newData)
		
		# The logging module is initialized
		_setupLogging(runNumber, 
			self._data["general"]["verbose"], self._data["general"]["summary"], 
			self._data["general"]["logFile"], self._data["general"]["logLevel"])
		self._data["loggingModule"]= logging
		
		# The configuration from the configfile is loaded
		if self._data["general"]["configFile"].strip():
			logging.debug("Openning the configuration file...")
			try:
				fd= open(self._data["general"]["configFile"])
			except IOError, message:
				logging.error("The configuration file could not be openned: %s" % (message,))
			else:
				logging.debug("OK")
				logging.debug("Parsing the configuration file...")
				parser= ConfigParser.SafeConfigParser()
				parser.readfp(fd)		
				fd.close()
				
				configKeys={
					"general" : ["pepper", "baseurl",], 
					"DB" : ["user", "passwd", "host", "db",], 
					"SMTP" : ["host", "port",], 
					"sender" : ["address", "name",], 
					"emailSubject" : ["singular", "plural",], 
					"emailIntro" : ["singular", "plural",], 
					"emailCourse" : ["text",], 
					"emailOutro" : ["singular", "plural"],}
				
				# The configuration data is updated with the configuration file options
				for section in parser.sections():
					if configKeys.has_key(section):
						for option in parser.options(section):
							if configKeys[section].count(option):
								self._data[section][option]= parser.get(section, option)
								# ConfigParser escapes \n's so they have to be unescaped
								if type(self._data[section][option]) == types.StringType:
									self._data[section][option]= self._data[section][option].replace(r"\n", "\n")
							else:
								logging.warning("Ignoring unknown option \"%s\" in config file" % (option, ))
					else:
						logging.warning("Ignoring unknown section \"%s\" in config file" % (section, ))
				
				logging.debug("OK")

	
	def __getitem__(self, key):
		return self._data[key]
	
	
	def __setitem__(self, *args):
		"""This method allows to set new configuration values in the
		same way that it would be done with a dictionnary.
		For example:
		configuration["runNumber"]= time.strftime("%Y%m%d%H%M%S", startTime) + str(random.randint(1000, 9999))
		configuration["logging"]= {}
		configuration["logging"]["summaryLevel"]= logging.INFO + 1
		"""
		item= self._data
		
		for key in args[:-2]:
			item= item[key]
		
		item[args[-2]]= args[-1]
	
	
	def __repr__(self):
		return repr(self._data)


def recursiveUpdate(old, new):
	"""Will update old with new much like dict.update but recursively.
	For example:
	a= {"fruits" : {"apples" : "green"},}
	b= {"fruits" : {"tomatoes" : "red"}, "vegetables" : {"carrots" : "orange"},}
	
	a.update(b) results in
	{'fruits': {'tomatoes': 'red'}, 'vegetables': {'carrots': 'orange'},}
	whereas recursiveUpdate(a, b) results in
	{'fruits': {'apples' : 'green', 'tomatoes': 'red'}, 'vegetables': {'carrots': 'orange'},}
	the "apples" key-value pair is not lost	
	"""
	for key in new:
		if old.has_key(key) and type(old[key]) == types.DictType:
			recursiveUpdate(old[key], new[key])
		else:
			old[key]= new[key]


def _setupLogging(runNumber, verbose, summary, logFile, logLevel):
	"""
	This function sets up levels, handlers and filters that will be 
	used for logging. It uses the logging module.
	After this function has been executed, calls to logger.log() and 
	the logger.critical, logger.error, ... methods will dispatch log 
	entries to the appropriate places.
	"""
	
	levelNames= [
		None, 
		logging.CRITICAL, 
		logging.ERROR, 
		logging.WARNING, 
		logging.DBENTRIES, 
		logging.SUMMARY, 
		logging.INFO,
		logging.DEBUG,]
	
	# set up the root logger
	logging.getLogger().setLevel(logging.NOTSET)
	
	# set up the formats
	outputFormat= logging.Formatter("%(levelname)-10s %(message)s")
	summaryFormat= logging.Formatter("%(message)s")
	logFormat= logging.Formatter("[%(asctime)s] [" + runNumber + "] [%(levelname)s] %(message)s")
	
	# debug through warning will go to stdout, depending on the verbose setting
	if verbose:
		general= logging.StreamHandler(sys.stdout)
		general.addFilter(LevelListFilter(logging.DEBUG, logging.INFO, logging.WARNING))
		general.setFormatter(outputFormat)
		logging.getLogger().addHandler(general)
	
	# summary will go to stdout, depending on the summary setting
	if summary:
		summary= logging.StreamHandler(sys.stdout)
		summary.addFilter(LevelListFilter(logging.SUMMARY))
		summary.setFormatter(summaryFormat)
		logging.getLogger().addHandler(summary)
	
	# error and critical will go to stderr
	error= logging.StreamHandler()
	error.setLevel(logging.ERROR)
	error.setFormatter(outputFormat)
	logging.getLogger().addHandler(error)
	
	# logging to file is dependant on the logLevel configuration
	if logLevel and logFile:
		log= logging.FileHandler(logFile)
		log.setLevel(levelNames[logLevel])
		log.setFormatter(logFormat)	
		logging.getLogger().addHandler(log)


class LevelListFilter(logging.Filter):
	"""This class defines a filter for log entries to be used with
	the logging module.
	
	This filter will accept entries whose level is among the list
	of levels specified during the initialisation of the filter, it 
	will reject any other entry.
	
	Use the method handler.addFilter() to use the filter after having
	created it.
	"""
	def __init__(self, *args):
		"""The initialisation parameter is a list of log levels for which
		entries will be accepted.
		For example:
		LevelListFilter(logging.DEBUG, logging.INFO, logging.WARNING)
		creates a filter that only accepts messages of level debug, 
		info or warning
		"""
		self.args= list(args)
	
	def filter(self, record):
		return self.args.count(record.levelno)


if __name__ == "__main__":
	configuration= Configuration.getInstance()
