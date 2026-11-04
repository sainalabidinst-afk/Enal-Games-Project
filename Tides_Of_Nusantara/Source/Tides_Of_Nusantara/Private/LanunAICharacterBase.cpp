#include "LanunAICharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"

ALanunAICharacterBase::ALanunAICharacterBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<ULanunAttributeSetBase>(TEXT("AttributeSet"));

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
}

UAbilitySystemComponent* ALanunAICharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ALanunAICharacterBase::BeginPlay()
{
    Super::BeginPlay();

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->InitAbilityActorInfo(this, this);
    }
}

void ALanunAICharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDead && AttributeSet && AttributeSet->GetHealth() <= 0.0f)
    {
        HandleDeath();
    }
}

bool ALanunAICharacterBase::IsAlive() const
{
    return !bIsDead && AttributeSet && AttributeSet->GetHealth() > 0.0f;
}

void ALanunAICharacterBase::HandleDeath()
{
    bIsDead = true;
    DetachFromControllerPendingDestroy();
    GetCharacterMovement()->DisableMovement();
    SetActorEnableCollision(false);
}
