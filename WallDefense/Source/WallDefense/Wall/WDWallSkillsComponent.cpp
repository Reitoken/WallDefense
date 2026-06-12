#include "Wall/WDWallSkillsComponent.h"

void UWDWallSkillsComponent::Configure(const TArray<FWDWallSkillDef>& UnlockedSkills)
{
	Armed = UnlockedSkills;
	Consumed.Init(false, Armed.Num());
}

void UWDWallSkillsComponent::NotifyStageStarted()
{
	for (int32 i = 0; i < Armed.Num(); ++i)
	{
		if (!Consumed[i] && Armed[i].Type == EWDWallSkill::StartingShield)
		{
			Trigger(i);
		}
	}
}

void UWDWallSkillsComponent::NotifyWallDamaged(float HealthPercent, float AttackerDistance)
{
	for (int32 i = 0; i < Armed.Num(); ++i)
	{
		if (Consumed[i])
		{
			continue;
		}
		switch (Armed[i].Type)
		{
		case EWDWallSkill::AutoRepair:
			if (HealthPercent < Armed[i].TriggerThreshold)
			{
				Trigger(i);
			}
			break;
		case EWDWallSkill::RepulsionWave:
			if (AttackerDistance <= Armed[i].TriggerThreshold)
			{
				Trigger(i);
			}
			break;
		default:
			break;
		}
	}
}

bool UWDWallSkillsComponent::IsArmed(EWDWallSkill Type) const
{
	for (int32 i = 0; i < Armed.Num(); ++i)
	{
		if (Armed[i].Type == Type && !Consumed[i])
		{
			return true;
		}
	}
	return false;
}

bool UWDWallSkillsComponent::HasTriggered(EWDWallSkill Type) const
{
	for (int32 i = 0; i < Armed.Num(); ++i)
	{
		if (Armed[i].Type == Type && Consumed[i])
		{
			return true;
		}
	}
	return false;
}

int32 UWDWallSkillsComponent::GetArmedCount() const
{
	int32 Count = 0;
	for (int32 i = 0; i < Armed.Num(); ++i)
	{
		Count += Consumed[i] ? 0 : 1;
	}
	return Count;
}

void UWDWallSkillsComponent::Trigger(int32 Index)
{
	Consumed[Index] = true;
	OnSkillTriggered.Broadcast(Armed[Index]);
}
