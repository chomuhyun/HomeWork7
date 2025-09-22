#include "DroneController.h"
#include "EnhancedInputSubsystems.h"

ADroneController::ADroneController()
	:InputMappingContext(nullptr),
	IA_MoveFwd(nullptr),
	IA_MoveUp(nullptr),
	IA_MoveDown(nullptr),
	IA_Look(nullptr),
	IA_Roll(nullptr),
	IA_Sprint(nullptr)
{ }

void ADroneController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}