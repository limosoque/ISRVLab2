// ISRV 2026. ITMO. Grandilevskii Aleksei

#include "ISRVWeapon.h"

#include <Animation/AnimInstance.h>
#include <Animation/AnimMontage.h>
#include <Components/SceneComponent.h>
#include <Components/SkeletalMeshComponent.h>

#include "Character/ISRVCharacter.h"
#include "Core/Weapon/Components/ISRVWeaponBarellComponent.h"

AISRVWeapon::AISRVWeapon()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RoolWeapon"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WMesh"));
	WeaponBarrel = CreateDefaultSubobject<UISRVWeaponBarellComponent>(TEXT("WBarrel"));

	WeaponMesh->SetupAttachment(RootComponent);
	WeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);
}

FHitResult AISRVWeapon::MakeShot()
{
	FHitResult EmptyHitResult;

	AISRVCharacter* Character = Cast<AISRVCharacter>(GetOwner());
	if (!IsValid(Character))
	{
		return EmptyHitResult;
	}

	auto* PlayerController = Character->GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return EmptyHitResult;
	}
	if (!PlayerController->IsLocalController())
	{
		return EmptyHitResult;
	}
	if (!IsValid(WeaponBarrel))
	{
		return EmptyHitResult;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotator;
	PlayerController->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotator);

	PlayCharacterMontage(FireMontage.Get());

	return WeaponBarrel->Shot(PlayerViewPoint, PlayerViewRotator.RotateVector(FVector::ForwardVector), 0.f);
}

float AISRVWeapon::PlayReloadAnimation()
{
	return PlayCharacterMontage(ReloadMontage.Get());
}

float AISRVWeapon::PlayCharacterMontage(UAnimMontage* MontageToPlay) const
{
	if (!IsValid(MontageToPlay))
	{
		return 0.f;
	}

	const AISRVCharacter* Character = Cast<AISRVCharacter>(GetOwner());
	if (!IsValid(Character) || !IsValid(Character->GetMesh()))
	{
		return 0.f;
	}

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance))
	{
		return 0.f;
	}

	return AnimInstance->Montage_Play(MontageToPlay);
}
