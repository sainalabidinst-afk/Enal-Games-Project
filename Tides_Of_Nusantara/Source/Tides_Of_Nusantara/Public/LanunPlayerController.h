#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "LanunPlayerController.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ALanunPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Context")
	UInputMappingContext* IMC_OnFoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Context")
	UInputMappingContext* IMC_Sailing;

	UFUNCTION(BlueprintCallable, Category = "Input|Context")
	void EnableOnFootContext();

	UFUNCTION(BlueprintCallable, Category = "Input|Context")
	void EnableSailingContext();

protected:
	virtual void BeginPlay() override;
};
