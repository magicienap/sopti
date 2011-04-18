#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

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

This module adds two custom log levels to the logging module and 
wraps the other functions so it can be used like this:
import logging_emailer as logging

CVS information:
$revision$
$date$
$author$
$name$

"""

from logging import *
# logging.__all__ doesn't include shutdown, so it is not caught by import *
# http://docs.python.org/tutorial/modules.html#importing-from-a-package
from logging import shutdown

# These next tow loops look for an available space between the info 
# and warning log levels for the summary and dbentries log levels. 
# This should not be a problem and should end up giving the same 
# result as doing SUMMARY= INFO + 1; DBENTRIES= INFO + 2 but it is 
# done anyways in case there are changes to the "logging" module
for numLevel in range(INFO + 1, WARNING):
	if getLevelName(numLevel) == "Level %s" % numLevel:
		SUMMARY= numLevel
		break
else:
	raise(Exception("Unable to allocate a number for the SUMMARY log level, the program needs to be updated"))

for numLevel in range(SUMMARY + 1, WARNING):
	if getLevelName(numLevel) == "Level %s" % numLevel:
		DBENTRIES= numLevel
		break
else:
	raise(Exception("Unable to allocate a number for the DBENTRIES log level, the program needs to be updated"))

addLevelName(SUMMARY, "SUMMARY")
addLevelName(DBENTRIES, "DBENTRIES")

def summary(msg, *args, **kwargs):
	getLogger().log(SUMMARY, msg, *args, **kwargs)

def dbentries(msg, *args, **kwargs):
	getLogger().log(DBENTRIES, msg, *args, **kwargs)
