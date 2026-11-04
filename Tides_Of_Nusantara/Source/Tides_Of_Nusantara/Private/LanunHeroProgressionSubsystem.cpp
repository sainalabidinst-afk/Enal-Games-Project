#include "LanunHeroProgressionSubsystem.h"
#include "LanunProgressionUtility.h"
#include "LanunAttributeSetBase.h"

void ULanunHeroProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    ProgressionState.Level = 1;
    ProgressionState.CurrentXP = 0;
    ProgressionState.XPToNextLevel = ULanunProgressionUtility::CalculateRequiredXPForLevel(1);
    ProgressionState.UnallocatedAttributePoints = 0;
    ProgressionState.UnallocatedSkillPoints = 0;
    ProgressionState.TotalAttributePointsSpent = 0;
    ProgressionState.TotalSkillPointsSpent = 0;

    AttributeRaw.Strength = 0;
    AttributeRaw.Agility = 0;
    AttributeRaw.Leadership = 0;
    AttributeRaw.Mysticism = 0;

    CachedAttributeSet = nullptr;
}

FHeroProgressionState ULanunHeroProgressionSubsystem::GetProgressionState() const
{
    return ProgressionState;
}

FHeroAttributeRaw ULanunHeroProgressionSubsystem::GetAttributeRaw() const
{
    return AttributeRaw;
}

bool ULanunHeroProgressionSubsystem::AddXP(int64 Amount)
{
    if (Amount <= 0 || ProgressionState.Level >= 100)
    {
        return false;
    }

    ProgressionState.CurrentXP += Amount;

    bool bLeveledUp = false;
    while (ProgressionState.Level < 100)
    {
        const int32 XPNeeded = ULanunProgressionUtility::CalculateRequiredXPForLevel(ProgressionState.Level);
        if (XPNeeded <= 0 || ProgressionState.CurrentXP < XPNeeded)
        {
            break;
        }

        ProgressionState.CurrentXP -= XPNeeded;
        ProgressionState.Level++;
        OnLevelUp(ProgressionState.Level);
        bLeveledUp = true;
    }

    ProgressionState.XPToNextLevel = ULanunProgressionUtility::CalculateRequiredXPForLevel(ProgressionState.Level);
    return bLeveledUp;
}

bool ULanunHeroProgressionSubsystem::AllocateAttributePoint(EHeroAttributeType AttributeType)
{
    if (ProgressionState.UnallocatedAttributePoints <= 0)
    {
        return false;
    }

    int32& TargetRef =
        (AttributeType == EHeroAttributeType::Strength) ? AttributeRaw.Strength :
        (AttributeType == EHeroAttributeType::Agility) ? AttributeRaw.Agility :
        (AttributeType == EHeroAttributeType::Leadership) ? AttributeRaw.Leadership :
        AttributeRaw.Mysticism;

    if (TargetRef >= 100)
    {
        return false;
    }

    TargetRef++;
    ProgressionState.UnallocatedAttributePoints--;
    ProgressionState.TotalAttributePointsSpent++;

    ApplyAttributesToASC();
    return true;
}

bool ULanunHeroProgressionSubsystem::SpendSkillPoint(int32 Cost)
{
    if (Cost <= 0 || ProgressionState.UnallocatedSkillPoints < Cost)
    {
        return false;
    }

    ProgressionState.UnallocatedSkillPoints -= Cost;
    ProgressionState.TotalSkillPointsSpent += Cost;
    return true;
}

void ULanunHeroProgressionSubsystem::RespecAll()
{
    ProgressionState.UnallocatedAttributePoints = ProgressionState.TotalAttributePointsSpent;
    ProgressionState.UnallocatedSkillPoints = ProgressionState.TotalSkillPointsSpent;

    AttributeRaw.Strength = 0;
    AttributeRaw.Agility = 0;
    AttributeRaw.Leadership = 0;
    AttributeRaw.Mysticism = 0;

    ProgressionState.TotalAttributePointsSpent = 0;
    ProgressionState.TotalSkillPointsSpent = 0;

    ApplyAttributesToASC();
}

void ULanunHeroProgressionSubsystem::SetAttributeSet(class ULanunAttributeSetBase* InAttributeSet)
{
    CachedAttributeSet = InAttributeSet;
    ApplyAttributesToASC();
}

void ULanunHeroProgressionSubsystem::ApplyAttributesToASC() const
{
    if (!CachedAttributeSet)
    {
        return;
    }

    CachedAttributeSet->SetStrength(static_cast<float>(AttributeRaw.Strength));
    CachedAttributeSet->SetAgility(static_cast<float>(AttributeRaw.Agility));
    CachedAttributeSet->SetLeadership(static_cast<float>(AttributeRaw.Leadership));
    CachedAttributeSet->SetMysticism(static_cast<float>(AttributeRaw.Mysticism));
}

void ULanunHeroProgressionSubsystem::OnLevelUp(int32 NewLevel)
{
    if (NewLevel < 2 || NewLevel > 100)
    {
        return;
    }

    ProgressionState.UnallocatedAttributePoints += 3;
    ProgressionState.UnallocatedSkillPoints += 1;
}
