#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LanunAttributeSetBase.generated.h"

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
};
