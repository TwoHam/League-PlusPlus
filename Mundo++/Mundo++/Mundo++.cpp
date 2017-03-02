#include "PluginSDK.h"

PluginSetup("[TwoHam] Dr.Mundo++");

IMenu* MainMenu;

IMenu* ComboMenu;
IMenuOption* ComboQ;
IMenuOption* ComboQHealth;
IMenuOption* ComboW;
IMenuOption* ComboWHealth;
IMenuOption* ComboE;
IMenuOption* ComboR;

IMenu* QSettings;
IMenu* WSettings;
IMenu* ESettings;
IMenu* RSettings;

IMenuOption* Hitchance;

ISpell2* Q;
ISpell2* W;
ISpell2* E;
ISpell2* R;


void LoadMenu() 
{
	MainMenu = GPluginSDK->AddMenu("Dr.Mundo++");

	ComboMenu = MainMenu->AddMenu("Combo Settings");
	{
		QSettings = ComboMenu->AddMenu("[Q] Settings");
		{
			ComboQ = QSettings->CheckBox("Use [Q] in Combo?", true);
			ComboQHealth = QSettings->AddFloat("Minimum HP% to use Q", 1, 100, 10);
		}
		WSettings = ComboMenu->AddMenu("[W] Settings");
		{
			ComboW = WSettings->CheckBox("Use [W] in Combo?", true);
			ComboWHealth = WSettings->AddFloat("Minimum HP% to use Q", 1, 100, 10);
		}
		ESettings = ComboMenu->AddMenu("[E] Settings");
		{
			ComboE = ESettings->CheckBox("Use [E] in Combo?", true);
		}
	}
}

void LoadSpells()
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, false, false, static_cast<eCollisionFlags>(kCollidesWithHeroes | kCollidesWithMinions | kCollidesWithYasuoWall));
	Q->SetSkillshot(0.275f, 60, 2000, 1000);

	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, true, kCollidesWithYasuoWall);
}

void ComoFunction()
{
	auto target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, 1000);
	
	if (target == nullptr || !target->IsHero())
		return;

	if (Q->IsReady() && ComboQ->Enabled() && GEntityList->Player()->HealthPercent() >= ComboQHealth->GetFloat())
		Q->CastOnTarget(target, kHitChanceVeryHigh);

	if (W->IsReady() && ComboW->Enabled() && GEntityList->Player()->HealthPercent() >= ComboWHealth->GetFloat() && GEntityList->Player()->HasBuff("BurningAgony"))
		W->CastOnTarget(target, kHitChanceVeryHigh);
}


PLUGIN_EVENT(void) OnGameUpdate()
{
	if (GEntityList->Player()->IsDead())
		return;
	
	switch (GOrbwalking->GetOrbwalkingMode())
	{
	case kModeCombo:
		ComoFunction();
		break;
	}
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	LoadMenu();
	LoadSpells();

	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
}

PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
}