#include "LanunPlayerController.h"
#include "EnhancedInputSubsystems.h"

void ALanunPlayerController::BeginPlay()
{
	Super::BeginPlay();

	EnableOnFootContext();
}

void ALanunPlayerController::EnableOnFootContext()
{
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Sailing)
			{
				Subsystem->RemoveMappingContext(IMC_Sailing);
			}

			if (IMC_OnFoot)
			{
				Subsystem->AddMappingContext(IMC_OnFoot, 0);
			}
		}
	}
}

void ALanunPlayerController::EnableSailingContext()
{
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_OnFoot)
			{
				Subsystem->RemoveMappingContext(IMC_OnFoot);
			}

			if (IMC_Sailing)
			{
				Subsystem->AddMappingContext(IMC_Sailing, 1);
			}
		}
	}
}
