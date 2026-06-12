#include "Stage/WDStageData.h"
#include "Monsters/WDMonsterData.h"

UWDStageData* UWDStageData::MakeDebugStage1(UObject* Outer)
{
	UWDStageData* Stage = NewObject<UWDStageData>(Outer);
	Stage->StageNumber = 1;
	Stage->BaseHealth = 100.f;
	Stage->BaseWallDamage = 10.f;

	const TArray<UWDMonsterData*> Bestiary = UWDMonsterData::MakeZone1Bestiary(Outer);
	UWDMonsterData* Ombrillon = Bestiary[0];
	UWDMonsterData* VifEsprit = Bestiary[1];
	UWDMonsterData* Rempart = Bestiary[2];
	UWDMonsterData* CracheBrume = Bestiary[3];
	UWDMonsterData* Vestale = Bestiary[4];
	UWDMonsterData* Brute = Bestiary[5];

	auto AddWave = [Stage](std::initializer_list<TPair<UWDMonsterData*, int32>> List, float TimeBeforeNext)
	{
		FWDWaveDef Wave;
		for (const auto& Pair : List)
		{
			FWDWaveEntry Entry;
			Entry.Monster = Pair.Key;
			Entry.Count = Pair.Value;
			Wave.Entries.Add(Entry);
		}
		Wave.TimeBeforeNextWave = TimeBeforeNext;
		Stage->Waves.Add(Wave);
	};

	// Progressive introduction (Bestiaire.md §0): each wave teaches something new.
	AddWave({ {Ombrillon, 3} }, 12.f);
	AddWave({ {Ombrillon, 3}, {VifEsprit, 2} }, 12.f);
	AddWave({ {Ombrillon, 3}, {Rempart, 1}, {Vestale, 1} }, 14.f);
	AddWave({ {VifEsprit, 2}, {CracheBrume, 2}, {Rempart, 1}, {Vestale, 1} }, 16.f);
	AddWave({ {Brute, 1}, {Ombrillon, 2}, {Vestale, 1} }, 20.f);

	return Stage;
}
