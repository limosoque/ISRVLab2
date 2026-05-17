// ISRV 2026. ITMO. Grandilevskii Aleksei

#pragma once

#include "CoreMinimal.h"

#include <GameFramework/Character.h>
#include <Logging/LogMacros.h>

#include "ISRVCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class UCameraComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(abstract)
class AISRVCharacter : public ACharacter
{
	GENERATED_BODY()


	AISRVCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UISRVEquipementComponent* EquipementComponent;

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ReloadAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Change Weapon Input Actions */
	UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* NextWeaponAction;
    
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* PreviousWeaponAction;

	// Aim is temporarily disabled to diagnose weapon-switch input.
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AimAction;
    
	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim")
	float AimCameraFOV = 65.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim")
	float AimCameraArmLength = 280.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim")
	FVector AimCameraSocketOffset = FVector(0.f, 60.f, 25.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AimCameraInterpSpeed = 12.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AimRotationInterpSpeed = 20.f;

	UPROPERTY(BlueprintReadOnly, Category = "Aim")
	bool bIsAiming = false;

	
private:
	float DefaultCameraFOV = 90.f;
	float DefaultCameraArmLength = 400.f;
	FVector DefaultCameraSocketOffset = FVector::ZeroVector;
	float LastWeaponSwitchInputTime = -1.f;
	int32 LastWeaponSwitchDirection = 0;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Equip(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	void Reload(const FInputActionValue& Value);
	void ReloadFromKeyboard();

	void EquipNextWeapon(const FInputActionValue& Value);
	void EquipPreviousWeapon(const FInputActionValue& Value);

	void StartAim(const FInputActionValue& Value);
	void StopAim(const FInputActionValue& Value);
	void SetAiming(bool bNewIsAiming);
	void UpdateAim(float DeltaSeconds);
	bool CanProcessWeaponSwitchInput(int32 Direction);
	
public:
	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
