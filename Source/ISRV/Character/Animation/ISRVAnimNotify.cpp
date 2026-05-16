// ISRV 2026. ITMO. Grandilevskii Aleksei


#include "ISRVAnimNotify.h"

#include <Components/SkeletalMeshComponent.h>

#include "Character/ISRVCharacter.h"

void UISRVAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AISRVCharacter* CharacterPtr = Cast<AISRVCharacter>(MeshComp->GetOwner()); IsValid(CharacterPtr)) {
		MeshComp->SetCollisionProfileName(RagdollCollisionProfileName);
		MeshComp->SetSimulatePhysics(true);
	}

}
