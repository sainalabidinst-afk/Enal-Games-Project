#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LanunAttributeSetBase.generated.h"

UENUM(BlueprintType)
enum class EHeroAttributeType : uint8
{
    Strength,
    Agility,
    Leadership,
    Mysticism
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunAttributeSetBase : public UAttributeSet
{
    GENERATED_BODY()

public:
    ULanunAttributeSetBase();

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, MaxHealth)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, MaxStamina)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Physics")
    FGameplayAttributeData ArmorWeight;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, ArmorWeight)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Physics")
    FGameplayAttributeData SwimmingBuoyancy;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, SwimmingBuoyancy)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Hero")
    FGameplayAttributeData Strength;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Strength)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Hero")
    FGameplayAttributeData Agility;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Agility)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Hero")
    FGameplayAttributeData Leadership;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Leadership)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Hero")
    FGameplayAttributeData Mysticism;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, Mysticism)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Points")
    FGameplayAttributeData UnallocatedAttributePoints;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, UnallocatedAttributePoints)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Points")
    FGameplayAttributeData UnallocatedSkillPoints;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, UnallocatedSkillPoints)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Reputation")
    FGameplayAttributeData FactionReputation;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, FactionReputation)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Reputation")
    FGameplayAttributeData TerritoryPopularity;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, TerritoryPopularity)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes|Reputation")
    FGameplayAttributeData CrewLoyalty;
    ATTRIBUTE_ACCESSORS(ULanunAttributeSetBase, CrewLoyalty)

    /** Dompet Rupiah terpusat (int64) — migrasi dari PlayerGold terpecah di subsistem. */
    UPROPERTY(BlueprintReadOnly, Category = "Currency|Rupiah")
    int64 Rupiah;

    UFUNCTION(BlueprintCallable, Category = "Lanun|Progression")
    bool AllocateAttributePoint(EHeroAttributeType AttributeType);

    UFUNCTION(BlueprintCallable, Category = "Currency|Rupiah")
    bool CanAffordRupiah(int64 Amount) const;

    UFUNCTION(BlueprintCallable, Category = "Currency|Rupiah")
    bool SpendRupiah(int64 Amount);

    UFUNCTION(BlueprintCallable, Category = "Currency|Rupiah")
    void AddRupiah(int64 Amount);

    UFUNCTION(BlueprintPure, Category = "Currency|Rupiah")
    int64 GetRupiah() const;
};
