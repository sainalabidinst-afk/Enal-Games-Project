#include "LanunGameplayAbility_TangkasSalawaku.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

ULanunGameplayAbility_TangkasSalawaku::ULanunGameplayAbility_TangkasSalawaku()
{
	AbilityInputID = ELanunAbilityInputID::SpecialAbility;
	StaminaCost = 20.0f;
	ParryActiveTag = FGameplayTag::RequestGameplayTag(FName(TEXT("State.Combat.Parrying")));
}

void ULanunGameplayAbility_TangkasSalawaku::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAbilitySystemComponent* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (ASC && Character)
	{
		ASC->AddLooseGameplayTag(ParryActiveTag);

		if (ParryMontage)
		{
			Character->PlayAnimMontage(ParryMontage);
		}

		GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &ULanunGameplayAbility_TangkasSalawaku::OnParryWindowExpired, ParryWindowDuration, false);
	}
}

void ULanunGameplayAbility_TangkasSalawaku::OnParryWindowExpired()
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(ParryActiveTag);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void ULanunGameplayAbility_TangkasSalawaku::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(ParryActiveTag);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ParryTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULanunGameplayAbility_TangkasSalawaku::OnSuccessfulParryTriggered(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (!ParryActiveTag.IsValid() || !CurrentActorInfo || !CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		return;
	}

	if (!CurrentActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(ParryActiveTag))
	{
		return;
	}

	if (CounterAttackMontage)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
		if (Character)
		{
			Character->PlayAnimMontage(CounterAttackMontage);
		}
	}

	CurrentActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(ParryActiveTag);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
