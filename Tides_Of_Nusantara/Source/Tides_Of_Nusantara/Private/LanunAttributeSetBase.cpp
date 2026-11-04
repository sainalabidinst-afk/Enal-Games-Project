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

    InitStrength(0.0f);
    InitAgility(0.0f);
    InitLeadership(0.0f);
    InitMysticism(0.0f);

    InitUnallocatedAttributePoints(0.0f);
    InitUnallocatedSkillPoints(0.0f);
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
    else if (Attribute == GetStrengthAttribute()
        || Attribute == GetAgilityAttribute()
        || Attribute == GetLeadershipAttribute()
        || Attribute == GetMysticismAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, 100.0f);
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
    else if (Data.EvaluatedData.Attribute == GetStrengthAttribute()
        || Data.EvaluatedData.Attribute == GetAgilityAttribute()
        || Data.EvaluatedData.Attribute == GetLeadershipAttribute()
        || Data.EvaluatedData.Attribute == GetMysticismAttribute())
    {
        const float NewValue = FMath::Clamp(Data.EvaluatedData.Attribute == GetStrengthAttribute() ? GetStrength()
            : Data.EvaluatedData.Attribute == GetAgilityAttribute() ? GetAgility()
            : Data.EvaluatedData.Attribute == GetLeadershipAttribute() ? GetLeadership()
            : GetMysticism(), 0.0f, 100.0f);
        if (Data.EvaluatedData.Attribute == GetStrengthAttribute())
        {
            SetStrength(NewValue);
        }
        else if (Data.EvaluatedData.Attribute == GetAgilityAttribute())
        {
            SetAgility(NewValue);
        }
        else if (Data.EvaluatedData.Attribute == GetLeadershipAttribute())
        {
            SetLeadership(NewValue);
        }
        else if (Data.EvaluatedData.Attribute == GetMysticismAttribute())
        {
            SetMysticism(NewValue);
        }
    }
}

bool ULanunAttributeSetBase::AllocateAttributePoint(EHeroAttributeType AttributeType)
{
    if (GetUnallocatedAttributePoints() <= 0.0f)
    {
        return false;
    }

    SetUnallocatedAttributePoints(GetUnallocatedAttributePoints() - 1.0f);

    switch (AttributeType)
    {
    case EHeroAttributeType::Strength:
        SetStrength(GetStrength() + 1.0f);
        break;
    case EHeroAttributeType::Agility:
        SetAgility(GetAgility() + 1.0f);
        break;
    case EHeroAttributeType::Leadership:
        SetLeadership(GetLeadership() + 1.0f);
        break;
    case EHeroAttributeType::Mysticism:
        SetMysticism(GetMysticism() + 1.0f);
        break;
    }

    return true;
}
