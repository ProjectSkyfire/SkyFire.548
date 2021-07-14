-- update ErrorType to appropriate error
UPDATE `conditions` SET `ErrorType`='106' WHERE  `SourceTypeOrReferenceId`=17 AND `SourceGroup`=0 AND `SourceEntry`=36554 AND `SourceId`=0 AND `ElseGroup`=0 AND `ConditionTypeOrReference`=21 AND `ConditionTarget`=0 AND `ConditionValue1`=1024 AND `ConditionValue2`=0 AND `ConditionValue3`=0;

-- not allowed to shadowstep to self
DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId`=17 AND `SourceGroup`=0 AND `SourceEntry`=36554 AND `SourceId`=0 AND `ElseGroup`=0 AND `ConditionTypeOrReference`=33 AND `ConditionTarget`=1 AND `ConditionValue1`=0 AND `ConditionValue2`=0 AND `ConditionValue3`=0;
INSERT INTO `conditions` (`SourceTypeOrReferenceId`, `SourceGroup`, `SourceEntry`, `SourceId`, `ElseGroup`, `ConditionTypeOrReference`, `ConditionTarget`, `ConditionValue1`, `ConditionValue2`, `ConditionValue3`, `NegativeCondition`, `ErrorType`, `ErrorTextId`, `ScriptName`, `Comment`) VALUES (17, 0, 36554, 0, 0, 33, 1, 0, 0, 0, 1, 13, 0, '', 'Shadowstep - prevent cast on self');
