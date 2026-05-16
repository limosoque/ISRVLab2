// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <Animation/AnimNotifies/AnimNotify.h>

#include "ISRVAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class ISRV_API UISRVAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

protected: 
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	FName RagdollCollisionProfileName;
	
};
