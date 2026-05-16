// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/GameModeBase.h>

#include "ISRVGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AISRVGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AISRVGameMode();
};



