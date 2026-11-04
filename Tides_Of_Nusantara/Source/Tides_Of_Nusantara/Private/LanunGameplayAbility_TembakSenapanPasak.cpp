#include "LanunGameplayAbility_TembakSenapanPasak.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

ULanunGameplayAbility_TembakSenapanPasak::ULanunGameplayAbility_TembakSenapanPasak()
{
    AbilityInputID = ELanunAbilityInputID::SpecialAbility;
    StaminaCost = 25.0f;
}

void ULanunGameplayAbility_TembakSenapanPasak::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    UAbilitySystemComponent* SourceASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;
    UWorld* World = AvatarActor ? AvatarActor->GetWorld() : nullptr;
    if (!AvatarActor || !SourceASC || !World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FVector StartLocation = AvatarActor->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
    const FVector EndLocation = StartLocation + AvatarActor->GetActorForwardVector() * MaxFireDistance;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LanunSenapanTrace), false);
    QueryParams.AddIgnoredActor(AvatarActor);
    const bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Pawn,
        QueryParams);

    const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Damage")), false);
    if (bHit && DamageGameplayEffectClass && DamageTag.IsValid())
    {
        AActor* HitActor = HitResult.GetActor();
        UAbilitySystemComponent* TargetASC = HitActor ? HitActor->FindComponentByClass<UAbilitySystemComponent>() : nullptr;
        if (TargetASC)
        {
            FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
            ContextHandle.AddHitResult(HitResult);
            FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
                DamageGameplayEffectClass,
                GetAbilityLevel(),
                ContextHandle);

            if (SpecHandle.IsValid())
            {
                SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, RangedDamage);
                SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
