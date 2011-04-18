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

Emailer.py reads notification requests from a table in the sopti 
database, checks if the groups for those requests have space 
available and if it is the case notifies by email the person who 
made the request that they can go change their schedule. After 
sending the email, the program removes that entry from the database. 

The program can log and print relevant parts of its operation.
See `emailer.py -h` for information on this.

Some aspects of its operation can also be changed via a configuration
file (see the example config file in the distribution)

The table that contains the notfications is filled via a php script 
embeded in the SOPTI web interface.

CVS information:
$revision$
$date$
$author$
$name$

"""

import sys
import MySQLdb
import smtplib
import socket
import email.MIMEText
import time
import random
import string
import urllib
import re

from configuration import Configuration
import hmac
import hashlib

def main():
	# The configuration is merged from the different sources
	configuration= Configuration.getInstance()
	logging= configuration["loggingModule"]
	random.seed()
	
	configuration["stats"]= {
		"emailsSuccessfully" : 0, # emails were sent
		"emailsFailed" : 0, # emails could not be sent
		"groupsSuccessfully" : 0, # groups were processed and removed from the db
		"groupsFailed" : 0, # groups could not be processed because of failing emails
		"remainingNotifications" : 0,  # notifications are still remaining in the database
		"remainingGroups" : 0,  # notifications are still remaining in the database
		"remainingEmails" : 0, } # notifications are still remaining in the database
	
	# The notifications to be sent are fetched from the database
	logging.debug("Connecting to the database server \"%s\"..." % (configuration["DB"]["host"],))
	
	try:
		connection= MySQLdb.connect(**configuration["DB"])
		
		connection.query("""
			select 
				`notifications`.`unique`, 
				`notifications`.`email`, 
				`groups`.`unique`, 
				`groups`.`name`, 
				`groups`.`theory_or_lab`, 
				`groupsT`.`teacher`, 
				`groupsL`.`teacher`, 
				`courses_semester`.`course_type`, 
				`courses`.`symbol`, 
				`courses`.`title` 
			from `notifications` 
			inner join `groups` on 
				`notifications`.`group` = `groups`.`unique` 
			left join `groups` as `groupsT` on 
				`groups`.`course_semester` = `groupsT`.`course_semester` and 
				`groups`.`name` = `groupsT`.`name` and 
				`groupsT`.`theory_or_lab` = 'C' 
			left join `groups` as `groupsL` on 
				`groups`.`course_semester` = `groupsL`.`course_semester` and 
				`groups`.`name` = `groupsL`.`name` and 
				`groupsL`.`theory_or_lab` = 'L' 
			inner join `courses_semester` on 
				`groups`.`course_semester` = `courses_semester`.`unique` 
			inner join `courses` on 
				`courses_semester`.`course` = `courses`.`unique` 
			where `groups`.`closed` = '0'
			order by `notifications`.`email`
		""")
	except MySQLdb.MySQLError, message:
		logging.critical("MySQL error %d: %s" % (message[0], message[1]))
		return 1
	else:
		logging.debug("OK")
		notifications= list(connection.store_result().fetch_row(maxrows= 0, how= 2))
	
	# The emails are sent
	try:
		logging.debug("Connecting to SMTP server \"%s:%d\"..." % (configuration["SMTP"]["host"], int(configuration["SMTP"]["port"]),))
		smtp= smtplib.SMTP(**configuration["SMTP"])
		#smtp.set_debuglevel(1)
	except (smtplib.SMTPException, socket.error), message:
		logging.critical("SMTP error %d: %s" % (message[0], message[1]))
		return 1
	else:
		logging.debug("OK")
		
		# the list of the notification entries that were successfully sent
		successfulNotifications= list()
		# the list of the notification entries that could not be sent because of recepient refused errors
		refusedNotifications= list()
		# a group of notifications that are to be sent to the same email address
		notificationGroup= list()
		
		while len(notifications) or len(notificationGroup):
			#TODO: verifier si c'est slow enlever le premier element au lieu du dernier
			if not len(notificationGroup) or (len(notifications) and notifications[0]["notifications.email"] == notificationGroup[0]["notifications.email"]):
				notificationGroup.append(notifications.pop(0))
				continue
			else:
				hasher= hmac.new(
					configuration["general"]["pepper"], 
					notificationGroup[0]["notifications.email"], 
					digestmod=hashlib.sha1)
				# build the notification email
				if len(notificationGroup) == 1:
					# for a single notification
					msg= email.MIMEText.MIMEText(
						configuration["emailIntro"]["singular"] + 
						configuration["emailCourse"]["text"] % getTemplateValues(notificationGroup[0]) +
						(configuration["emailOutro"]["singular"] % {
							"email" : notificationGroup[0]["notifications.email"], 
							"hash" : hasher.hexdigest(),
							"baseurl" : configuration["general"]["baseurl"],}))
					
					msg["Subject"]= configuration["emailSubject"]["singular"]
					msg["From"]= "%s <%s>" % (configuration["sender"]["name"], configuration["sender"]["address"],)
					msg["To"]= notificationGroup[0]["notifications.email"]
				else:
					# for many notifications that are to be sent to 
					# the same address
					msg= email.MIMEText.MIMEText(
						configuration["emailIntro"]["plural"] + 
						"".join([configuration["emailCourse"]["text"] % getTemplateValues(notification) for notification in notificationGroup]) +
						configuration["emailOutro"]["plural"] % {
							"email" : urllib.quote(notificationGroup[0]["notifications.email"]), 
							# a period (.) at the end of the line is also replaced otherwise it is left out of the link by some mail readers
							"hash" : hasher.hexdigest(),
							"baseurl" : configuration["general"]["baseurl"],})
					
					msg["Subject"]= configuration["emailSubject"]["plural"]
					msg["From"]= "%s <%s>" % (configuration["sender"]["name"], configuration["sender"]["address"],)
					msg["To"]= notificationGroup[0]["notifications.email"]
			
			# send the notification email
			infoMesg= "Sending a notification to %s" % (msg["To"],)
			if len(notificationGroup) == 1:
				infoMesg += " for group " + str(notificationGroup[0]["groups.unique"])
			else:
				infoMesg += " for groups " + ", ".join([str(notification["groups.unique"]) for notification in notificationGroup])
			logging.info(infoMesg)
			
			try:
				if not configuration["general"]["dry-run"]: smtp.sendmail(configuration["sender"]["address"], msg["To"], msg.as_string())
			except smtplib.SMTPSenderRefused, message:
				logging.error("SMTP error %d: %s" % (message[0], message[1]))
				logging.error("The program will not try sending any more emails, consider changing your \"from\" address or you SMTP server")
				
				smtp.quit()
				connection.close()
				logging.shutdown()
				
				return 1
			except smtplib.SMTPRecipientsRefused, message:
				message= message[0].values()[0]
				logging.error("SMTP Recipient refused error %d: %s" % (message[0], message[1]))
				
				configuration["stats"]["groupsFailed"]+= len(notificationGroup)
				configuration["stats"]["emailsFailed"]+= 1
				
				if len(notificationGroup) == 1:
					refusedNotifications.append({
						"notifications.unique" : notificationGroup[0]["notifications.unique"], 
						"notifications.email" : notificationGroup[0]["notifications.email"], 
						"notifications.group" : notificationGroup[0]["groups.unique"],})
				else:
					refusedNotifications.extend([{
						"notifications.unique" : notification["notifications.unique"], 
						"notifications.email" : notification["notifications.email"], 
						"notifications.group" : notification["groups.unique"],} 
						for notification in notificationGroup])
			
			except smtplib.SMTPDataError, message:
				logging.error("SMTP Data error %d: %s" % (message[0], message[1]))
				
				configuration["stats"]["groupsFailed"]+= len(notificationGroup)
				configuration["stats"]["emailsFailed"]+= 1
			else:
				if len(notificationGroup) == 1:
					successfulNotifications.append({
						"notifications.unique" : notificationGroup[0]["notifications.unique"], 
						"notifications.email" : notificationGroup[0]["notifications.email"], 
						"notifications.group" : notificationGroup[0]["groups.unique"],})
				else:
					successfulNotifications.extend([{
						"notifications.unique" : notification["notifications.unique"], 
						"notifications.email" : notification["notifications.email"], 
						"notifications.group" : notification["groups.unique"],} 
						for notification in notificationGroup])
				
				configuration["stats"]["emailsSuccessfully"]+= 1
			
			notificationGroup= list()
			
		smtp.quit()
	
	configuration["stats"]["groupsSuccessfully"]= len(successfulNotifications)
	
	# The notifications that have been successfully sent are 
	# cleared from the database
	# The notifications that generated a recipient refused message may
	# be cleared as well depending on the configuration
	logging.info("Cleaning the DB...")
	
	if configuration["general"]["clearRecipientsRefused"]:
		successfulNotifications.extend(refusedNotifications)
	
	if len(successfulNotifications):
		if not configuration["general"]["dry-run"]:
			try:
				connection.query("""
					delete 
					from `notifications` 
					where `unique` in (""" + ", ".join([str(notification["notifications.unique"]) for notification in successfulNotifications]) + """) 
					limit """ + str(len(successfulNotifications)))
			except MySQLdb.MySQLError, message:
				logging.error("MySQL error %d: %s" % (message[0], message[1]))
			else:
				for notification in successfulNotifications:
					logging.dbentries(
						"erased from `notifications` values(%d, %d, '%s')" % (
						notification["notifications.unique"], 
						notification["notifications.group"], 
						notification["notifications.email"],))
				logging.info("%d rows have been removed" % (connection.affected_rows(), ))
		else:
			logging.info("0 rows have been removed")
	else:
		logging.info("nothing to be done")
	
	# statistics are processed
	try:
		connection.query("""
			select 
				count(*) 
				from `notifications`
			""")
	except MySQLdb.MySQLError, message:
		logging.error("MySQL error %d: %s" % (message[0], message[1]))
	else:
		configuration["stats"]["remainingNotifications"]= int(connection.store_result().fetch_row()[0][0])
	
	try:
		# subquerry version
		# if your database server supports subqueries you might want to use this block instead of the next
		#~ connection.query("""
			#~ select 
				#~ count(*) 
				#~ from (
					#~ select 
						#~ count(*) 
					#~ from `notifications`
					#~ group by `group`)
				#~ as `subTable`
			#~ """)
		# safe version
		#~ connection.query("""create temporary table if not exists `_groups` (`group` int(11) not null)""")
		#~ connection.query("""truncate table `_groups`""")
		#~ connection.query("""insert into `_groups` (`group`) select count(*) from `notifications` group by `group`""")
		#~ connection.query("""select count(*) from `_groups`""")
		# temporary table version
		connection.query("""create temporary table `_groups` (`group` int(11) not null) select count(*) from `notifications` group by `group`""")
		connection.query("""select count(*) from `_groups`""")
	except MySQLdb.MySQLError, message:
		logging.error("MySQL error %d: %s" % (message[0], message[1]))
	else:
		configuration["stats"]["remainingGroups"]= int(connection.store_result().fetch_row()[0][0])
	
	try:
		# subquerry version
		# if your database server supports subqueries you might want to use this block instead of the next
		#~ connection.query("""
			#~ select 
				#~ count(*) 
				#~ from (
					#~ select 
						#~ count(*) 
					#~ from `notifications`
					#~ group by `email`)
				#~ as `subTable`
			#~ """)
		# temporary table version
		connection.query("""create temporary table `_emails` (`email` varchar(60) not null) select count(*) from `notifications` group by `email`""")
		connection.query("""select count(*) from `_emails`""")
	except MySQLdb.MySQLError, message:
		logging.error("MySQL error %d: %s" % (message[0], message[1]))
	else:
		configuration["stats"]["remainingEmails"]= int(connection.store_result().fetch_row()[0][0])
	
	logging.summary("Summary for emailer run #%s:" % (configuration["runNumber"],))
	logging.summary("Started at %s" % (time.asctime(configuration["startTime"]), ))
	logging.summary("Finished at %s" % (time.asctime(), ))
	if configuration["stats"]["groupsSuccessfully"]:
		logging.summary(
			("%d group notification" + 
			(configuration["stats"]["groupsSuccessfully"] > 1 and "s were " or " was ") + 
			"sent successfully in %d email" +
			(configuration["stats"]["emailsSuccessfully"] > 1 and ["s"] or [""])[0]) % 
			(configuration["stats"]["groupsSuccessfully"], configuration["stats"]["emailsSuccessfully"],))
	if configuration["stats"]["groupsFailed"]:
		logging.summary(
			("%d group notification" + 
			(configuration["stats"]["groupsFailed"] > 1 and "s " or " ") + 
			"distributed in %d email" + 
			(configuration["stats"]["emailsFailed"] > 1 and "s " or " ") + 
			"FAILED to be sent") % 
			(configuration["stats"]["groupsFailed"], configuration["stats"]["emailsFailed"],))
	logging.summary(
		("%d notification" + 
		(configuration["stats"]["remainingNotifications"] > 1 and "s are " or " is ") + 
		"still in the database spreaded over %d group" + 
		(configuration["stats"]["remainingGroups"] > 1 and "s " or " ") + 
		"and %d email" + 
		(configuration["stats"]["remainingEmails"] > 1 and ["s"] or [""])[0]) % 
		(configuration["stats"]["remainingNotifications"], configuration["stats"]["remainingGroups"], configuration["stats"]["remainingEmails"],))
	
	connection.close()
	logging.shutdown()


def getTemplateValues(notification):
	configuration= Configuration.getInstance()
	
	values= {
		"symbol" : notification["courses.symbol"], 
		"title" : notification["courses.title"], 
		"name" : notification["groups.name"],}
	
	if notification["courses_semester.course_type"] == "TL":
		values["theory_or_lab"]= "combinée théorique et laboratoire"
		values["teacher"]= "\n\tThéorie: %s\n\tLab: %s" % (notification["groupsT.teacher"], notification["groupsL.teacher"],)
	elif notification["groups.theory_or_lab"] == "C":
		values["theory_or_lab"]= "théorique"
		values["teacher"]= notification["groupsT.teacher"]
	elif notification["groups.theory_or_lab"] == "L":
		values["theory_or_lab"]= "laboratoire"
		values["teacher"]= notification["groupsL.teacher"]
	else:
		raise(Exception("Unknown course type"))
	
	return values


if __name__ == "__main__":
	main()
