#pragma once

#include "CoreMinimal.h"
#include "LanunGameplayAbilityBase.h"
#include "LanunGameplayAbility_TebasKeris.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunGameplayAbility_TebasKeris : public ULanunGameplayAbilityBase
{
    GENERATED_BODY()

public:
    ULanunGameplayAbility_TebasKeris();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Keris", meta = (ClampMin = "0.0"))
    float BaseMeleeDamage = 45.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Keris", meta = (ClampMin = "0.0"))
    float AttackRange = 180.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Keris", meta = (ClampMin = "0.0"))
    float SweepRadius = 40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|GAS")
    TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;
};
