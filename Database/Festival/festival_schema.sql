/*M!999999\- enable the sandbox mode */ 

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*M!100616 SET @OLD_NOTE_VERBOSITY=@@NOTE_VERBOSITY, NOTE_VERBOSITY=0 */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_event` (
  `id` int(10) unsigned NOT NULL,
  `status` enum('CLOSED','ACTIVE','PROCESSING') NOT NULL DEFAULT 'CLOSED',
  `started_at` datetime DEFAULT NULL,
  `ends_at` datetime DEFAULT NULL,
  `current_cycle` int(10) unsigned NOT NULL DEFAULT 0,
  `cycle_duration_seconds` int(10) unsigned NOT NULL DEFAULT 1800,
  `cycle_started_at` datetime DEFAULT NULL,
  `cycle_ends_at` datetime DEFAULT NULL,
  `created_at` datetime NOT NULL DEFAULT current_timestamp(),
  `updated_at` datetime NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_guild_state` (
  `event_id` int(10) unsigned NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `total_points` int(10) unsigned NOT NULL DEFAULT 0,
  `completed_missions` int(10) unsigned NOT NULL DEFAULT 0,
  `renewal_count` int(10) unsigned NOT NULL DEFAULT 0,
  `next_renewal_at` datetime DEFAULT NULL,
  `updated_at` datetime NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `reward_progress` int(10) unsigned NOT NULL DEFAULT 0,
  `pending_points` int(10) unsigned NOT NULL DEFAULT 0,
  `claimed_chests` tinyint(3) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`event_id`,`guild_id`),
  KEY `idx_guild_points` (`event_id`,`total_points`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_guild_mission` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `template_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned DEFAULT NULL,
  `status` enum('AVAILABLE','IN_PROGRESS','COMPLETED','FAILED') NOT NULL DEFAULT 'AVAILABLE',
  `progress` int(10) unsigned NOT NULL DEFAULT 0,
  `target_count` int(10) unsigned NOT NULL DEFAULT 1,
  `points` int(10) unsigned NOT NULL DEFAULT 0,
  `points_awarded` tinyint(1) NOT NULL DEFAULT 0,
  `created_at` datetime NOT NULL DEFAULT current_timestamp(),
  `accepted_at` datetime DEFAULT NULL,
  `expires_at` datetime DEFAULT NULL,
  `completed_at` datetime DEFAULT NULL,
  `cycle_id` int(10) unsigned NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  KEY `idx_guild_event_status` (`guild_id`,`event_id`,`status`),
  KEY `idx_player_status` (`player_id`,`status`),
  KEY `idx_event_guild` (`event_id`,`guild_id`),
  KEY `idx_expiration` (`status`,`expires_at`),
  KEY `idx_template` (`template_id`)
) ENGINE=InnoDB AUTO_INCREMENT=621 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_mission_template` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `title` varchar(100) NOT NULL,
  `description` varchar(255) NOT NULL,
  `type` tinyint(3) unsigned NOT NULL DEFAULT 1,
  `target_vnum` int(10) unsigned NOT NULL DEFAULT 0,
  `target_count` int(10) unsigned NOT NULL DEFAULT 1,
  `time_limit` int(10) unsigned NOT NULL DEFAULT 3600,
  `points` int(10) unsigned NOT NULL DEFAULT 0,
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  KEY `idx_enabled_type` (`enabled`,`type`)
) ENGINE=InnoDB AUTO_INCREMENT=59 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_player_state` (
  `event_id` int(10) unsigned NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned NOT NULL,
  `cycle_id` int(10) unsigned NOT NULL,
  `accepted_missions` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`event_id`,`guild_id`,`player_id`,`cycle_id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_pvp_history` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `cycle_id` int(10) unsigned NOT NULL,
  `killer_id` int(10) unsigned NOT NULL,
  `victim_id` int(10) unsigned NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `killed_at` datetime NOT NULL DEFAULT current_timestamp(),
  PRIMARY KEY (`id`),
  KEY `idx_killer_victim` (`event_id`,`cycle_id`,`killer_id`,`victim_id`),
  KEY `idx_victim` (`event_id`,`cycle_id`,`victim_id`),
  KEY `idx_guild` (`event_id`,`cycle_id`,`guild_id`),
  KEY `idx_killed_at` (`killed_at`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_config` (
  `config_key` varchar(64) NOT NULL,
  `config_value` varchar(255) NOT NULL,
  `description` varchar(255) DEFAULT NULL,
  `updated_at` datetime NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`config_key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_chests` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `chest_index` tinyint(3) unsigned NOT NULL,
  `required_points` int(10) unsigned NOT NULL,
  `roll_count` smallint(5) unsigned NOT NULL DEFAULT 1,
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_event_chest` (`event_id`,`chest_index`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_chest_rewards` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `chest_id` int(10) unsigned NOT NULL,
  `vnum` int(10) unsigned NOT NULL,
  `weight` int(10) unsigned NOT NULL,
  `min_count` smallint(5) unsigned NOT NULL DEFAULT 1,
  `max_count` smallint(5) unsigned NOT NULL DEFAULT 1,
  `enabled` tinyint(1) NOT NULL DEFAULT 1,
  PRIMARY KEY (`id`),
  KEY `idx_chest_enabled` (`chest_id`,`enabled`),
  CONSTRAINT `fk_festival_chest_rewards` FOREIGN KEY (`chest_id`) REFERENCES `festival_chests` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=19 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_guild_history` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `edition_key` datetime NOT NULL,
  `started_at` datetime NOT NULL,
  `ended_at` datetime NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `total_points` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_edition_guild` (`edition_key`,`guild_id`),
  KEY `idx_edition_points` (`edition_key`,`total_points`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_chest_claims` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `edition_key` datetime NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned NOT NULL,
  `sequence_no` int(10) unsigned NOT NULL,
  `chest_id` int(10) unsigned NOT NULL,
  `chest_index` tinyint(3) unsigned NOT NULL,
  `status` enum('PENDING','DELIVERED') NOT NULL DEFAULT 'PENDING',
  `claimed_at` datetime NOT NULL DEFAULT current_timestamp(),
  `delivered_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_player_chest_claim` (`event_id`,`edition_key`,`guild_id`,`player_id`,`sequence_no`,`chest_index`),
  KEY `idx_claim_player_status` (`player_id`,`status`,`edition_key`),
  KEY `idx_claim_guild` (`event_id`,`guild_id`,`edition_key`)
) ENGINE=InnoDB AUTO_INCREMENT=47 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `festival_chest_claims` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `event_id` int(10) unsigned NOT NULL,
  `edition_key` datetime NOT NULL,
  `guild_id` int(10) unsigned NOT NULL,
  `player_id` int(10) unsigned NOT NULL,
  `sequence_no` int(10) unsigned NOT NULL,
  `chest_id` int(10) unsigned NOT NULL,
  `chest_index` tinyint(3) unsigned NOT NULL,
  `status` enum('PENDING','DELIVERED') NOT NULL DEFAULT 'PENDING',
  `claimed_at` datetime NOT NULL DEFAULT current_timestamp(),
  `delivered_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_player_chest_claim` (`event_id`,`edition_key`,`guild_id`,`player_id`,`sequence_no`,`chest_index`),
  KEY `idx_claim_player_status` (`player_id`,`status`,`edition_key`),
  KEY `idx_claim_guild` (`event_id`,`guild_id`,`edition_key`)
) ENGINE=InnoDB AUTO_INCREMENT=47 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*M!100616 SET NOTE_VERBOSITY=@OLD_NOTE_VERBOSITY */;

