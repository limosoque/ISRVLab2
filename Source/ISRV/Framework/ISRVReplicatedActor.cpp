// ISRV 2026. ITMO. Grandilevskii Aleksei


#include "Framework/ISRVReplicatedActor.h"


AISRVReplicatedActor::AISRVReplicatedActor()
{
	bReplicates = true;
	RepCounter = 0;
}

void AISRVReplicatedActor::ServerIncrementCounter_Implementation()
{
	RepCounter++;

	UE_LOG(LogTemp, Warning, TEXT("SERVER: Counter = %d"), RepCounter);

	MuticastInformEveryOne();
};

void AISRVReplicatedActor::MuticastInformEveryOne_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("NETMUTICAST: Counter = %d"), RepCounter);
}

void AISRVReplicatedActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISRVReplicatedActor, RepCounter);
}