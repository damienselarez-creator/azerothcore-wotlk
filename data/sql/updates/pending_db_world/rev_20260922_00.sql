-- Shared rewards for Horde and Alliance; keep the shared gossip menus unchanged.
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_tharl_gloubie'
WHERE `entry` IN (16076, 2943) AND `AIName` = '' AND `ScriptName` = '';
