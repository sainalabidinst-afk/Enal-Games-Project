#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "LanunAttributeSetBase.h"
#include "LanunAICharacterBase.generated.h"

UENUM(BlueprintType)
enum class ELanunFaction : uint8
{
    PelautBebas,
    VOC_Dutch,
    Kesultanan,
    Masyarakat
};

UCLASS()
class TIDES_OF_NUSANTARA_API ALanunAICharacterBase : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    ALanunAICharacterBase();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    ULanunAttributeSetBase* GetAttributeSet() const { return AttributeSet; }

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Abilities")
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Abilities")
    ULanunAttributeSetBase* AttributeSet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Faction")
    ELanunFaction Faction = ELanunFaction::VOC_Dutch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat", meta = (ClampMin = "0.0"))
    float BaseAttackRange = 180.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|State")
    bool bIsDead = false;

    UFUNCTION(BlueprintPure, Category = "AI|Stats")
    bool IsAlive() const;

protected:
    virtual void HandleDeath();
};
