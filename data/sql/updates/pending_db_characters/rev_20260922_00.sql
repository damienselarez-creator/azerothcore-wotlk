-- Persistent receipt: never delete on character deletion or on script rollback.
CREATE TABLE IF NOT EXISTS `custom_tharl_reward_claim` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `account_id` int unsigned NOT NULL,
  `item_entry` int unsigned NOT NULL,
  `character_guid` int unsigned NOT NULL,
  `mail_id` int unsigned NOT NULL,
  `claimed_at` int unsigned NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uq_account_reward` (`account_id`, `item_entry`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
