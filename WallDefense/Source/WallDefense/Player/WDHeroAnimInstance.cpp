#include "Player/WDHeroAnimInstance.h"
#include "Player/WDHeroCharacter.h"

void UWDHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AWDHeroCharacter* Hero = Cast<AWDHeroCharacter>(TryGetPawnOwner()))
	{
		Speed = Hero->GetSpeed();
		bIsMoving = Hero->IsMoving();
		MoveDirection = Hero->GetMoveDirectionAngle();
	}
}
