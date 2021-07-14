/*
 * Copyright (C) 2011-2021 Project SkyFire <https://www.projectskyfire.org/>
 * Copyright (C) 2008-2021 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2021 MaNGOS <https://www.getmangos.eu/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_WARRIOR and SPELLFAMILY_GENERIC spells used by warrior players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_warr_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"

enum WarriorSpells
{
    SPELL_WARRIOR_SHOCKWAVE_STUN                    = 132168,
    SPELL_WARRIOR_BLOODTHIRST_DAMAGE                = 23881,
    SPELL_WARRIOR_BLOODTHIRST_HEAL                  = 117313,
    SPELL_WARRIOR_CHARGE                            = 34846,
    SPELL_WARRIOR_CHARGE_STUN                       = 7922,
    SPELL_WARRIOR_COLOSSUS_SMASH                    = 86346,
    SPELL_WARRIOR_TASTE_FOR_BLOOD_AURA              = 56636,
    SPELL_WARRIOR_TASTE_FOR_BLOOD                   = 60503,
    SPELL_WARRIOR_VICTORIOUS                        = 32216,

    SPELL_WARRIOR_LAST_STAND_TRIGGERED              = 12976, // obsolete
    SPELL_WARRIOR_RALLYING_CRY                      = 97463,
    SPELL_WARRIOR_RETALIATION_DAMAGE                = 22858,
    SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_1          = 29834, // obsolete
    SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_2          = 29838,
    SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_1       = 29841, // obsolete
    SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_2       = 29842, // Arms/Fury Passive Unbridled Wrath
    SPELL_WARRIOR_SHIELD_SLAM                       = 23922,
    
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK     = 26654,

    SPELL_WARRIOR_WARBRINGER                        = 103828,
    SPELL_WARRIOR_WARBRINGER_ROOT                   = 105771,
    SPELL_WARRIOR_WARBRINGER_SLOW                   = 137637,
    SPELL_WARRIOR_BLOODSURGE                        = 46915,
    SPELL_WARRIOR_BLOODSURGE_EFFECT                 = 46916,
    SPELL_WARRIOR_THUNDER_CLAP                      = 6343,
    SPELL_WARRIOR_BLOOD_AND_THUNDER                 = 84615,
    SPELL_WARRIOR_DEEP_WOUNDS                       = 115767,
    SPELL_WARRIOR_MORTAL_STRIKE_AURA                = 12294,
    SPELL_WARRIOR_T16_DPS_4P_BONUS                  = 144441,
    SPELL_WARRIOR_T16_DPS_4P_BONUS_PROC             = 144442
};

enum WarriorSpellIcons
{
    WARRIOR_ICON_ID_SUDDEN_DEATH                    = 1989
};


enum MiscSpells
{
    SPELL_PALADIN_BLESSING_OF_SANCTUARY             = 20911,
    SPELL_PALADIN_GREATER_BLESSING_OF_SANCTUARY     = 25899,
    SPELL_PRIEST_RENEWED_HOPE                       = 63944
};

// Shockwave - 46968
class spell_warr_shockwave : public SpellScriptLoader
{
public:
    spell_warr_shockwave() : SpellScriptLoader("spell_warr_shockwave") { }

    class spell_warr_shockwave_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_shockwave_SpellScript);

        void CountTargets(std::list<WorldObject*>& targets)
        {
            targetCount = targets.size();
        }

        void HandleStun(SpellEffIndex /*effIndex*/)
        {
            GetCaster()->CastSpell(GetHitUnit(), SPELL_WARRIOR_SHOCKWAVE_STUN, true);
        }

        // Cooldown reduced by 20 sec. if it hits 3 targets
        void HandleAfterCast()
        {
            if (targetCount >= GetSpellInfo()->Effects[EFFECT_0].BasePoints)
                GetCaster()->ToPlayer()->ModifySpellCooldown(GetSpellInfo()->Id, -GetSpellInfo()->Effects[EFFECT_3].BasePoints * IN_MILLISECONDS);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_shockwave_SpellScript::HandleStun, EFFECT_0, SPELL_EFFECT_DUMMY);
            AfterCast += SpellCastFn(spell_warr_shockwave_SpellScript::HandleAfterCast);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_shockwave_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CONE_ENEMY_104);
        }

    private:
        int32 targetCount = 0;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_shockwave_SpellScript();
    }
};

// Bloodthirst - 23881
class spell_warr_bloodthirst : public SpellScriptLoader
{
public:
    spell_warr_bloodthirst() : SpellScriptLoader("spell_warr_bloodthirst") { }

    class spell_warr_bloodthirst_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_bloodthirst_SpellScript);

        bool Validate(SpellInfo const* /*SpellEntry*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_BLOODTHIRST_DAMAGE) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_BLOODTHIRST_HEAL))
                return false;
            return true;
        }
        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
                if (Unit* target = GetHitUnit())
                    if (GetHitDamage())
                    {
                        _player->CastSpell(_player, SPELL_WARRIOR_BLOODTHIRST_HEAL, true);
                        if (_player->HasAura(SPELL_WARRIOR_BLOODSURGE))
                            if (roll_chance_i(20))
                                _player->CastSpell(_player, SPELL_WARRIOR_BLOODSURGE_EFFECT, true);
                    }
        }

        void Register() OVERRIDE
        {
            OnHit += SpellHitFn(spell_warr_bloodthirst_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_bloodthirst_SpellScript();
    }
};

/// Updated 4.3.4
class spell_warr_charge : public SpellScriptLoader
{
public:
    spell_warr_charge() : SpellScriptLoader("spell_warr_charge") { }

    class spell_warr_charge_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_charge_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_WARBRINGER_ROOT) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_WARBRINGER_SLOW) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_WARBRINGER) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_CHARGE_STUN) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_CHARGE))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            int32 chargeBasePoints0 = GetEffectValue();
            Unit* caster = GetCaster();
            caster->CastCustomSpell(caster, SPELL_WARRIOR_CHARGE, &chargeBasePoints0, NULL, NULL, true);
        }

        void HandleCharge(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetHitUnit())
            {
                if (GetCaster()->HasAura(SPELL_WARRIOR_WARBRINGER))
                {
                    GetCaster()->CastSpell(target, SPELL_WARRIOR_WARBRINGER_ROOT, true);
                    GetCaster()->CastSpell(target, SPELL_WARRIOR_WARBRINGER_SLOW, true);
                }
                else
                    GetCaster()->CastSpell(target, SPELL_WARRIOR_CHARGE_STUN, true);
            }
        }


        void Register() OVERRIDE
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            OnEffectHitTarget += SpellEffectFn(spell_warr_charge_SpellScript::HandleCharge, EFFECT_0, SPELL_EFFECT_CHARGE);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_charge_SpellScript();
    }
};

/// Updated 4.3.4
class spell_warr_concussion_blow : public SpellScriptLoader
{
public:
    spell_warr_concussion_blow() : SpellScriptLoader("spell_warr_concussion_blow") { }

    class spell_warr_concussion_blow_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_concussion_blow_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            SetHitDamage(CalculatePct(GetCaster()->GetTotalAttackPowerValue(WeaponAttackType::BASE_ATTACK), GetEffectValue()));
        }

        void Register() OVERRIDE
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_concussion_blow_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_concussion_blow_SpellScript();
    }
};

// Called By Thunder Clap - 6343, Mortal Strike - 12294, Bloodthirst - 23881 and Devastate - 20243
// Deep Wounds - 115767
class spell_warr_deep_wounds : public SpellScriptLoader
{
public:
    spell_warr_deep_wounds() : SpellScriptLoader("spell_warr_deep_wounds") { }

    class spell_warr_deep_wounds_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_deep_wounds_SpellScript);

        void HandleOnHit()
        {
            if (Player* _player = GetCaster()->ToPlayer())
            {
                if (Unit* target = GetHitUnit())
                {
                    if (target->GetGUID() == _player->GetGUID())
                        return;

                    if (_player->getLevel() >= 32)
                        if (GetSpellInfo()->Id != SPELL_WARRIOR_THUNDER_CLAP || _player->HasAura(SPELL_WARRIOR_BLOOD_AND_THUNDER))
                            _player->CastSpell(target, SPELL_WARRIOR_DEEP_WOUNDS, true);

                    // Item - Warrior T16 DPS 4P Bonus - 144441
                    if (GetSpellInfo()->Id == SPELL_WARRIOR_MORTAL_STRIKE_AURA || GetSpellInfo()->Id == SPELL_WARRIOR_BLOODTHIRST_DAMAGE)
                        if (_player->HasAura(SPELL_WARRIOR_T16_DPS_4P_BONUS))
                            if (roll_chance_i(10))
                                _player->CastSpell(_player, SPELL_WARRIOR_T16_DPS_4P_BONUS_PROC, true);
                }
            }
        }

        void Register() OVERRIDE
        {
            OnHit += SpellHitFn(spell_warr_deep_wounds_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_deep_wounds_SpellScript();
    }
};

/// Updated 4.3.4
class spell_warr_execute : public SpellScriptLoader
{
public:
    spell_warr_execute() : SpellScriptLoader("spell_warr_execute") { }

    class spell_warr_execute_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_execute_SpellScript);

        void HandleEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (GetHitUnit())
            {
                /*
                SpellInfo const* spellInfo = GetSpellInfo();
                int32 rageUsed = std::min<int32>(200 - spellInfo->CalcPowerCost(caster, SpellSchoolMask(spellInfo->SchoolMask)), caster->GetPower(POWER_RAGE));
                int32 newRage = std::max<int32>(0, caster->GetPower(POWER_RAGE) - rageUsed);

                // Sudden Death rage save
                if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_PROC_TRIGGER_SPELL, SPELLFAMILY_GENERIC, WARRIOR_ICON_ID_SUDDEN_DEATH, EFFECT_0))
                {
                    int32 ragesave = aurEff->GetSpellInfo()->Effects[EFFECT_0].CalcValue() * 10;
                    newRage = std::max(newRage, ragesave);
                }

                caster->SetPower(POWER_RAGE, uint32(newRage));

                /// Formula taken from the DBC: "${10+$AP*0.437*$m1/100}"
                int32 baseDamage = int32(10 + caster->GetTotalAttackPowerValue(WeaponAttackType::BASE_ATTACK) * 0.437f * GetEffectValue() / 100.0f);
                /// Formula taken from the DBC: "${$ap*0.874*$m1/100-1} = 20 rage"
                int32 moreDamage = int32(rageUsed * (caster->GetTotalAttackPowerValue(WeaponAttackType::BASE_ATTACK) * 0.874f * GetEffectValue() / 100.0f - 1) / 200);
                SetHitDamage(baseDamage + moreDamage);
                */
            }
        }

        void Register() OVERRIDE
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_execute_SpellScript::HandleEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_execute_SpellScript();
    }
};

// 59725 - Improved Spell Reflection
class spell_warr_improved_spell_reflection : public SpellScriptLoader
{
public:
    spell_warr_improved_spell_reflection() : SpellScriptLoader("spell_warr_improved_spell_reflection") { }

    class spell_warr_improved_spell_reflection_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_improved_spell_reflection_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            if (GetCaster())
                unitList.remove(GetCaster());
        }

        void Register() OVERRIDE
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_improved_spell_reflection_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_improved_spell_reflection_SpellScript();
    }
};

// 5246 - Intimidating Shout
class spell_warr_intimidating_shout : public SpellScriptLoader
{
public:
    spell_warr_intimidating_shout() : SpellScriptLoader("spell_warr_intimidating_shout") { }

    class spell_warr_intimidating_shout_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_intimidating_shout_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            unitList.remove(GetExplTargetWorldObject());
        }

        void Register() OVERRIDE
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_warr_intimidating_shout_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_intimidating_shout_SpellScript();
    }
};

/// Updated 4.3.4
// 12975 - Last Stand
class spell_warr_last_stand : public SpellScriptLoader
{
public:
    spell_warr_last_stand() : SpellScriptLoader("spell_warr_last_stand") { }

    class spell_warr_last_stand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_last_stand_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_LAST_STAND_TRIGGERED))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(GetEffectValue()));
            caster->CastCustomSpell(caster, SPELL_WARRIOR_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
        }

        void Register() OVERRIDE
        {
            // add dummy effect spell handler to Last Stand
            OnEffectHit += SpellEffectFn(spell_warr_last_stand_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_last_stand_SpellScript();
    }
};

// 97462 - Rallying Cry
class spell_warr_rallying_cry : public SpellScriptLoader
{
public:
    spell_warr_rallying_cry() : SpellScriptLoader("spell_warr_rallying_cry") { }

    class spell_warr_rallying_cry_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_rallying_cry_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_RALLYING_CRY))
                return false;
            return true;
        }

        bool Load() OVERRIDE
        {
            return GetCaster()->GetTypeId() == TypeID::TYPEID_PLAYER;
        }

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            int32 basePoints0 = int32(GetHitUnit()->CountPctFromMaxHealth(GetEffectValue()));

            GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_WARRIOR_RALLYING_CRY, &basePoints0, NULL, NULL, true);
        }

        void Register() OVERRIDE
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_rallying_cry_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_rallying_cry_SpellScript();
    }
};

// 94009 - Rend
class spell_warr_rend : public SpellScriptLoader
{
public:
    spell_warr_rend() : SpellScriptLoader("spell_warr_rend") { }

    class spell_warr_rend_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_rend_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
        {
            if (Unit* caster = GetCaster())
            {
                canBeRecalculated = false;

                // $0.25 * (($MWB + $mwb) / 2 + $AP / 14 * $MWS) bonus per tick
                float ap = caster->GetTotalAttackPowerValue(WeaponAttackType::BASE_ATTACK);
                int32 mws = caster->GetAttackTime(WeaponAttackType::BASE_ATTACK);
                float mwbMin = caster->GetWeaponDamageRange(WeaponAttackType::BASE_ATTACK, WeaponDamageRange::MINDAMAGE);
                float mwbMax = caster->GetWeaponDamageRange(WeaponAttackType::BASE_ATTACK, WeaponDamageRange::MAXDAMAGE);
                float mwb = ((mwbMin + mwbMax) / 2 + ap * mws / 14000) * 0.25f;
                amount += int32(caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), mwb));
            }
        }

        void Register() OVERRIDE
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_rend_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_rend_AuraScript();
    }
};

// 20230 - Retaliation
class spell_warr_retaliation : public SpellScriptLoader
{
public:
    spell_warr_retaliation() : SpellScriptLoader("spell_warr_retaliation") { }

    class spell_warr_retaliation_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_retaliation_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_RETALIATION_DAMAGE))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            // check attack comes not from behind
            return GetTarget()->isInFront(eventInfo.GetProcTarget(), M_PI);
        }

        void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_WARRIOR_RETALIATION_DAMAGE, true, NULL, aurEff);
        }

        void Register() OVERRIDE
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_retaliation_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warr_retaliation_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_retaliation_AuraScript();
    }
};

// 64380, 65941 - Shattering Throw
class spell_warr_shattering_throw : public SpellScriptLoader
{
public:
    spell_warr_shattering_throw() : SpellScriptLoader("spell_warr_shattering_throw") { }

    class spell_warr_shattering_throw_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_shattering_throw_SpellScript);

        void HandleScript(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            // remove shields, will still display immune to damage part
            if (Unit* target = GetHitUnit())
                target->RemoveAurasWithMechanic(1 << MECHANIC_IMMUNE_SHIELD, AURA_REMOVE_BY_ENEMY_SPELL);
        }

        void Register() OVERRIDE
        {
            OnEffectHitTarget += SpellEffectFn(spell_warr_shattering_throw_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const OVERRIDE
    {
        return new spell_warr_shattering_throw_SpellScript();
    }
};

class spell_warr_second_wind_proc : public SpellScriptLoader
{
public:
    spell_warr_second_wind_proc() : SpellScriptLoader("spell_warr_second_wind_proc") { }

    class spell_warr_second_wind_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_second_wind_proc_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_1) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_2) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_1) ||
                !sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_2))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            if (eventInfo.GetProcTarget() == GetTarget())
                return false;
            if (!eventInfo.GetDamageInfo()->GetSpellInfo() || !(eventInfo.GetDamageInfo()->GetSpellInfo()->GetAllEffectsMechanicMask() & ((1 << MECHANIC_ROOT) | (1 << MECHANIC_STUN))))
                return false;
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            uint32 spellId = 0;

            if (GetSpellInfo()->Id == SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_1)
                spellId = SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_1;
            else if (GetSpellInfo()->Id == SPELL_WARRIOR_SECOUND_WIND_PROC_RANK_2)
                spellId = SPELL_WARRIOR_SECOUND_WIND_TRIGGER_RANK_2;
            if (!spellId)
                return;

            GetTarget()->CastSpell(GetTarget(), spellId, true, NULL, aurEff);
        }

        void Register() OVERRIDE
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_second_wind_proc_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warr_second_wind_proc_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_second_wind_proc_AuraScript();
    }
};

class spell_warr_second_wind_trigger : public SpellScriptLoader
{
public:
    spell_warr_second_wind_trigger() : SpellScriptLoader("spell_warr_second_wind_trigger") { }

    class spell_warr_second_wind_trigger_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_second_wind_trigger_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            amount = int32(GetUnitOwner()->CountPctFromMaxHealth(amount));
        }

        void Register() OVERRIDE
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_warr_second_wind_trigger_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_second_wind_trigger_AuraScript();
    }
};

// 52437 - Sudden Death
class spell_warr_sudden_death : public SpellScriptLoader
{
public:
    spell_warr_sudden_death() : SpellScriptLoader("spell_warr_sudden_death") { }

    class spell_warr_sudden_death_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_sudden_death_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_COLOSSUS_SMASH))
                return false;
            return true;
        }

        void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            // Remove cooldown on Colossus Smash
            if (Player* player = GetTarget()->ToPlayer())
                player->RemoveSpellCooldown(SPELL_WARRIOR_COLOSSUS_SMASH, true);
        }

        void Register() OVERRIDE
        {
            AfterEffectApply += AuraEffectRemoveFn(spell_warr_sudden_death_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL); // correct?
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_sudden_death_AuraScript();
    }
};

// 12328, 18765, 35429 - Sweeping Strikes
class spell_warr_sweeping_strikes : public SpellScriptLoader
{
public:
    spell_warr_sweeping_strikes() : SpellScriptLoader("spell_warr_sweeping_strikes") { }

    class spell_warr_sweeping_strikes_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_sweeping_strikes_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK))
                return false;
            return true;
        }

        bool Load() OVERRIDE
        {
            _procTarget = NULL;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            _procTarget = eventInfo.GetActor()->SelectNearbyTarget(eventInfo.GetProcTarget());
            return _procTarget;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(_procTarget, SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK, true, NULL, aurEff);
        }

        void Register() OVERRIDE
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_sweeping_strikes_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warr_sweeping_strikes_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }

    private:
        Unit* _procTarget;
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_sweeping_strikes_AuraScript();
    }
};

// -46951 - Sword and Board
class spell_warr_sword_and_board : public SpellScriptLoader
{
public:
    spell_warr_sword_and_board() : SpellScriptLoader("spell_warr_sword_and_board") { }

    class spell_warr_sword_and_board_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_sword_and_board_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) OVERRIDE
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_SHIELD_SLAM))
                return false;
            return true;
        }

        void OnProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            // Remove cooldown on Shield Slam
            if (Player* player = GetTarget()->ToPlayer())
                player->RemoveSpellCooldown(SPELL_WARRIOR_SHIELD_SLAM, true);
        }

        void Register() OVERRIDE
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_sword_and_board_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_sword_and_board_AuraScript();
    }
};

// 32216 - Victorious
class spell_warr_victorious : public SpellScriptLoader
{
public:
    spell_warr_victorious() : SpellScriptLoader("spell_warr_victorious") { }

    class spell_warr_victorious_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warr_victorious_AuraScript);

        void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->RemoveAura(GetId());
        }

        void Register() OVERRIDE
        {
            OnEffectProc += AuraEffectProcFn(spell_warr_victorious_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const OVERRIDE
    {
        return new spell_warr_victorious_AuraScript();
    }
};

// 60503 - Taste for Blood
class spell_warr_taste_for_blood : public SpellScriptLoader
{
public:
    spell_warr_taste_for_blood() : SpellScriptLoader("spell_warr_taste_for_blood") { }

    class spell_warr_taste_for_blood_AuraScript: public AuraScript
    {
        PrepareAuraScript(spell_warr_taste_for_blood_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_TASTE_FOR_BLOOD))
                return false;
            return true;
        }

        int32 stacks;

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            auto const caster = eventInfo.GetActor();
            auto const target = eventInfo.GetActionTarget();

            if (!caster || !target || caster == target)
                return false;

            if (eventInfo.GetHitMask() & PROC_EX_DODGE)
            {
                stacks = GetSpellInfo()->Effects[EFFECT_0].BasePoints;
                return true;
            }

            auto const spellInfo = eventInfo.GetSpellInfo();
            if (spellInfo && spellInfo->Id == SPELL_WARRIOR_MORTAL_STRIKE_AURA)
            {
                stacks = GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                return true;
            }

            return false;
        }

        void HandleEffectProc(AuraEffect const*, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();

            CustomSpellValues values;
            values.AddSpellMod(SPELLVALUE_AURA_STACK, stacks);

            auto const caster = eventInfo.GetActor();
            caster->CastCustomSpell(SPELL_WARRIOR_TASTE_FOR_BLOOD, values, caster, TRIGGERED_FULL_MASK);
        }

        void Register()
        {
            DoCheckProc += AuraCheckProcFn(spell_warr_taste_for_blood_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_warr_taste_for_blood_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_warr_taste_for_blood_AuraScript;
    }
};

// 12294 - Mortal strike
class spell_warr_mortal_strike : public SpellScriptLoader
{
public:
    spell_warr_mortal_strike() : SpellScriptLoader("spell_warr_mortal_strike") { }

    class spell_warr_mortal_strike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_mortal_strike_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_TASTE_FOR_BLOOD))
                return false;
            return true;
        }

        void HandleOnHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
            {
                // fix taste for blood charge on hit with mortal strike
                if (player->HasAura(SPELL_WARRIOR_TASTE_FOR_BLOOD_AURA))
                {
                    player->StartReactiveTimer(REACTIVE_OVERPOWER);
                    player->CastSpell(player, SPELL_WARRIOR_TASTE_FOR_BLOOD, true);
                }
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_warr_mortal_strike_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_mortal_strike_SpellScript();
    }
};

// Called by Overpower - 7384
// Taste for Blood - 56638
class spell_warr_overpower : public SpellScriptLoader
{
public:
    spell_warr_overpower() : SpellScriptLoader("spell_warr_overpower") { }

    class spell_warr_overpower_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_overpower_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_TASTE_FOR_BLOOD))
                return false;
            return true;
        }

        void HandleOnHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
            {
                if (player->HasAura(SPELL_WARRIOR_TASTE_FOR_BLOOD_AURA))
                {
                    if (Aura* overPower = player->GetAura(SPELL_WARRIOR_TASTE_FOR_BLOOD))
                    {
                        if (overPower->GetStackAmount() > 1)
                            overPower->SetStackAmount(overPower->GetStackAmount() - 1);
                        else
                            player->RemoveAura(SPELL_WARRIOR_TASTE_FOR_BLOOD);
                    }
                }
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_warr_overpower_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warr_overpower_SpellScript();
    }
};

// 34428 - Victory Rush
class spell_warr_victory_rush : public SpellScriptLoader
{
public:
    spell_warr_victory_rush() : SpellScriptLoader("spell_warr_victory_rush") { }

    class spell_warr_victory_rush_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warr_victory_rush_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARRIOR_VICTORIOUS))
                return false;
            return true;
        }

        void HandleOnHit()
        {
            if (Unit* caster = GetCaster()->ToPlayer())
            {
                caster->RemoveAura(SPELL_WARRIOR_VICTORIOUS);
            }
        }

        void Register() OVERRIDE
        {
            OnHit += SpellHitFn(spell_warr_victory_rush_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
       return new spell_warr_victory_rush_SpellScript();
    }
};

void AddSC_warrior_spell_scripts()
{
    new spell_warr_bloodthirst();
    new spell_warr_charge();
    new spell_warr_concussion_blow();
    new spell_warr_deep_wounds();
    new spell_warr_execute();
    new spell_warr_improved_spell_reflection();
    new spell_warr_intimidating_shout();
    new spell_warr_last_stand();
    new spell_warr_rallying_cry();
    new spell_warr_rend();
    new spell_warr_retaliation();
    new spell_warr_second_wind_proc();
    new spell_warr_second_wind_trigger();
    new spell_warr_shattering_throw();
    new spell_warr_shockwave();
    new spell_warr_sudden_death();
    new spell_warr_sweeping_strikes();
    new spell_warr_sword_and_board();
    new spell_warr_victorious();
    new spell_warr_taste_for_blood();
    new spell_warr_mortal_strike();
    new spell_warr_overpower();
    new spell_warr_victory_rush();
}
