// ISRV 2026. ITMO. Grandilevskii Aleksei

#include "ISRVWeaponBarellComponent.h"

#include <CollisionQueryParams.h>
#include <Components/DecalComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Engine/DamageEvents.h>
#include <GameFramework/DamageType.h>
#include <Kismet/GameplayStatics.h>

FHitResult UISRVWeaponBarellComponent::Shot(
	const FVector& ShotStart,
	const FVector& ShotDirection,
	float SpreadAngle,
	float DamagePercent)
{
	FVector RealDirection = ShotDirection.GetSafeNormal();
	if (SpreadAngle > 0.f)
	{
		RealDirection = (RealDirection + GetBulletSpreadOffset(SpreadAngle, RealDirection.Rotation())).GetSafeNormal();
	}

	const FVector ShotEnd = ShotStart + FiringRange * RealDirection;

	DrawDebugLine(GetWorld(), ShotStart, ShotEnd, FColor::Purple, false, 2.f, (uint8)0U, 1.f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	if (APawn* OwningCharacter = GetOwningCharacter())
	{
		QueryParams.AddIgnoredActor(OwningCharacter);
	}

	if (GetWorld()->LineTraceSingleByChannel(HitResult, ShotStart, ShotEnd, ECC_Visibility, QueryParams))
	{
		if (HitResult.BoneName.IsNone())
		{
			FCollisionQueryParams MeshQueryParams = QueryParams;
			if (UPrimitiveComponent* HitComponent = HitResult.GetComponent())
			{
				MeshQueryParams.AddIgnoredComponent(HitComponent);
			}

			FHitResult MeshHitResult;
			if (GetWorld()->LineTraceSingleByChannel(MeshHitResult, ShotStart, ShotEnd, ECC_Visibility, MeshQueryParams)
				&& MeshHitResult.GetActor() == HitResult.GetActor()
				&& Cast<USkeletalMeshComponent>(MeshHitResult.GetComponent()))
			{
				HitResult = MeshHitResult;
			}
		}

		ProcessHit(HitResult, RealDirection, DamagePercent);
		SpawnImpactDecal(HitResult);
	}

	return HitResult;
}

FVector UISRVWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
		+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;
}

void UISRVWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction, float DamagePercent)
{
	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor)) {
		return;
	}

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = Direction;
	DamageEvent.DamageTypeClass = DamageType;

	HitActor->TakeDamage(Damage * DamagePercent, DamageEvent, GetOwningController(), GetOwningCharacter());
}

void UISRVWeaponBarellComponent::SpawnImpactDecal(const FHitResult& HitResult) const
{
	if (!IsValid(BulletHitDecalMaterial) || !GetWorld())
	{
		return;
	}

	const FRotator DecalRotation = FRotationMatrix::MakeFromX(HitResult.ImpactNormal).Rotator();
	UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(),
		BulletHitDecalMaterial,
		BulletHitDecalSize,
		HitResult.ImpactPoint,
		DecalRotation,
		BulletHitDecalLifeTime
	);
}

APawn* UISRVWeaponBarellComponent::GetOwningCharacter() const
{
	AActor* OwningWeapon = GetOwner();
	if (!IsValid(OwningWeapon))
	{
		return nullptr;
	}
	return Cast<APawn>(OwningWeapon->GetOwner());
}

AController* UISRVWeaponBarellComponent::GetOwningController() const
{
	auto* owner = GetOwningCharacter();
	return IsValid(owner) ? owner->GetController() : nullptr;
}
