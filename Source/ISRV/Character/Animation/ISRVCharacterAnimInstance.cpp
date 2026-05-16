// ISRV 2026. ITMO. Grandilevskii Aleksei

#include "ISRVCharacterAnimInstance.h"

#include <GameFramework/CharacterMovementComponent.h>

#include "Character/ISRVCharacter.h"
#include "Character/Components/ISRVMovementComponent.h"

void UISRVCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	CachedCharacterPtr = Cast<AISRVCharacter>(TryGetPawnOwner());
	checkf(CachedCharacterPtr.IsValid(), TEXT("Use ploho"));
	CachedMovementComponent = Cast<UISRVMovementComponent>(CachedCharacterPtr->GetCharacterMovement());

}

void UISRVCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);
}
