#include "LanunGameplayAbility_TebasKeris.h"

#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/Actor.h"

ULanunGameplayAbility_TebasKeris::ULanunGameplayAbility_TebasKeris()
{
    AbilityInputID = ELanunAbilityInputID::PrimaryAttack;
    StaminaCost = 15.0f;
}

void ULanunGameplayAbility_TebasKeris::ActivateAbility(
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

    const FVector StartLocation = AvatarActor->GetActorLocation();
    const FVector EndLocation = StartLocation + AvatarActor->GetActorForwardVector() * AttackRange;

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(LanunKerisSweep), false);
    QueryParams.AddIgnoredActor(AvatarActor);

    TArray<FHitResult> HitResults;
    const FCollisionShape SphereShape = FCollisionShape::MakeSphere(SweepRadius);
    const bool bHit = World->SweepMultiByChannel(
        HitResults,
        StartLocation,
        EndLocation,
        FQuat::Identity,
        ECC_Pawn,
        SphereShape,
        QueryParams);

    const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Damage")), false);
    if (bHit && DamageGameplayEffectClass && DamageTag.IsValid())
    {
        TSet<UAbilitySystemComponent*> DamagedTargets;
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            UAbilitySystemComponent* TargetASC = HitActor ? HitActor->FindComponentByClass<UAbilitySystemComponent>() : nullptr;
            if (!TargetASC || DamagedTargets.Contains(TargetASC))
            {
                continue;
            }

            DamagedTargets.Add(TargetASC);
            FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
            ContextHandle.AddHitResult(Hit);
            FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
                DamageGameplayEffectClass,
                GetAbilityLevel(),
                ContextHandle);

            if (SpecHandle.IsValid())
            {
                SpecHandle.Data->SetSetByCallerMagnitude(DamageTag, BaseMeleeDamage);
                SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
