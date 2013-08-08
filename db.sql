-- MySQL dump 10.13  Distrib 5.5.31, for debian-linux-gnu (armv7l)
--
-- Host: localhost    Database: mit
-- ------------------------------------------------------
-- Server version	5.5.31-0+wheezy1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `domotics`
--

DROP TABLE IF EXISTS `domotics`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `domotics` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `type` varchar(100) DEFAULT NULL,
  `code` varchar(45) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `type_idx` (`type`)
) ENGINE=InnoDB AUTO_INCREMENT=20 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `domotics`
--

LOCK TABLES `domotics` WRITE;
/*!40000 ALTER TABLE `domotics` DISABLE KEYS */;
INSERT INTO `domotics` VALUES (1,'Light Board','light','Light_Board'),(2,'Light Middle Board','light','Light_Middle_Board'),(3,'Light Middle TV','light','Light_Middle_TV'),(4,'Light TV','light','Light_TV'),(5,'Light Board Left','light','Light_Board_Left'),(6,'Light Board Right','light','Light_Board_Right'),(7,'Light Middle Board Left','light','Light_Middle_Board_Left'),(8,'Light Middle Board Right','light','Light_Middle_Board_Right'),(9,'Light Middle TV Left','light','Light_Middle_TV_Left'),(10,'Light Middle TV Right','light','Light_Middle_TV_Right'),(11,'Light TV Left','light','Light_TV_Left'),(12,'Light TV Right','light','Light_TV_Right'),(13,'Light All','light','Light_All'),(14,'Light Leds TV','light','Light_Leds_TV'),(15,'Blind 1','blind','Blind_1'),(16,'Blind 2','blind','Blind_2'),(17,'Blind 3','blind','Blind_3'),(18,'Blind All','blind','Blind_All'),(19,'Door','door','Door');
/*!40000 ALTER TABLE `domotics` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `domotics_functions`
--

DROP TABLE IF EXISTS `domotics_functions`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `domotics_functions` (
  `type` varchar(100) NOT NULL DEFAULT '',
  `function` varchar(100) NOT NULL DEFAULT '',
  `value` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`type`,`function`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `domotics_functions`
--

LOCK TABLES `domotics_functions` WRITE;
/*!40000 ALTER TABLE `domotics_functions` DISABLE KEYS */;
INSERT INTO `domotics_functions` VALUES ('ac','Decrease Fan Speed',0),('ac','Decrease Temperature',0),('ac','Increase Fan Speed',0),('ac','Increase Temperature',0),('ac','Mode Cold',0),('ac','Mode Hot',0),('ac','Set Fan Speed',1),('ac','Set Temperature',1),('ac','Swing Off',0),('ac','Swing On',0),('ac','Turn Off',0),('ac','Turn On',0),('blind','Set',1),('door','Open',0),('light','Set',1),('sockets','Turn Off',0),('sockets','Turn On',0);
/*!40000 ALTER TABLE `domotics_functions` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `mobiles`
--

DROP TABLE IF EXISTS `mobiles`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mobiles` (
  `name` varchar(100) DEFAULT NULL,
  `mac` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`mac`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `mobiles`
--

LOCK TABLES `mobiles` WRITE;
/*!40000 ALTER TABLE `mobiles` DISABLE KEYS */;
INSERT INTO `mobiles` VALUES ('dario','5C:96:9D:76:25:36');
/*!40000 ALTER TABLE `mobiles` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sensorData`
--

DROP TABLE IF EXISTS `sensorData`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sensorData` (
  `sensor_id` int(11) NOT NULL DEFAULT '0',
  `mobile_mac` varchar(100) NOT NULL DEFAULT '',
  `ts` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `rss` double DEFAULT NULL,
  `distance` double DEFAULT NULL,
  PRIMARY KEY (`sensor_id`,`mobile_mac`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sensorData`
--

LOCK TABLES `sensorData` WRITE;
/*!40000 ALTER TABLE `sensorData` DISABLE KEYS */;
INSERT INTO `sensorData` VALUES (1,'5C:96:9D:76:25:36','2013-08-02 21:36:49',-68,1.3941158635037);
/*!40000 ALTER TABLE `sensorData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `sensors`
--

DROP TABLE IF EXISTS `sensors`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sensors` (
  `sensor_id` int(11) NOT NULL DEFAULT '0',
  `x` double DEFAULT NULL,
  `y` double DEFAULT NULL,
  `sensor_mac` varchar(100) DEFAULT NULL,
  `sensor_ip` varchar(100) DEFAULT NULL,
  `correction_factor` double DEFAULT NULL,
  PRIMARY KEY (`sensor_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `sensors`
--

LOCK TABLES `sensors` WRITE;
/*!40000 ALTER TABLE `sensors` DISABLE KEYS */;
INSERT INTO `sensors` VALUES (1,162,258,'00:15:83:52:42:92','10.0.0.1',0.5),(2,557,59,'00:02:72:AF:00:81','10.0.0.2',1),(3,681,512,'00:02:72:B0:D4:1C','10.0.0.3',1);
/*!40000 ALTER TABLE `sensors` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `task_list`
--

DROP TABLE IF EXISTS `task_list`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `task_list` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` varchar(45) DEFAULT NULL,
  `value` int(11) DEFAULT NULL,
  `domotic` int(11) DEFAULT NULL,
  `compare` varchar(45) DEFAULT NULL,
  `distance` double DEFAULT NULL,
  `sensor_id` int(11) DEFAULT NULL,
  `mac` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `mac_idx` (`mac`),
  KEY `sensor_id_idx` (`sensor_id`),
  KEY `domotic_idx` (`domotic`),
  CONSTRAINT `domotic` FOREIGN KEY (`domotic`) REFERENCES `domotics` (`id`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `mac` FOREIGN KEY (`mac`) REFERENCES `mobiles` (`mac`) ON DELETE NO ACTION ON UPDATE NO ACTION,
  CONSTRAINT `sensor_id` FOREIGN KEY (`sensor_id`) REFERENCES `sensors` (`sensor_id`) ON DELETE NO ACTION ON UPDATE NO ACTION
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `task_list`
--

LOCK TABLES `task_list` WRITE;
/*!40000 ALTER TABLE `task_list` DISABLE KEYS */;
INSERT INTO `task_list` VALUES (6,'Set',100,1,'lower',100,1,'5C:96:9D:76:25:36');
/*!40000 ALTER TABLE `task_list` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `toDo`
--

DROP TABLE IF EXISTS `toDo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `toDo` (
  `data` varchar(100) NOT NULL,
  PRIMARY KEY (`data`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `toDo`
--

LOCK TABLES `toDo` WRITE;
/*!40000 ALTER TABLE `toDo` DISABLE KEYS */;
/*!40000 ALTER TABLE `toDo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-08-02 21:36:56
