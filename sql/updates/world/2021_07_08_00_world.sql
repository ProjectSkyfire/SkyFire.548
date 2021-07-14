DELETE FROM `spell_linked_spell` WHERE `spell_trigger`=34428 AND `spell_effect`=122979 AND `type`=0;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (34428, 122979, 0, 'Victory rush dmg & heal');
DELETE FROM `spell_script_names` WHERE `spell_id`=34428 AND `ScriptName`='spell_warr_victory_rush';
INSERT INTO `spell_script_names` (`spell_id`, `ScriptName`) VALUES (34428, 'spell_warr_victory_rush');

DELETE FROM `spell_linked_spell` WHERE `spell_trigger`=34428 AND `spell_effect`=122979 AND `type`=0;
INSERT INTO `spell_linked_spell` (`spell_trigger`, `spell_effect`, `type`, `comment`) VALUES (34428, 122979, 0, 'Victory rush heal');
