-- MySQL dump 9.11
--
-- Host: localhost    Database: horaires
-- ------------------------------------------------------
-- Server version	4.0.24_Debian-10-log

--
-- Table structure for table `courses`
--

CREATE TABLE `courses` (
  `unique` int(11) NOT NULL auto_increment,
  `symbol` char(15) NOT NULL default '',
  `title` char(100) default NULL,
  PRIMARY KEY  (`unique`),
  KEY `symbol` (`symbol`)
) TYPE=MyISAM;

--
-- Dumping data for table `courses`
--


--
-- Table structure for table `courses_semester`
--

CREATE TABLE `courses_semester` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `course` int(11) NOT NULL default '0',
  `semester` int(11) NOT NULL default '0',
  `course_type` char(6) NOT NULL default '',
  PRIMARY KEY  (`unique`),
  KEY `semester_index` (`semester`),
  KEY `course_index` (`course`)
) TYPE=MyISAM;

--
-- Dumping data for table `courses_semester`
--


--
-- Table structure for table `groups`
--

CREATE TABLE `groups` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `course_semester` int(11) NOT NULL default '0',
  `name` char(10) default NULL,
  `theory_or_lab` char(5) default NULL,
  `teacher` char(50) default NULL,
  `places_room` int(11) NOT NULL default '0',
  `places_group` int(11) NOT NULL default '0',
  `places_taken` int(11) NOT NULL default '0',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`unique`),
  KEY `name_index` (`name`),
  KEY `tol_index` (`theory_or_lab`),
  KEY `course_semester_index` (`course_semester`)
) TYPE=MyISAM;

--
-- Dumping data for table `groups`
--


--
-- Table structure for table `periods`
--

CREATE TABLE `periods` (
  `unique` int(11) NOT NULL auto_increment,
  `group` int(11) NOT NULL default '0',
  `time` char(8) NOT NULL default '',
  `room` char(15) NOT NULL default '',
  `week` char(3) NOT NULL default '0',
  `period_code` char(5) NOT NULL default '',
  `weekday` char(10) NOT NULL default '',
  PRIMARY KEY  (`unique`),
  KEY `group` (`group`)
) TYPE=MyISAM;

--
-- Dumping data for table `periods`
--


--
-- Table structure for table `semesters`
--

CREATE TABLE `semesters` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `code` char(10) NOT NULL default '',
  `pretty_name` char(15) NOT NULL default '',
  PRIMARY KEY  (`unique`)
) TYPE=MyISAM;

--
-- Dumping data for table `semesters`
--

INSERT INTO `semesters` VALUES (2,'H2005','Hiver 2005');

