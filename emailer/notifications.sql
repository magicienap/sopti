-- 
-- Table structure for table `notifications`
-- 

CREATE TABLE `notifications` (
  `unique` int(10) unsigned NOT NULL auto_increment,
  `group` int(11) NOT NULL default '0',
  `email` varchar(60) NOT NULL default '',
  `ip` varchar(15) default NULL,
  PRIMARY KEY  (`unique`)
) ENGINE=MyISAM;
