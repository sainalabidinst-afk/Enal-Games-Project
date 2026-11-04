#pragma once

#include "CoreMinimal.h"
#include "LanunGameplayAbilityBase.h"
#include "LanunGameplayAbility_TembakSenapanPasak.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunGameplayAbility_TembakSenapanPasak : public ULanunGameplayAbilityBase
{
    GENERATED_BODY()

public:
    ULanunGameplayAbility_TembakSenapanPasak();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Senapan", meta = (ClampMin = "0.0"))
    float RangedDamage = 85.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Senapan", meta = (ClampMin = "0.0"))
    float MaxFireDistance = 4000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|GAS")
    TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;
};
