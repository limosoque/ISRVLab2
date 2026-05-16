// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <Animation/AnimInstance.h>

#include "ISRVCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ISRV_API UISRVCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void NativeBeginPlay() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "CharacterProperties");
	float CharacterSpeed = .0f;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterPtrs");
	TWeakObjectPtr<class AISRVCharacter> CachedCharacterPtr = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterPtrs");
	TWeakObjectPtr<class UISRVMovementComponent> CachedMovementComponent = nullptr;

};
