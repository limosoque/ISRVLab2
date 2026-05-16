// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogISRV, Log, All);

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None UMETA(DisplayName = "Empty"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "AssaultRifle"),
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipementSlots : uint8
{
	None,
	SideArm,
	PrimaryWeapon,
	SecondaryWeapon,
	Throwable,
	MAX UMETA(Hidden)
};

const FName FXParamTraceEnd = FName("TraceEnd");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");