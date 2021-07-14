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
 * Scripts for spells with SPELLFAMILY_DRUID and SPELLFAMILY_GENERIC spells used by druid players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_dru_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Containers.h"

enum DruidSpells
{
    SPELL_DRUID_WRATH                       = 5176,
    SPELL_DRUID_STARFIRE                    = 2912,
    SPELL_DRUID_STARSURGE                   = 78674,
    SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE    = 89265,
    SPELL_DRUID_STARSURGE_ENERGIZE          = 86605,
    SPELL_DRUID_LUNAR_ECLIPSE_MARKER        = 67484, // Will make the yellow arrow on eclipse bar point to the blue side (lunar)
    SPELL_DRUID_SOLAR_ECLIPSE_MARKER        = 67483, // Will make the yellow arrow on eclipse bar point to the yellow side (solar)
    SPELL_DRUID_SOLAR_ECLIPSE               = 48517,
    SPELL_DRUID_LUNAR_ECLIPSE               = 48518,
    SPELL_DRUID_FERAL_CHARGE_BEAR           = 16979,
    SPELL_DRUID_FERAL_CHARGE_CAT            = 49376,
    SPELL_DRUID_GLYPH_OF_INNERVATE          = 54833,
    SPELL_DRUID_GLYPH_OF_STARFIRE           = 54846,
    SPELL_DRUID_INCREASED_MOONFIRE_DURATION = 38414,
    SPELL_DRUID_LIFEBLOOM_ENERGIZE          = 64372,
    SPELL_DRUID_LIFEBLOOM_FINAL_HEAL        = 33778,
    SPELL_DRUID_LIVING_SEED_HEAL            = 48503,
    SPELL_DRUID_LIVING_SEED_PROC            = 48504,
    SPELL_DRUID_NATURES_GRACE               = 16880,
    SPELL_DRUID_NATURES_GRACE_TRIGGER       = 16886,
    SPELL_DRUID_SURVIVAL_INSTINCTS          = 50322,
    SPELL_DRUID_SAVAGE_ROAR                 = 62071,
    SPELL_DRUID_STAMPEDE_BAER_RANK_1        = 81016,
    SPELL_DRUID_STAMPEDE_CAT_RANK_1         = 81021,
    SPELL_DRUID_STAMPEDE_CAT_STATE          = 109881,
    SPELL_DRUID_TIGER_S_FURY_ENERGIZE       = 51178,
    SPELL_DRUID_PROWL                       = 5215,
    SPELL_DRUID_CAT_FORM                    = 768,
    SPELL_DRUID_DASH                        = 1850,
    SPELL_DRUID_LIFEBLOOM                   = 33763,
    SPELL_DRUID_GLYPH_OF_BLOOMING           = 121840,
    SPELL_DRUID_PREDATORY_SWIFTNESS_TRIGGER = 69369,
};

// Called by Regrowth - 8936, Nourish - 50464, Healing Touch - 5185
// Lifebloom - 33763 refresh duration
class spell_dru_lifebloom_refresh : public SpellScriptLoader
{
    public:
        spell_dru_lifebloom_refresh() : SpellScriptLoader("spell_dru_lifebloom_refresh") { }

        class spell_dru_lifebloom_refresh_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dru_lifebloom_refresh_SpellScript)

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_LIFEBLOOM))
                    return false;
                return true;
            }

            void HandleOnHit()
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!target)
                    return;

                if (!caster->HasAura(SPELL_DRUID_GLYPH_OF_BLOOMING))
                {
                    if (Aura* lifeBloom = target->GetAura(SPELL_DRUID_LIFEBLOOM, caster->GetGUID()))
                    {
                        lifeBloom->RefreshDuration();
                    }
                }
            }
            
            void Register() override
            {
                OnHit += SpellHitFn(spell_dru_lifebloom_refresh_SpellScript::HandleOnHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dru_lifebloom_refresh_SpellScript();
        }
};

// Cat Form - 768
class spell_dru_cat_form : public SpellScriptLoader
{
public:
    spell_dru_cat_form() : SpellScriptLoader("spell_dru_cat_form") { }

    class spell_dru_cat_form_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_cat_form_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_PROWL))
                return false;
            return true;
        }

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetTarget();

            SpellInfo const* dash = sSpellMgr->GetSpellInfo(SPELL_DRUID_DASH);
            if (!dash)
                return;

            // if we have dash, restore increased movement speed
            if (AuraEffect* dashAura = caster->GetAuraEffect(SPELL_DRUID_DASH, EFFECT_0))
                dashAura->SetAmount(dash->Effects[EFFECT_0].BasePoints);
        }


        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            // remove prowl on leaving catform
            if (Unit* caster = GetTarget())
            {
                caster->RemoveOwnedAura(SPELL_DRUID_PROWL);

                // remove dash leaving catform. temp. fix
                if (AuraEffect* dashAura = caster->GetAuraEffect(SPELL_DRUID_DASH, EFFECT_0))
                    dashAura->SetAmount(0);
            }

            
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_dru_cat_form_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_dru_cat_form_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dru_cat_form_AuraScript();
    }
};

// 1850 - Dash
class spell_dru_dash : public SpellScriptLoader
{
public:
    spell_dru_dash() : SpellScriptLoader("spell_dru_dash") { }

    class spell_dru_dash_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_dash_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            // do not set speed if not in cat form
            if (GetUnitOwner()->GetShapeshiftForm() != FORM_CAT)
                amount = 0;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_dash_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_INCREASE_SPEED);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_dash_AuraScript();
    }

    class spell_dru_dash_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_dash_SpellScript);

        void HandleOnHit()
        {
            if (Player* player = GetCaster()->ToPlayer())
                player->RemoveMovementImpairingAuras();
        }


        void Register() override
        {
            OnHit += SpellHitFn(spell_dru_dash_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_dash_SpellScript();
    }
};

// 48517 - Eclipse (Solar)
// 48518 - Eclipse (Lunar)
class spell_dru_eclipse : public SpellScriptLoader
{
public:
    spell_dru_eclipse(char const* scriptName) : SpellScriptLoader(scriptName) { }

    class spell_dru_eclipse_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_eclipse_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_NATURES_GRACE) ||
                !sSpellMgr->GetSpellInfo(SPELL_DRUID_NATURES_GRACE_TRIGGER))
                return false;
            return true;
        }

        bool Load() override
        {
            return GetCaster() && GetCaster()->GetTypeId() == TypeID::TYPEID_PLAYER;
        }

        void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            if (caster->ToPlayer()->GetAuraOfRankedSpell(SPELL_DRUID_NATURES_GRACE))
                caster->ToPlayer()->RemoveSpellCooldown(SPELL_DRUID_NATURES_GRACE_TRIGGER, true);
        }

        void Register() override
        {
            OnEffectApply += AuraEffectApplyFn(spell_dru_eclipse_AuraScript::ApplyEffect, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_eclipse_AuraScript();
    }
};

// 2912, 5176, 78674 - Starfire, Wrath, and Starsurge
class spell_dru_eclipse_energize : public SpellScriptLoader
{
public:
    spell_dru_eclipse_energize() : SpellScriptLoader("spell_dru_eclipse_energize") { }

    class spell_dru_eclipse_energize_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_eclipse_energize_SpellScript);

        int32 energizeAmount;

        bool Load() override
        {
            if (GetCaster()->GetTypeId() != TypeID::TYPEID_PLAYER)
                return false;

            if (GetCaster()->ToPlayer()->getClass() != CLASS_DRUID)
                return false;

            energizeAmount = 0;

            return true;
        }

        void HandleEnergize(SpellEffIndex effIndex)
        {
            Player* caster = GetCaster()->ToPlayer();

            // No boomy, no deal.
            if (caster->GetTalentSpecialization(caster->GetActiveSpec()) != TALENT_TREE_DRUID_BALANCE)
                return;

            switch (GetSpellInfo()->Id)
            {
            case SPELL_DRUID_WRATH:
            {
                energizeAmount = -GetSpellInfo()->Effects[effIndex].BasePoints; // -13
                // If we are set to fill the lunar side or we've just logged in with 0 power..
                if ((!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                    || caster->GetPower(POWER_ECLIPSE) == 0)
                {
                    caster->CastCustomSpell(caster, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, &energizeAmount, 0, 0, true);
                    // If the energize was due to 0 power, cast the eclipse marker aura
                    if (!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_LUNAR_ECLIPSE_MARKER, true);
                }
                // The energizing effect brought us out of the solar eclipse, remove the aura
                if (caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) <= 0)
                    caster->RemoveAurasDueToSpell(SPELL_DRUID_SOLAR_ECLIPSE);
                break;
            }
            case SPELL_DRUID_STARFIRE:
            {
                energizeAmount = GetSpellInfo()->Effects[effIndex].BasePoints; // 20
                // If we are set to fill the solar side or we've just logged in with 0 power..
                if ((!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                    || caster->GetPower(POWER_ECLIPSE) == 0)
                {
                    caster->CastCustomSpell(caster, SPELL_DRUID_ECLIPSE_GENERAL_ENERGIZE, &energizeAmount, 0, 0, true);
                    // If the energize was due to 0 power, cast the eclipse marker aura
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_SOLAR_ECLIPSE_MARKER, true);
                }
                // The energizing effect brought us out of the lunar eclipse, remove the aura
                if (caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) >= 0)
                    caster->RemoveAura(SPELL_DRUID_LUNAR_ECLIPSE);
                break;
            }
            case SPELL_DRUID_STARSURGE:
            {
                // If we are set to fill the solar side or we've just logged in with 0 power (confirmed with sniffs)
                if ((!caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                    || caster->GetPower(POWER_ECLIPSE) == 0)
                {
                    energizeAmount = GetSpellInfo()->Effects[effIndex].BasePoints; // 15
                    caster->CastCustomSpell(caster, SPELL_DRUID_STARSURGE_ENERGIZE, &energizeAmount, 0, 0, true);

                    // If the energize was due to 0 power, cast the eclipse marker aura
                    if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER))
                        caster->CastSpell(caster, SPELL_DRUID_SOLAR_ECLIPSE_MARKER, true);
                }
                else if (!caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE_MARKER) && caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE_MARKER))
                {
                    energizeAmount = -GetSpellInfo()->Effects[effIndex].BasePoints; // -15
                    caster->CastCustomSpell(caster, SPELL_DRUID_STARSURGE_ENERGIZE, &energizeAmount, 0, 0, true);
                }
                // The energizing effect brought us out of the lunar eclipse, remove the aura
                if (caster->HasAura(SPELL_DRUID_LUNAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) >= 0)
                    caster->RemoveAura(SPELL_DRUID_LUNAR_ECLIPSE);
                // The energizing effect brought us out of the solar eclipse, remove the aura
                else if (caster->HasAura(SPELL_DRUID_SOLAR_ECLIPSE) && caster->GetPower(POWER_ECLIPSE) <= 0)
                    caster->RemoveAura(SPELL_DRUID_SOLAR_ECLIPSE);
                break;
            }
            }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_eclipse_energize_SpellScript::HandleEnergize, EFFECT_1, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_eclipse_energize_SpellScript;
    }
};

// 54832 - Glyph of Innervate
class spell_dru_glyph_of_innervate : public SpellScriptLoader
{
public:
    spell_dru_glyph_of_innervate() : SpellScriptLoader("spell_dru_glyph_of_innervate") { }

    class spell_dru_glyph_of_innervate_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_glyph_of_innervate_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_GLYPH_OF_INNERVATE))
                return false;
            return true;
        }

        bool CheckProc(ProcEventInfo& eventInfo)
        {
            // Not proc from self Innervate
            return GetTarget() != eventInfo.GetProcTarget();
        }

        void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_GLYPH_OF_INNERVATE, true, NULL, aurEff);
        }

        void Register() override
        {
            DoCheckProc += AuraCheckProcFn(spell_dru_glyph_of_innervate_AuraScript::CheckProc);
            OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_innervate_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_glyph_of_innervate_AuraScript();
    }
};

// 54846 - Glyph of Starfire
class spell_dru_glyph_of_starfire : public SpellScriptLoader
{
public:
    spell_dru_glyph_of_starfire() : SpellScriptLoader("spell_dru_glyph_of_starfire") { }

    class spell_dru_glyph_of_starfire_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_glyph_of_starfire_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_INCREASED_MOONFIRE_DURATION))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (Unit* unitTarget = GetHitUnit())
                if (AuraEffect const* aurEff = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, 0x2, 0, 0, caster->GetGUID()))
                {
                    Aura* aura = aurEff->GetBase();

                    uint32 countMin = aura->GetMaxDuration();
                    uint32 countMax = aura->GetSpellInfo()->GetMaxDuration() + 9000;
                    if (caster->HasAura(SPELL_DRUID_INCREASED_MOONFIRE_DURATION))
                        countMax += 3000;

                    if (countMin < countMax)
                    {
                        aura->SetDuration(uint32(aura->GetDuration() + 3000));
                        aura->SetMaxDuration(countMin + 3000);
                    }
                }
        }

        void Register() override
        {
            OnEffectHitTarget += SpellEffectFn(spell_dru_glyph_of_starfire_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_glyph_of_starfire_SpellScript();
    }
};

// 54845 - Glyph of Starfire
class spell_dru_glyph_of_starfire_proc : public SpellScriptLoader
{
public:
    spell_dru_glyph_of_starfire_proc() : SpellScriptLoader("spell_dru_glyph_of_starfire_proc") { }

    class spell_dru_glyph_of_starfire_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_glyph_of_starfire_proc_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_GLYPH_OF_STARFIRE))
                return false;
            return true;
        }

        void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_DRUID_GLYPH_OF_STARFIRE, true, NULL, aurEff);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_glyph_of_starfire_proc_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_glyph_of_starfire_proc_AuraScript();
    }
};

// 29166 - Innervate
class spell_dru_innervate : public SpellScriptLoader
{
public:
    spell_dru_innervate() : SpellScriptLoader("spell_dru_innervate") { }

    class spell_dru_innervate_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_innervate_AuraScript);

        void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
        {
            amount = CalculatePct(int32(GetUnitOwner()->GetCreatePowers(POWER_MANA) / aurEff->GetTotalTicks()), amount);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_innervate_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_innervate_AuraScript();
    }
};

// 33763 - Lifebloom
class spell_dru_lifebloom : public SpellScriptLoader
{
public:
    spell_dru_lifebloom() : SpellScriptLoader("spell_dru_lifebloom") { }

    class spell_dru_lifebloom_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_lifebloom_AuraScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL))
                return false;
            return true;
        }

        void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            // Final heal only on duration end
            if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                return;

            if (!GetCaster())
                return;

            if (GetCaster()->ToPlayer()->HasSpellCooldown(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL))
                return;


            // final heal
            int32 stack = GetStackAmount();
            int32 healAmount = aurEff->GetAmount();
            if (Player* caster = GetCaster()->ToPlayer())
            {
                healAmount = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), healAmount, HEAL, stack);
                healAmount = GetTarget()->SpellHealingBonusTaken(caster, GetSpellInfo(), healAmount, HEAL, stack);

                if (caster->HasAura(SPELL_DRUID_GLYPH_OF_BLOOMING))
                    AddPct(healAmount, 50);

                GetTarget()->CastCustomSpell(GetTarget(), SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, NULL, NULL, true, NULL, aurEff, GetCasterGUID());

                caster->AddSpellCooldown(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, 0, 1 * IN_MILLISECONDS);

                return;
            }

            // Increase final heal by 50%
            if (GetCaster()->HasAura(SPELL_DRUID_GLYPH_OF_BLOOMING))
                AddPct(healAmount, 50);

            GetTarget()->CastCustomSpell(GetTarget(), SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, NULL, NULL, true, NULL, aurEff, GetCasterGUID());
            GetCaster()->ToPlayer()->AddSpellCooldown(SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, 0, 1 * IN_MILLISECONDS);
        }

        void HandleDispel(DispelInfo* dispelInfo)
        {
            if (Unit* target = GetUnitOwner())
            {
                if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                {
                    // final heal
                    int32 healAmount = aurEff->GetAmount();
                    if (Unit* caster = GetCaster())
                    {
                        healAmount = caster->SpellHealingBonusDone(target, GetSpellInfo(), healAmount, HEAL, dispelInfo->GetRemovedCharges());
                        healAmount = target->SpellHealingBonusTaken(caster, GetSpellInfo(), healAmount, HEAL, dispelInfo->GetRemovedCharges());
                        target->CastCustomSpell(target, SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, NULL, NULL, true, NULL, NULL, GetCasterGUID());

                        return;
                    }

                    target->CastCustomSpell(target, SPELL_DRUID_LIFEBLOOM_FINAL_HEAL, &healAmount, NULL, NULL, true, NULL, NULL, GetCasterGUID());
                }
            }
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_dru_lifebloom_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterDispel += AuraDispelFn(spell_dru_lifebloom_AuraScript::HandleDispel);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_lifebloom_AuraScript();
    }
};

// -48496 - Living Seed
class spell_dru_living_seed : public SpellScriptLoader
{
public:
    spell_dru_living_seed() : SpellScriptLoader("spell_dru_living_seed") { }

    class spell_dru_living_seed_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_living_seed_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_LIVING_SEED_PROC))
                return false;
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            int32 amount = CalculatePct(eventInfo.GetHealInfo()->GetHeal(), aurEff->GetAmount());
            GetTarget()->CastCustomSpell(SPELL_DRUID_LIVING_SEED_PROC, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget(), true, NULL, aurEff);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_living_seed_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_living_seed_AuraScript();
    }
};

// 48504 - Living Seed (Proc)
class spell_dru_living_seed_proc : public SpellScriptLoader
{
public:
    spell_dru_living_seed_proc() : SpellScriptLoader("spell_dru_living_seed_proc") { }

    class spell_dru_living_seed_proc_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_living_seed_proc_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_LIVING_SEED_HEAL))
                return false;
            return true;
        }

        void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
        {
            PreventDefaultAction();
            GetTarget()->CastCustomSpell(SPELL_DRUID_LIVING_SEED_HEAL, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, NULL, aurEff);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_living_seed_proc_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_living_seed_proc_AuraScript();
    }
};

// -16972 - Predatory Strikes
class spell_dru_predatory_strikes : public SpellScriptLoader
{
public:
    spell_dru_predatory_strikes() : SpellScriptLoader("spell_dru_predatory_strikes") { }

    class spell_dru_predatory_strikes_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_predatory_strikes_AuraScript);

        void UpdateAmount(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Player* target = GetTarget()->ToPlayer())
                target->UpdateAttackPowerAndDamage();
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dru_predatory_strikes_AuraScript::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dru_predatory_strikes_AuraScript::UpdateAmount, EFFECT_ALL, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_predatory_strikes_AuraScript();
    }
};

// Called by Healing touch - 5185, Entangling roots - 339, Hibernate - 2637, Rebirth - 20484
// Predatory swiftness - 16974
class spell_dru_predatory_swiftness : public SpellScriptLoader
{
public:
    spell_dru_predatory_swiftness() : SpellScriptLoader("spell_dru_predatory_swiftness") { }

    class spell_dru_predatory_swiftness_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_predatory_swiftness_SpellScript);

        void HandleOnHit()
        {   
            if (Unit* caster = GetCaster())
            {
                if (caster->HasAura(SPELL_DRUID_PREDATORY_SWIFTNESS_TRIGGER))
                {
                    caster->RemoveAurasDueToSpell(SPELL_DRUID_PREDATORY_SWIFTNESS_TRIGGER);
                }
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_dru_predatory_swiftness_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_predatory_swiftness_SpellScript();
    }
};

// 62606 - Savage Defense
class spell_dru_savage_defense : public SpellScriptLoader
{
public:
    spell_dru_savage_defense() : SpellScriptLoader("spell_dru_savage_defense") { }

    class spell_dru_savage_defense_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_savage_defense_AuraScript);

        uint32 absorbPct;

        bool Load() override
        {
            absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
            return true;
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            // Set absorbtion amount to unlimited
            amount = -1;
        }

        void Absorb(AuraEffect* aurEff, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
        {
            absorbAmount = uint32(CalculatePct(GetTarget()->GetTotalAttackPowerValue(WeaponAttackType::BASE_ATTACK), absorbPct));
            aurEff->SetAmount(0);
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_savage_defense_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_dru_savage_defense_AuraScript::Absorb, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_savage_defense_AuraScript();
    }
};

// 52610 - Savage Roar
class spell_dru_savage_roar : public SpellScriptLoader
{
public:
    spell_dru_savage_roar() : SpellScriptLoader("spell_dru_savage_roar") { }

    class spell_dru_savage_roar_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_savage_roar_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (caster->GetShapeshiftForm() != FORM_CAT)
                return SPELL_FAILED_ONLY_SHAPESHIFT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_dru_savage_roar_SpellScript::CheckCast);
        }
    };

    class spell_dru_savage_roar_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_savage_roar_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_SAVAGE_ROAR))
                return false;
            return true;
        }

        void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->CastSpell(target, SPELL_DRUID_SAVAGE_ROAR, true, NULL, aurEff, GetCasterGUID());
        }

        void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SAVAGE_ROAR);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dru_savage_roar_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dru_savage_roar_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_savage_roar_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_savage_roar_AuraScript();
    }
};

// 50286 - Starfall (Dummy)
class spell_dru_starfall_dummy : public SpellScriptLoader
{
public:
    spell_dru_starfall_dummy() : SpellScriptLoader("spell_dru_starfall_dummy") { }

    class spell_dru_starfall_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_starfall_dummy_SpellScript);

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            Skyfire::Containers::RandomResizeList(targets, 2);
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            // Shapeshifting into an animal form or mounting cancels the effect
            if (caster->GetCreatureType() == CREATURE_TYPE_BEAST || caster->IsMounted())
            {
                if (SpellInfo const* spellInfo = GetTriggeringSpell())
                    caster->RemoveAurasDueToSpell(spellInfo->Id);
                return;
            }

            // Any effect which causes you to lose control of your character will supress the starfall effect.
            if (caster->HasUnitState(UNIT_STATE_CONTROLLED))
                return;

            caster->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_starfall_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            OnEffectHitTarget += SpellEffectFn(spell_dru_starfall_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_starfall_dummy_SpellScript();
    }
};

// -78892 - Stampede
class spell_dru_stampede : public SpellScriptLoader
{
public:
    spell_dru_stampede() : SpellScriptLoader("spell_dru_stampede") { }

    class spell_dru_stampede_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_stampede_AuraScript);

        bool Validate(SpellInfo const* /*spellInfo*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_STAMPEDE_BAER_RANK_1) ||
                !sSpellMgr->GetSpellInfo(SPELL_DRUID_STAMPEDE_CAT_RANK_1) ||
                !sSpellMgr->GetSpellInfo(SPELL_DRUID_STAMPEDE_CAT_STATE) ||
                !sSpellMgr->GetSpellInfo(SPELL_DRUID_FERAL_CHARGE_CAT) ||
                !sSpellMgr->GetSpellInfo(SPELL_DRUID_FERAL_CHARGE_BEAR))
                return false;
            return true;
        }

        void HandleEffectCatProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            if (GetTarget()->GetShapeshiftForm() != FORM_CAT || eventInfo.GetDamageInfo()->GetSpellInfo()->Id != SPELL_DRUID_FERAL_CHARGE_CAT)
                return;

            GetTarget()->CastSpell(GetTarget(), sSpellMgr->GetSpellWithRank(SPELL_DRUID_STAMPEDE_CAT_RANK_1, GetSpellInfo()->GetRank()), true, NULL, aurEff);
            GetTarget()->CastSpell(GetTarget(), SPELL_DRUID_STAMPEDE_CAT_STATE, true, NULL, aurEff);
        }

        void HandleEffectBearProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
        {
            PreventDefaultAction();
            if (GetTarget()->GetShapeshiftForm() != FORM_BEAR || eventInfo.GetDamageInfo()->GetSpellInfo()->Id != SPELL_DRUID_FERAL_CHARGE_BEAR)
                return;

            GetTarget()->CastSpell(GetTarget(), sSpellMgr->GetSpellWithRank(SPELL_DRUID_STAMPEDE_BAER_RANK_1, GetSpellInfo()->GetRank()), true, NULL, aurEff);
        }

        void Register() override
        {
            OnEffectProc += AuraEffectProcFn(spell_dru_stampede_AuraScript::HandleEffectCatProc, EFFECT_0, SPELL_AURA_DUMMY);
            OnEffectProc += AuraEffectProcFn(spell_dru_stampede_AuraScript::HandleEffectBearProc, EFFECT_1, SPELL_AURA_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_stampede_AuraScript();
    }
};

// 61336 - Survival Instincts
class spell_dru_survival_instincts : public SpellScriptLoader
{
public:
    spell_dru_survival_instincts() : SpellScriptLoader("spell_dru_survival_instincts") { }

    class spell_dru_survival_instincts_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_survival_instincts_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            if (!caster->IsInFeralForm())
                return SPELL_FAILED_ONLY_SHAPESHIFT;

            return SPELL_CAST_OK;
        }

        void Register() override
        {
            OnCheckCast += SpellCheckCastFn(spell_dru_survival_instincts_SpellScript::CheckCast);
        }
    };

    class spell_dru_survival_instincts_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_survival_instincts_AuraScript);

        bool Validate(SpellInfo const* /*spell*/) override
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_DRUID_SURVIVAL_INSTINCTS))
                return false;
            return true;
        }

        void AfterApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            int32 bp0 = target->CountPctFromMaxHealth(aurEff->GetAmount());
            target->CastCustomSpell(target, SPELL_DRUID_SURVIVAL_INSTINCTS, &bp0, NULL, NULL, true);
        }

        void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(SPELL_DRUID_SURVIVAL_INSTINCTS);
        }

        void Register() override
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dru_survival_instincts_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dru_survival_instincts_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_survival_instincts_SpellScript();
    }

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_survival_instincts_AuraScript();
    }
};

// 40121 - Swift Flight Form (Passive)
class spell_dru_swift_flight_passive : public SpellScriptLoader
{
public:
    spell_dru_swift_flight_passive() : SpellScriptLoader("spell_dru_swift_flight_passive") { }

    class spell_dru_swift_flight_passive_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dru_swift_flight_passive_AuraScript);

        bool Load() override
        {
            return GetCaster()->GetTypeId() == TypeID::TYPEID_PLAYER;
        }

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (Player* caster = GetCaster()->ToPlayer())
                if (caster->GetSkillValue(SKILL_RIDING) >= 375)
                    amount = 310;
        }

        void Register() override
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dru_swift_flight_passive_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED);
        }
    };

    AuraScript* GetAuraScript() const override
    {
        return new spell_dru_swift_flight_passive_AuraScript();
    }
};

// 70691 - Item T10 Restoration 4P Bonus
class spell_dru_t10_restoration_4p_bonus : public SpellScriptLoader
{
public:
    spell_dru_t10_restoration_4p_bonus() : SpellScriptLoader("spell_dru_t10_restoration_4p_bonus") { }

    class spell_dru_t10_restoration_4p_bonus_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dru_t10_restoration_4p_bonus_SpellScript);

        bool Load() override
        {
            return GetCaster()->GetTypeId() == TypeID::TYPEID_PLAYER;
        }

        void FilterTargets(std::list<WorldObject*>& targets)
        {
            if (!GetCaster()->ToPlayer()->GetGroup())
            {
                targets.clear();
                targets.push_back(GetCaster());
            }
            else
            {
                targets.remove(GetExplTargetUnit());
                std::list<Unit*> tempTargets;
                for (std::list<WorldObject*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    if ((*itr)->GetTypeId() == TypeID::TYPEID_PLAYER && GetCaster()->IsInRaidWith((*itr)->ToUnit()))
                        tempTargets.push_back((*itr)->ToUnit());

                if (tempTargets.empty())
                {
                    targets.clear();
                    FinishCast(SPELL_FAILED_DONT_REPORT);
                    return;
                }

                Unit* target = Skyfire::Containers::SelectRandomContainerElement(tempTargets);
                targets.clear();
                targets.push_back(target);
            }
        }

        void Register() override
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_dru_t10_restoration_4p_bonus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new spell_dru_t10_restoration_4p_bonus_SpellScript();
    }
};

void AddSC_druid_spell_scripts()
{
    new spell_dru_predatory_swiftness();
    new spell_dru_lifebloom_refresh();
    new spell_dru_cat_form();
    new spell_dru_dash();
    new spell_dru_eclipse("spell_dru_eclipse_lunar");
    new spell_dru_eclipse("spell_dru_eclipse_solar");
    new spell_dru_eclipse_energize();
    new spell_dru_glyph_of_innervate();
    new spell_dru_glyph_of_starfire();
    new spell_dru_glyph_of_starfire_proc();
    new spell_dru_innervate();
    new spell_dru_lifebloom();
    new spell_dru_living_seed();
    new spell_dru_living_seed_proc();
    new spell_dru_predatory_strikes();
    new spell_dru_savage_defense();
    new spell_dru_savage_roar();
    new spell_dru_starfall_dummy();
    new spell_dru_stampede();
    new spell_dru_survival_instincts();
    new spell_dru_swift_flight_passive();
    new spell_dru_t10_restoration_4p_bonus();
}
