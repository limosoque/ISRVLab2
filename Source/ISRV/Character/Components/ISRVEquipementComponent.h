// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <Components/ActorComponent.h>
#include <TimerManager.h>

#include "ISRV.h"

#include "ISRVEquipementComponent.generated.h"

class AISRVWeapon;

USTRUCT(BlueprintType)
struct FISRVWeaponAmmoState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 CurrentAmmoInMagazine = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	int32 TotalAmmo = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISRV_API UISRVEquipementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UISRVEquipementComponent();

	void EquipLastWeapon();
	void UnequipCurrentWeapon();

	void EquipNextWeapon();
	void EquipPreviousWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool TryMakeShot();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool ReloadCurrentWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	FISRVWeaponAmmoState GetCurrentWeaponAmmoState() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	int32 GetCurrentAmmoInMagazine() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	int32 GetCurrentTotalAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	bool IsCurrentWeaponReloading() const;

	FORCEINLINE AISRVWeapon* GetCurrentWeapon() const { return currentWeapon; };

protected:
	virtual void BeginPlay() override;

private:
	FORCEINLINE bool IsValidSlotIndex(uint8 SlotIndex) const
	{
		return WeaponArray.IsValidIndex(SlotIndex) && WeaponAmmoArray.IsValidIndex(SlotIndex);
	}

	FORCEINLINE uint8 NextWeaponsArraySlotIndex(uint8 CurrentSlotIndex) const
	{
		return CurrentSlotIndex != WeaponArray.Num() - 1 ? CurrentSlotIndex + 1 : 0;
	};
	FORCEINLINE uint8 PreviousWeaponsArraySlotIndex(uint8 CurrentSlotIndex) const
	{
		return CurrentSlotIndex != 0 ? CurrentSlotIndex - 1 : WeaponArray.Num() - 1;
	};

	void EquipItemInSlot(const EEquipementSlots& Slot);
	void InitializeAmmoForWeapon(EEquipementSlots Slot, const AISRVWeapon* Weapon);
	void FinishReload();
	bool RestoreAmmoForHeadshotIfNeeded(const FHitResult& HitResult, EEquipementSlots Slot, int32 AmmoAmountToRestore);
	FISRVWeaponAmmoState* GetAmmoState(EEquipementSlots Slot);
	const FISRVWeaponAmmoState* GetAmmoState(EEquipementSlots Slot) const;
	AISRVWeapon* GetWeaponInSlot(EEquipementSlots Slot) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipementSlots, TSubclassOf<class AISRVWeapon>> WeaponLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo|Bonus")
	bool bRestoreAmmoOnHeadshot = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo|Bonus")
	TArray<FName> HeadshotBoneNames;

private:
	AISRVWeapon* currentWeapon = nullptr;

	TArray<AISRVWeapon*, TInlineAllocator<(uint8)EEquipementSlots::MAX>> WeaponArray;
	TArray<FISRVWeaponAmmoState, TInlineAllocator<(uint8)EEquipementSlots::MAX>> WeaponAmmoArray;

	EEquipementSlots PreviousEquippedSlot = EEquipementSlots::None;
	EEquipementSlots CurrentEquippedSlot = EEquipementSlots::None;

	bool bIsReloading = false;
	EEquipementSlots ReloadingSlot = EEquipementSlots::None;
	FTimerHandle ReloadTimer;

	TWeakObjectPtr<class AISRVCharacter> CachedCharacterPtr;
};
