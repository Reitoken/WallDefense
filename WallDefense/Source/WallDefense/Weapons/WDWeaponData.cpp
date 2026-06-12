#include "Weapons/WDWeaponData.h"

FWDShotParams UWDWeaponData::MakeShotParams(AActor* InInstigator) const
{
	FWDShotParams Params;
	Params.Element = Element;
	Params.Damage = Damage;
	Params.Speed = ProjectileSpeed;
	Params.Range = Range;
	Params.Radius = ProjectileRadius;
	Params.Pierce = Pierce;
	Params.Bounce = Bounce;
	Params.FragmentCount = FragmentCount;
	Params.ChainCount = ChainCount;
	Params.PushBackDistance = ProjectilePushBack;
	Params.Instigator = InInstigator;
	return Params;
}

namespace
{
	UWDWeaponData* MakeWeapon(UObject* Outer, const TCHAR* Name, EWDElement Element, EWDFireMode Mode)
	{
		UWDWeaponData* Weapon = NewObject<UWDWeaponData>(Outer);
		Weapon->DisplayName = FText::FromString(Name);
		Weapon->Element = Element;
		Weapon->FireMode = Mode;
		return Weapon;
	}
}

TArray<UWDWeaponData*> UWDWeaponData::MakeDebugArsenal(UObject* Outer)
{
	TArray<UWDWeaponData*> Arsenal;

	// Normal — Fusil automatique : mono-cible, cadence soutenue.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Fusil automatique"), EWDElement::Normal, EWDFireMode::Projectile);
		W->Damage = 10.f; W->FireInterval = 0.15f; W->ProjectileSpeed = 3000.f; W->Range = 1600.f;
		Arsenal.Add(W);
	}
	// Feu — Lance-flammes : cône court continu.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Lance-flammes"), EWDElement::Fire, EWDFireMode::ContinuousCone);
		W->Damage = 4.f; W->FireInterval = 0.1f; W->Range = 450.f; W->ConeHalfAngleDeg = 25.f;
		Arsenal.Add(W);
	}
	// Glace — Canon givrant : projectile lent et lourd.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Canon givrant"), EWDElement::Ice, EWDFireMode::Projectile);
		W->Damage = 18.f; W->FireInterval = 0.7f; W->ProjectileSpeed = 1600.f; W->ProjectileRadius = 30.f; W->Range = 1400.f;
		Arsenal.Add(W);
	}
	// Foudre — Arc électrique : tir précis qui chaîne sur 1 voisin.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Arc électrique"), EWDElement::Lightning, EWDFireMode::Projectile);
		W->Damage = 12.f; W->FireInterval = 0.4f; W->ProjectileSpeed = 3500.f; W->ChainCount = 1; W->Range = 1700.f;
		Arsenal.Add(W);
	}
	// Vent — Canon à tornade : lent, large, transperce tout et REPOUSSE les ennemis légers.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Canon à tornade"), EWDElement::Wind, EWDFireMode::Projectile);
		W->Damage = 15.f; W->FireInterval = 0.9f; W->ProjectileSpeed = 700.f; W->ProjectileRadius = 100.f;
		W->Pierce = 999; W->Range = 1200.f; W->ProjectilePushBack = 160.f;
		Arsenal.Add(W);
	}
	// Lumière — Frappe céleste : colonne sur la zone visée, après un court délai.
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Frappe céleste"), EWDElement::Light, EWDFireMode::TargetedStrike);
		W->Damage = 30.f; W->FireInterval = 1.2f; W->StrikeDelay = 0.6f; W->StrikeRadius = 200.f; W->StrikeMaxDistance = 900.f;
		Arsenal.Add(W);
	}
	// Ténèbres — Laser sombre : transperce la ligne, rebondit, gros cooldown (on switche pendant).
	{
		UWDWeaponData* W = MakeWeapon(Outer, TEXT("Laser sombre"), EWDElement::Dark, EWDFireMode::BouncingRay);
		W->Damage = 14.f; W->FireInterval = 2.f; W->Bounce = 1; W->Range = 2400.f; W->RayThickness = 40.f;
		Arsenal.Add(W);
	}

	return Arsenal;
}
