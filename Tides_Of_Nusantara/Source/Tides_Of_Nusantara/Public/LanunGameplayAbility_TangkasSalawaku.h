#pragma once

#include "CoreMinimal.h"
#include "LanunGameplayAbilityBase.h"
#include "LanunGameplayAbility_TangkasSalawaku.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunGameplayAbility_TangkasSalawaku : public ULanunGameplayAbilityBase
{
	GENERATED_BODY()

public:
	ULanunGameplayAbility_TangkasSalawaku();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry Settings")
	float ParryWindowDuration = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry Settings")
	FGameplayTag ParryActiveTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry Settings")
	UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parry Settings")
	UAnimMontage* CounterAttackMontage;

	FTimerHandle ParryTimerHandle;

	UFUNCTION()
	void OnParryWindowExpired();

	UFUNCTION(BlueprintCallable, Category = "Lanun|Combat|Parry")
	void OnSuccessfulParryTriggered(FGameplayTag EventTag, FGameplayEventData EventData);
};
