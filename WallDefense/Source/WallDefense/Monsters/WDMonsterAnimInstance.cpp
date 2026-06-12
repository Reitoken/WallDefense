#include "Monsters/WDMonsterAnimInstance.h"
#include "Monsters/WDMonster.h"
#include "Combat/WDHealthComponent.h"

void UWDMonsterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AWDMonster* Monster = Cast<AWDMonster>(GetOwningActor()))
	{
		Speed = Monster->GetSpeed();
		bIsMoving = Monster->IsMoving();
		bIsDead = Monster->IsDead();
		bIsAttacking = Monster->IsAttacking();
		bHasShield = Monster->GetHealth() && Monster->GetHealth()->HasShield();
	}
}
