// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/Actor.h>

#include "ISRV.h"

#include "ISRVWeapon.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FISRVWeaponAmmoConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxAmmoInMagazine = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0", UIMin = "0"))
	int32 InitialAmmoInMagazine = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0", UIMin = "0"))
	int32 InitialAmmoReserve = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "1", UIMin = "1"))
	int32 AmmoPerShot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ReloadDuration = 1.5f;
};

UCLASS(BlueprintType)
class ISRV_API AISRVWeapon : public AActor
{
	GENERATED_BODY()

public:
	AISRVWeapon();

#pragma region WEAPON_PROPERTIES
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
	TWeakObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Animation")
	TWeakObjectPtr<UAnimMontage> FireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UISRVWeaponBarellComponent* WeaponBarrel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	FISRVWeaponAmmoConfig AmmoConfig;

private:
	bool bIsReloading = false;
	bool bIsFiring = false;

	FTimerHandle ReloadTimer;
#pragma endregion

#pragma region WEAPON_ACTIONS
public:
	FHitResult MakeShot();
	float PlayReloadAnimation();

	FORCEINLINE const FISRVWeaponAmmoConfig& GetAmmoConfig() const { return AmmoConfig; }

private:
	float PlayCharacterMontage(UAnimMontage* MontageToPlay) const;
#pragma endregion

#pragma region BASE_EQUIPABLE_INTERFACE
public:
	FORCEINLINE EWeaponType GetEquipableItemType() const { return WeaponType; };
	FORCEINLINE FName GetUnequippedSocketName() const { return UnequippedSocketName; };
	FORCEINLINE FName GetEquippedSocketName() const { return EquippedSocketName; };

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	EWeaponType WeaponType = EWeaponType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	FName UnequippedSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equip")
	FName EquippedSocketName;
#pragma endregion

};
