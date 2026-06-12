#include "Stage/WDStageData.h"
#include "Monsters/WDMonsterData.h"

UWDStageData* UWDStageData::MakeZone1Stage(UObject* Outer, int32 StageNumber)
{
	StageNumber = FMath::Clamp(StageNumber, 1, 5);

	UWDStageData* Stage = NewObject<UWDStageData>(Outer);
	Stage->StageNumber = StageNumber;
	// Monster bases scale per stage (MonsterScaling.md spirit — debug curve).
	Stage->BaseHealth = 100.f * (1.f + 0.35f * (StageNumber - 1));
	Stage->BaseWallDamage = 10.f * (1.f + 0.2f * (StageNumber - 1));

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

	// GDD §2.1/§6.3: 2 types at stage 1, +1 new type per stage, the whole cast + boss at stage 5.
	switch (StageNumber)
	{
	case 1: // Ombrillon + Vif-Esprit — learn to aim and to switch.
		AddWave({ {Ombrillon, 3} }, 12.f);
		AddWave({ {Ombrillon, 3}, {VifEsprit, 2} }, 12.f);
		AddWave({ {Ombrillon, 4}, {VifEsprit, 2} }, 14.f);
		break;
	case 2: // + Rempart — the first big one, defense matters.
		AddWave({ {Ombrillon, 3}, {VifEsprit, 1} }, 12.f);
		AddWave({ {Rempart, 1}, {Ombrillon, 3} }, 14.f);
		AddWave({ {VifEsprit, 3}, {Rempart, 1}, {Ombrillon, 2} }, 16.f);
		break;
	case 3: // + Crache-Brume — the wall takes ranged hits now.
		AddWave({ {Ombrillon, 3}, {VifEsprit, 2} }, 12.f);
		AddWave({ {CracheBrume, 1}, {Rempart, 1}, {Ombrillon, 2} }, 14.f);
		AddWave({ {CracheBrume, 2}, {VifEsprit, 2}, {Rempart, 1} }, 16.f);
		AddWave({ {CracheBrume, 2}, {Rempart, 2}, {Ombrillon, 3} }, 18.f);
		break;
	case 4: // + Vestale — kill priority becomes a decision.
		AddWave({ {Ombrillon, 3}, {VifEsprit, 2} }, 12.f);
		AddWave({ {Rempart, 1}, {Vestale, 1}, {Ombrillon, 3} }, 14.f);
		AddWave({ {CracheBrume, 2}, {Vestale, 1}, {Rempart, 1}, {VifEsprit, 2} }, 16.f);
		AddWave({ {CracheBrume, 2}, {Rempart, 2}, {Vestale, 1}, {Ombrillon, 3} }, 18.f);
		break;
	default: // Stage 5 — the whole cast, the Brute closes the zone (new weapon on victory, step content).
		AddWave({ {Ombrillon, 3} }, 12.f);
		AddWave({ {Ombrillon, 3}, {VifEsprit, 2} }, 12.f);
		AddWave({ {Ombrillon, 3}, {Rempart, 1}, {Vestale, 1} }, 14.f);
		AddWave({ {VifEsprit, 2}, {CracheBrume, 2}, {Rempart, 1}, {Vestale, 1} }, 16.f);
		AddWave({ {Brute, 1}, {Ombrillon, 2}, {Vestale, 1} }, 20.f);
		break;
	}

	return Stage;
}
