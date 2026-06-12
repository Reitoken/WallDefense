#include "Monsters/WDMonsterData.h"

namespace
{
	UWDMonsterData* MakeMonster(UObject* Outer, const TCHAR* Name, EWDMonsterRole Role, EWDMovePattern Pattern)
	{
		UWDMonsterData* Monster = NewObject<UWDMonsterData>(Outer);
		Monster->DisplayName = FText::FromString(Name);
		Monster->Role = Role;
		Monster->Pattern = Pattern;
		return Monster;
	}
}

TArray<UWDMonsterData*> UWDMonsterData::MakeZone1Bestiary(UObject* Outer)
{
	TArray<UWDMonsterData*> Bestiary;

	// M101 « Ombrillon » — the reference monster. Weak to Fire.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Ombrillon"), EWDMonsterRole::Standard, EWDMovePattern::Straight);
		M->Speed = 200.f; M->HealthMultiplier = 1.f; M->WallDamageMultiplier = 1.f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Fire;
		Bestiary.Add(M);
	}
	// M102 « Vif-Esprit » — fast, zigzags, forces re-aiming. Weak to Fire.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Vif-Esprit"), EWDMonsterRole::Fast, EWDMovePattern::Zigzag);
		M->Speed = 350.f; M->HealthMultiplier = 0.5f; M->WallDamageMultiplier = 0.7f; M->BodyScale = 0.8f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Fire;
		Bestiary.Add(M);
	}
	// M103 « Rempart » — the first big one. Weak to Ice.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Rempart"), EWDMonsterRole::Tank, EWDMovePattern::Straight);
		M->Speed = 100.f; M->HealthMultiplier = 4.f; M->Defense = 50.f; M->WallDamageMultiplier = 1.5f; M->BodyScale = 1.6f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Ice;
		Bestiary.Add(M);
	}
	// M104 « Crache-Brume » — shoots the wall from afar. Weak to Fire.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Crache-Brume"), EWDMonsterRole::Shooter, EWDMovePattern::ChargePause);
		M->Speed = 200.f; M->HealthMultiplier = 0.8f; M->WallDamageMultiplier = 1.f;
		M->bRangedAttack = true; M->AttackRange = 900.f; M->AttackInterval = 2.5f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Fire;
		Bestiary.Add(M);
	}
	// M105 « Vestale » — heals allies in pulses; kill it first. Weak to Ice.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Vestale"), EWDMonsterRole::Healer, EWDMovePattern::Sinusoidal);
		M->Speed = 200.f; M->HealthMultiplier = 0.7f; M->WallDamageMultiplier = 0.f;
		M->HealPulseAmount = 15.f; M->HealPulseInterval = 2.5f; M->HealPulseRadius = 500.f; M->BodyScale = 0.9f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Ice;
		Bestiary.Add(M);
	}
	// B1 « Brute dimensionnelle » — zone 1 boss: big, slow, hits hard. Weak to Fire.
	{
		UWDMonsterData* M = MakeMonster(Outer, TEXT("Brute dimensionnelle"), EWDMonsterRole::Boss, EWDMovePattern::Straight);
		M->Speed = 110.f; M->HealthMultiplier = 15.f; M->Defense = 50.f; M->WallDamageMultiplier = 3.f; M->BodyScale = 2.6f;
		M->AttackInterval = 2.f;
		M->ElementalProfile.bHasWeakness = true; M->ElementalProfile.Weakness = EWDElement::Fire;
		Bestiary.Add(M);
	}

	return Bestiary;
}
