// ISRV 2026. ITMO. Grandilevskii Aleksei

#include "ISRVEquipementComponent.h"

#include "Character/ISRVCharacter.h"
#include "Core/Weapon/ISRVWeapon.h"

UISRVEquipementComponent::UISRVEquipementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	HeadshotBoneNames = { TEXT("head"), TEXT("Head") };
}

void UISRVEquipementComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacterPtr = Cast<AISRVCharacter>(GetOwner());
	if (!CachedCharacterPtr.IsValid())
	{
		UE_LOG(LogISRV, Error, TEXT("EquipmentComponent owner is not AISRVCharacter"));
		return;
	}

	WeaponArray.AddZeroed((uint8)EEquipementSlots::MAX);
	WeaponAmmoArray.AddDefaulted((uint8)EEquipementSlots::MAX);

	for (const auto& WeaponPair : WeaponLoadout)
	{
		if (!IsValid(WeaponPair.Value))
		{
			continue;
		}

		const uint8 SlotIndex = (uint8)WeaponPair.Key;
		if (!WeaponArray.IsValidIndex(SlotIndex))
		{
			continue;
		}

		AISRVWeapon* WeaponPtr = GetWorld()->SpawnActor<AISRVWeapon>(WeaponPair.Value);
		if (!IsValid(WeaponPtr))
		{
			continue;
		}

		WeaponPtr->AttachToComponent(CachedCharacterPtr->GetMesh(),
			FAttachmentTransformRules::KeepRelativeTransform,
			WeaponPtr->GetUnequippedSocketName()
		);
		WeaponPtr->SetOwner(CachedCharacterPtr.Get());
		WeaponArray[SlotIndex] = WeaponPtr;
		InitializeAmmoForWeapon(WeaponPair.Key, WeaponPtr);
	}
}

void UISRVEquipementComponent::EquipLastWeapon()
{
	if (PreviousEquippedSlot == EEquipementSlots::None)
	{
		EquipItemInSlot(EEquipementSlots::PrimaryWeapon);
	}
	else
	{
		EquipItemInSlot(PreviousEquippedSlot);
	}
}

void UISRVEquipementComponent::EquipItemInSlot(const EEquipementSlots& Slot)
{
	const uint8 SlotIndex = static_cast<uint8>(Slot);
	if (!WeaponArray.IsValidIndex(SlotIndex))
	{
		return;
	}

	AISRVWeapon* TempPtr = WeaponArray[SlotIndex];
	if (!IsValid(TempPtr))
	{
		return;
	}

	UnequipCurrentWeapon();
	currentWeapon = TempPtr;

	currentWeapon->AttachToComponent(CachedCharacterPtr->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		currentWeapon->GetEquippedSocketName());
	CurrentEquippedSlot = Slot;

	const FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	if (AmmoState)
	{
		UE_LOG(LogISRV, Log, TEXT("Equipped weapon. Ammo: %d / %d"), AmmoState->CurrentAmmoInMagazine, AmmoState->TotalAmmo);
	}
}

void UISRVEquipementComponent::UnequipCurrentWeapon()
{
	if (CurrentEquippedSlot == EEquipementSlots::None || !IsValid(currentWeapon))
	{
		return;
	}

	currentWeapon->AttachToComponent(CachedCharacterPtr->GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		currentWeapon->GetUnequippedSocketName());
	currentWeapon = nullptr;
	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipementSlots::None;
}

void UISRVEquipementComponent::EquipNextWeapon()
{
	if (WeaponArray.Num() == 0)
	{
		return;
	}

	uint8 CurrentSlotIndex = static_cast<uint8>(CurrentEquippedSlot);
	uint8 NextSlotIndex = CurrentSlotIndex;

	for (int32 Attempts = 0; Attempts < WeaponArray.Num(); ++Attempts)
	{
		NextSlotIndex = NextWeaponsArraySlotIndex(NextSlotIndex);
		if (IsValid(WeaponArray[NextSlotIndex]))
		{
			EquipItemInSlot(static_cast<EEquipementSlots>(NextSlotIndex));
			return;
		}
	}
}

void UISRVEquipementComponent::EquipPreviousWeapon()
{
	if (WeaponArray.Num() == 0)
	{
		return;
	}

	uint8 CurrentSlotIndex = static_cast<uint8>(CurrentEquippedSlot);
	uint8 PrevSlotIndex = CurrentSlotIndex;

	for (int32 Attempts = 0; Attempts < WeaponArray.Num(); ++Attempts)
	{
		PrevSlotIndex = PreviousWeaponsArraySlotIndex(PrevSlotIndex);
		if (IsValid(WeaponArray[PrevSlotIndex]))
		{
			EquipItemInSlot(static_cast<EEquipementSlots>(PrevSlotIndex));
			return;
		}
	}
}

bool UISRVEquipementComponent::TryMakeShot()
{
	if (!IsValid(currentWeapon) || CurrentEquippedSlot == EEquipementSlots::None)
	{
		return false;
	}

	if (IsCurrentWeaponReloading())
	{
		UE_LOG(LogISRV, Log, TEXT("Cannot shoot: current weapon is reloading"));
		return false;
	}

	FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	if (!AmmoState)
	{
		return false;
	}

	const FISRVWeaponAmmoConfig& AmmoConfig = currentWeapon->GetAmmoConfig();
	const int32 AmmoPerShot = FMath::Max(AmmoConfig.AmmoPerShot, 1);

	if (AmmoState->CurrentAmmoInMagazine < AmmoPerShot)
	{
		UE_LOG(LogISRV, Warning, TEXT("Magazine is empty. Trying to reload..."));
		ReloadCurrentWeapon();
		return false;
	}

	AmmoState->CurrentAmmoInMagazine -= AmmoPerShot;
	const FHitResult HitResult = currentWeapon->MakeShot();
	RestoreAmmoForHeadshotIfNeeded(HitResult, CurrentEquippedSlot, AmmoPerShot);

	UE_LOG(LogISRV, Log, TEXT("Shot. Ammo: %d / %d"), AmmoState->CurrentAmmoInMagazine, AmmoState->TotalAmmo);
	return true;
}

bool UISRVEquipementComponent::ReloadCurrentWeapon()
{
	if (!IsValid(currentWeapon) || CurrentEquippedSlot == EEquipementSlots::None || bIsReloading)
	{
		return false;
	}

	FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	if (!AmmoState)
	{
		return false;
	}

	const FISRVWeaponAmmoConfig& AmmoConfig = currentWeapon->GetAmmoConfig();
	const int32 MagazineCapacity = FMath::Max(AmmoConfig.MaxAmmoInMagazine, 1);
	if (AmmoState->CurrentAmmoInMagazine >= MagazineCapacity || AmmoState->TotalAmmo <= 0)
	{
		UE_LOG(LogISRV, Log, TEXT("Reload skipped. Ammo: %d / %d"), AmmoState->CurrentAmmoInMagazine, AmmoState->TotalAmmo);
		return false;
	}

	bIsReloading = true;
	ReloadingSlot = CurrentEquippedSlot;

	float ReloadDuration = currentWeapon->PlayReloadAnimation();
	if (ReloadDuration <= KINDA_SMALL_NUMBER)
	{
		ReloadDuration = AmmoConfig.ReloadDuration;
	}

	if (ReloadDuration <= KINDA_SMALL_NUMBER || !GetWorld())
	{
		FinishReload();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UISRVEquipementComponent::FinishReload, ReloadDuration, false);
	}

	UE_LOG(LogISRV, Log, TEXT("Reload started"));
	return true;
}

FISRVWeaponAmmoState UISRVEquipementComponent::GetCurrentWeaponAmmoState() const
{
	const FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	return AmmoState ? *AmmoState : FISRVWeaponAmmoState();
}

int32 UISRVEquipementComponent::GetCurrentAmmoInMagazine() const
{
	const FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	return AmmoState ? AmmoState->CurrentAmmoInMagazine : 0;
}

int32 UISRVEquipementComponent::GetCurrentTotalAmmo() const
{
	const FISRVWeaponAmmoState* AmmoState = GetAmmoState(CurrentEquippedSlot);
	return AmmoState ? AmmoState->TotalAmmo : 0;
}

bool UISRVEquipementComponent::IsCurrentWeaponReloading() const
{
	return bIsReloading && ReloadingSlot == CurrentEquippedSlot;
}

void UISRVEquipementComponent::InitializeAmmoForWeapon(EEquipementSlots Slot, const AISRVWeapon* Weapon)
{
	const uint8 SlotIndex = static_cast<uint8>(Slot);
	if (!IsValid(Weapon) || !WeaponAmmoArray.IsValidIndex(SlotIndex))
	{
		return;
	}

	const FISRVWeaponAmmoConfig& AmmoConfig = Weapon->GetAmmoConfig();
	const int32 MagazineCapacity = FMath::Max(AmmoConfig.MaxAmmoInMagazine, 1);

	FISRVWeaponAmmoState& AmmoState = WeaponAmmoArray[SlotIndex];
	AmmoState.CurrentAmmoInMagazine = FMath::Clamp(AmmoConfig.InitialAmmoInMagazine, 0, MagazineCapacity);
	AmmoState.TotalAmmo = FMath::Max(AmmoConfig.InitialAmmoReserve, 0);
}

void UISRVEquipementComponent::FinishReload()
{
	FISRVWeaponAmmoState* AmmoState = GetAmmoState(ReloadingSlot);
	AISRVWeapon* Weapon = GetWeaponInSlot(ReloadingSlot);
	if (AmmoState && IsValid(Weapon))
	{
		const FISRVWeaponAmmoConfig& AmmoConfig = Weapon->GetAmmoConfig();
		const int32 MagazineCapacity = FMath::Max(AmmoConfig.MaxAmmoInMagazine, 1);
		const int32 AmmoNeeded = MagazineCapacity - AmmoState->CurrentAmmoInMagazine;
		const int32 AmmoToReload = FMath::Min(AmmoNeeded, AmmoState->TotalAmmo);

		AmmoState->CurrentAmmoInMagazine += AmmoToReload;
		AmmoState->TotalAmmo -= AmmoToReload;

		UE_LOG(LogISRV, Log, TEXT("Reload finished. Ammo: %d / %d"), AmmoState->CurrentAmmoInMagazine, AmmoState->TotalAmmo);
	}

	bIsReloading = false;
	ReloadingSlot = EEquipementSlots::None;
}

bool UISRVEquipementComponent::RestoreAmmoForHeadshotIfNeeded(const FHitResult& HitResult, EEquipementSlots Slot, int32 AmmoAmountToRestore)
{
	if (!bRestoreAmmoOnHeadshot || !HitResult.bBlockingHit || AmmoAmountToRestore <= 0)
	{
		return false;
	}

	const FString BoneName = HitResult.BoneName.ToString();
	bool bIsHeadshot = BoneName.Contains(TEXT("head"), ESearchCase::IgnoreCase);
	for (const FName& HeadshotBoneName : HeadshotBoneNames)
	{
		if (HitResult.BoneName == HeadshotBoneName)
		{
			bIsHeadshot = true;
			break;
		}
	}

	if (!bIsHeadshot)
	{
		return false;
	}

	FISRVWeaponAmmoState* AmmoState = GetAmmoState(Slot);
	AISRVWeapon* Weapon = GetWeaponInSlot(Slot);
	if (!AmmoState || !IsValid(Weapon))
	{
		return false;
	}

	const int32 MagazineCapacity = FMath::Max(Weapon->GetAmmoConfig().MaxAmmoInMagazine, 1);
	AmmoState->CurrentAmmoInMagazine = FMath::Min(AmmoState->CurrentAmmoInMagazine + AmmoAmountToRestore, MagazineCapacity);
	UE_LOG(LogISRV, Log, TEXT("Headshot! Ammo restored. Ammo: %d / %d"), AmmoState->CurrentAmmoInMagazine, AmmoState->TotalAmmo);
	return true;
}

FISRVWeaponAmmoState* UISRVEquipementComponent::GetAmmoState(EEquipementSlots Slot)
{
	const uint8 SlotIndex = static_cast<uint8>(Slot);
	return WeaponAmmoArray.IsValidIndex(SlotIndex) ? &WeaponAmmoArray[SlotIndex] : nullptr;
}

const FISRVWeaponAmmoState* UISRVEquipementComponent::GetAmmoState(EEquipementSlots Slot) const
{
	const uint8 SlotIndex = static_cast<uint8>(Slot);
	return WeaponAmmoArray.IsValidIndex(SlotIndex) ? &WeaponAmmoArray[SlotIndex] : nullptr;
}

AISRVWeapon* UISRVEquipementComponent::GetWeaponInSlot(EEquipementSlots Slot) const
{
	const uint8 SlotIndex = static_cast<uint8>(Slot);
	return WeaponArray.IsValidIndex(SlotIndex) ? WeaponArray[SlotIndex] : nullptr;
}
