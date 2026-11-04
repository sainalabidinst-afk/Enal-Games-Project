#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "LanunAttributeSetBase.h"
#include "LanunCharacterHeroBase.generated.h"

class AShipPawnBase;

UCLASS()
class TIDES_OF_NUSANTARA_API ALanunCharacterHeroBase : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ALanunCharacterHeroBase();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    ULanunAttributeSetBase* GetAttributeSet() const { return AttributeSet; }

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    ULanunAttributeSetBase* AttributeSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
    UInputAction* InteractAction;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void InteractWithShip(AShipPawnBase* TargetShip);

    UFUNCTION()
    void InteractInput();

protected:
    void UpdateSwimmingBuoyancyPhysics();
};
