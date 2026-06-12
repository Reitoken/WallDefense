#include "Wall/WDWallData.h"
#include "Engine/StaticMesh.h"

FWDWallLevelDef UWDWallData::GetLevelDef(int32 Level) const
{
	if (Levels.IsEmpty())
	{
		return FWDWallLevelDef();
	}
	return Levels[FMath::Clamp(Level - 1, 0, Levels.Num() - 1)];
}

TArray<FWDWallSkillDef> UWDWallData::GetUnlockedSkills(int32 Level) const
{
	TArray<FWDWallSkillDef> Unlocked;
	for (const FWDWallSkillDef& Skill : Skills)
	{
		if (Skill.UnlockLevel <= Level)
		{
			Unlocked.Add(Skill);
		}
	}
	return Unlocked;
}

UWDWallData* UWDWallData::MakeDebugWall(UObject* Outer)
{
	UWDWallData* Wall = NewObject<UWDWallData>(Outer);

	Wall->Levels = {
		{ 1500.f, 0.f },
		{ 2200.f, 10.f },
		{ 3000.f, 20.f },
		{ 4000.f, 30.f },
	};

	FWDWallSkillDef Shield;
	Shield.Type = EWDWallSkill::StartingShield;
	Shield.UnlockLevel = 2;
	Shield.Value = 0.15f; // +15% of max HP as a starting shield layer
	Wall->Skills.Add(Shield);

	FWDWallSkillDef Repulsion;
	Repulsion.Type = EWDWallSkill::RepulsionWave;
	Repulsion.UnlockLevel = 3;
	Repulsion.Value = 700.f;            // push-back distance
	Repulsion.TriggerThreshold = 300.f; // fires on the first melee contact (shooters stay out of range)
	Repulsion.Radius = 900.f;
	Wall->Skills.Add(Repulsion);

	FWDWallSkillDef Repair;
	Repair.Type = EWDWallSkill::AutoRepair;
	Repair.UnlockLevel = 4;
	Repair.Value = 0.25f;            // heals +25% of max HP...
	Repair.TriggerThreshold = 0.25f; // ...once, the first time HP drops under 25%
	Wall->Skills.Add(Repair);

	// Same engine cube as the debug body — exercises the soft-ref preload path for real.
	Wall->WallMesh = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Engine/BasicShapes/Cube.Cube")));

	return Wall;
}
