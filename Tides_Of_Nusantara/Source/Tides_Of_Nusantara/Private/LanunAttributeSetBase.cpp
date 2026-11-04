#include "LanunAttributeSetBase.h"

#include "GameplayEffectExtension.h"

ULanunAttributeSetBase::ULanunAttributeSetBase()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitStamina(100.0f);
    InitMaxStamina(100.0f);
    InitArmorWeight(0.0f);
    InitSwimmingBuoyancy(1.0f);
}

void ULanunAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
    else if (Attribute == GetSwimmingBuoyancyAttribute())
    {
        NewValue = FMath::Clamp(NewValue, -1.0f, 1.0f);
    }
}

void ULanunAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
    {
        SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
    }
    else if (Data.EvaluatedData.Attribute == GetArmorWeightAttribute())
    {
        const float NewBuoyancy = FMath::Clamp(1.0f - (GetArmorWeight() * 0.02f), -1.0f, 1.0f);
        SetSwimmingBuoyancy(NewBuoyancy);
    }
}
