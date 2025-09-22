#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DroneCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
struct FInputActionValue;
class UFloatingPawnMovement;

UCLASS()
class MOVINGOBJECT_API ADroneCharacter : public APawn
{
	GENERATED_BODY()

public:
	ADroneCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* DroneMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UFloatingPawnMovement* MovementComp;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Gravity;

	virtual float TakeDamage(float DamageAmount,
		struct FDamageEvent const& DamageEvnet,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	FVector VerticalVelocity;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void MoveFwd(const FInputActionValue& value);
	void MoveUp(const FInputActionValue& value);
	void MoveDown(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Roll(const FInputActionValue& value);
	void StartSprint(const FInputActionValue& value);
	void StopSprint(const FInputActionValue& value);

	void OnDeath();

private:
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	FVector LastLocation;
};
