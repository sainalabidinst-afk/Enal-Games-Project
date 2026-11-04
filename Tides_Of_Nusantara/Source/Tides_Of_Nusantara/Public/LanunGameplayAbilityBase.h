#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimMontage.h"
#include "LanunGameplayAbilityBase.generated.h"

UENUM(BlueprintType)
enum class ELanunAbilityInputID : uint8
{
    None,
    Confirm,
    Cancel,
    PrimaryAttack,
    SecondaryAttack,
    SpecialAbility,
    UseJamu,
    Interacting
};

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	ULanunGameplayAbilityBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Config")
	ELanunAbilityInputID AbilityInputID = ELanunAbilityInputID::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Config", meta = (ClampMin = "0.0"))
	float StaminaCost = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability|Animation")
	UAnimMontage* AbilityMontage;
};
