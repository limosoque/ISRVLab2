// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/Actor.h>
#include <Net/UnrealNetwork.h>

#include "ISRVReplicatedActor.generated.h"

UCLASS()
class ISRV_API AISRVReplicatedActor : public AActor
{
	GENERATED_BODY()

	AISRVReplicatedActor();

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RepCounter;

public:

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerIncrementCounter();

	UFUNCTION(NetMulticast, Reliable)
	void MuticastInformEveryOne();

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
