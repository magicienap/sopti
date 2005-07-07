-- MySQL dump 10.9
--
-- Host: localhost    Database: poly_courses
-- ------------------------------------------------------
-- Server version	4.1.11-Debian_3-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `courses`
--

DROP TABLE IF EXISTS `courses`;
CREATE TABLE `courses` (
  `unique` int(11) NOT NULL auto_increment,
  `symbol` varchar(15) NOT NULL default '',
  `title` varchar(100) default NULL,
  PRIMARY KEY  (`unique`),
  KEY `symbol` (`symbol`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `courses`
--


/*!40000 ALTER TABLE `courses` DISABLE KEYS */;
LOCK TABLES `courses` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `courses` ENABLE KEYS */;

--
-- Table structure for table `courses_semester`
--

DROP TABLE IF EXISTS `courses_semester`;
CREATE TABLE `courses_semester` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `course` int(11) NOT NULL default '0',
  `semester` int(11) NOT NULL default '0',
  `course_type` varchar(6) NOT NULL default '',
  PRIMARY KEY  (`unique`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `courses_semester`
--


/*!40000 ALTER TABLE `courses_semester` DISABLE KEYS */;
LOCK TABLES `courses_semester` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `courses_semester` ENABLE KEYS */;

--
-- Table structure for table `groups`
--

DROP TABLE IF EXISTS `groups`;
CREATE TABLE `groups` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `course_semester` int(11) NOT NULL default '0',
  `name` varchar(15) NOT NULL default '0',
  `theory_or_lab` varchar(5) NOT NULL default '0',
  `teacher` varchar(50) default NULL,
  `places_room` int(11) NOT NULL default '0',
  `places_group` int(11) NOT NULL default '0',
  `places_taken` int(11) NOT NULL default '0',
  `closed` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`unique`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `groups`
--


/*!40000 ALTER TABLE `groups` DISABLE KEYS */;
LOCK TABLES `groups` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `groups` ENABLE KEYS */;

--
-- Table structure for table `periods`
--

DROP TABLE IF EXISTS `periods`;
CREATE TABLE `periods` (
  `unique` int(11) NOT NULL auto_increment,
  `group` int(11) NOT NULL default '0',
  `time` varchar(8) NOT NULL default '0',
  `room` varchar(15) NOT NULL default '0',
  `week` char(3) NOT NULL default '0',
  `period_code` varchar(5) NOT NULL default '',
  `weekday` varchar(10) NOT NULL default '',
  PRIMARY KEY  (`unique`),
  KEY `group` (`group`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `periods`
--


/*!40000 ALTER TABLE `periods` DISABLE KEYS */;
LOCK TABLES `periods` WRITE;
UNLOCK TABLES;
/*!40000 ALTER TABLE `periods` ENABLE KEYS */;

--
-- Table structure for table `semesters`
--

DROP TABLE IF EXISTS `semesters`;
CREATE TABLE `semesters` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `code` varchar(10) NOT NULL default '',
  `pretty_name` varchar(15) NOT NULL default '',
  PRIMARY KEY  (`unique`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Dumping data for table `semesters`
--


/*!40000 ALTER TABLE `semesters` DISABLE KEYS */;
LOCK TABLES `semesters` WRITE;
INSERT INTO `semesters` VALUES (2,'H2005','Hiver 2005');
UNLOCK TABLES;
/*!40000 ALTER TABLE `semesters` ENABLE KEYS */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

