// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <Components/SceneComponent.h>
#include <Materials/MaterialInterface.h>

#include "ISRVWeaponBarellComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class ISRV_API UISRVWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	FHitResult Shot(
	const FVector& ShotStart,
	const FVector& ShotDirection,
	float SpreadAngle,
	float DamagePercent = 1.f);

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(ClampMin=0.f, UIMin=0.f))
	float Damage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
	float FiringRange = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes")
	TSubclassOf<class UDamageType> DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Effects")
	TObjectPtr<UMaterialInterface> BulletHitDecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Effects")
	FVector BulletHitDecalSize = FVector(8.f, 8.f, 8.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual Effects", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BulletHitDecalLifeTime = 10.f;

private:
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	void ProcessHit(const FHitResult& HitResult, const FVector& Direction, float DamagePercent);
	void SpawnImpactDecal(const FHitResult& HitResult) const;

	APawn* GetOwningCharacter() const;
	AController* GetOwningController() const;
};
