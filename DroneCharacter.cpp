#include "DroneCharacter.h"
#include "DroneController.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADroneCharacter::ADroneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->InitCapsuleSize(42.0f,100.0f);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CapsuleComp->SetSimulatePhysics(false);
	RootComponent = CapsuleComp;

	DroneMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DroneMesh"));
	DroneMesh->SetupAttachment(CapsuleComp);
	DroneMesh->SetRelativeLocation(FVector(0.0f,0.0f,-90.f));
	DroneMesh->SetRelativeRotation(FRotator(0.0f, -90.f, 0.0f));

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->UpdatedComponent = RootComponent;
	MovementComp->MaxSpeed = 1200.f;
	LastLocation = GetActorLocation();

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = false;
	SpringArmComp->bInheritPitch = true;
	SpringArmComp->bInheritYaw = true;
	SpringArmComp->bInheritRoll = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = true; 

	NormalSpeed = 1200.f;
	SprintSpeedMultiplier = 1.8f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADroneCharacter::Tick(float DeltaTime) 
{	
	Super::Tick(DeltaTime); 
	FVector CurrentLocation = GetActorLocation(); 
	Velocity = (CurrentLocation - LastLocation) / DeltaTime; 
	LastLocation = CurrentLocation; 

	VerticalVelocity.Z -= Gravity * DeltaTime;
	FVector GravityMove = VerticalVelocity * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(GravityMove, true, &Hit);

	if (Hit.bBlockingHit)
	{
		VerticalVelocity.Z = 0.0f;
	}
}

void ADroneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ADroneController* DroneControll = Cast<ADroneController>(GetController()))
		{
			if (DroneControll->IA_MoveFwd)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_MoveFwd,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::MoveFwd
				);
			}
			if (DroneControll->IA_MoveUp)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_MoveUp,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::MoveUp
				);
			}

			if (DroneControll->IA_MoveDown)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_MoveDown,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::MoveDown
				);
			}
			if (DroneControll->IA_Look)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_Look,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::Look
				);
			}
			if (DroneControll->IA_Sprint)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_Sprint,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::StartSprint

				);

				EnhancedInput->BindAction(
					DroneControll->IA_Sprint,
					ETriggerEvent::Completed,
					this,
					&ADroneCharacter::StopSprint
				);
			}

			if (DroneControll->IA_Roll)
			{
				EnhancedInput->BindAction(
					DroneControll->IA_Roll,
					ETriggerEvent::Triggered,
					this,
					&ADroneCharacter::Roll
				);
			}
		}
	}
}

void ADroneCharacter::MoveFwd(const FInputActionValue& value)
{

	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	FVector MoveOffset = FVector::ZeroVector;

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveOffset.X += MoveInput.X * NormalSpeed * DeltaTime; // X√‡ = Forward
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveOffset.Y += MoveInput.Y * NormalSpeed * DeltaTime; // Y√‡ = Right
	}

	AddActorLocalOffset(MoveOffset, true);
}

void ADroneCharacter::MoveUp(const FInputActionValue& value)
{
	if (!Controller) return;

	float MoveValue = value.Get<float>();

	if (!FMath::IsNearlyZero(MoveValue))
	{
		AddMovementInput(FVector::UpVector, MoveValue);
	}
}
void ADroneCharacter::MoveDown(const FInputActionValue& value)
{
	if (!Controller) return;

	float MoveValue = -value.Get<float>();

	if (!FMath::IsNearlyZero(MoveValue))
	{
		AddMovementInput(FVector::UpVector, MoveValue);
	}
}
void ADroneCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(LookInput.X))
	{
		AddActorLocalRotation(FRotator(0.0f, LookInput.X, 0.0f));
	}

	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		AddActorLocalRotation(FRotator(LookInput.Y, 0.0f, 0.0f));
	}
}
void ADroneCharacter::Roll(const FInputActionValue& value)
{
	float RollValue = value.Get<float>();

	if (!FMath::IsNearlyZero(RollValue))
	{
		FRotator RollRotation(0.0f, 0.0f, RollValue);

		AddActorLocalRotation(RollRotation);
	}
}
void ADroneCharacter::StartSprint(const FInputActionValue& value)
{
	MovementComp->MaxSpeed = SprintSpeed;
}
void ADroneCharacter::StopSprint(const FInputActionValue& value)
{
	MovementComp->MaxSpeed = NormalSpeed;
}

float ADroneCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvnet,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvnet, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health decreased to : %f"), Health);

	if (Health < 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ADroneCharacter::OnDeath()
{

}