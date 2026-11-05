#include "LanunProgressionWidgetBase.h"
#include "LanunHeroProgressionSubsystem.h"
#include "Internationalization/Regex.h"

ULanunProgressionWidgetBase::ULanunProgressionWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULanunProgressionWidgetBase::SetProgressionSubsystem(ULanunHeroProgressionSubsystem* InProgressionSubsystem)
{
	CachedProgressionSubsystem = InProgressionSubsystem;
	Refresh();
}

int32 ULanunProgressionWidgetBase::GetLevel() const
{
	if (CachedProgressionSubsystem)
	{
		return CachedProgressionSubsystem->GetProgressionState().Level;
	}
	return 1;
}

int64 ULanunProgressionWidgetBase::GetCurrentXP() const
{
	if (CachedProgressionSubsystem)
	{
		return CachedProgressionSubsystem->GetProgressionState().CurrentXP;
	}
	return 0;
}

int64 ULanunProgressionWidgetBase::GetXPToNextLevel() const
{
	if (CachedProgressionSubsystem)
	{
		return CachedProgressionSubsystem->GetProgressionState().XPToNextLevel;
	}
	return 0;
}

float ULanunProgressionWidgetBase::GetXPPercent() const
{
	if (CachedProgressionSubsystem)
	{
		const FHeroProgressionState& State = CachedProgressionSubsystem->GetProgressionState();
		if (State.XPToNextLevel > 0)
		{
			return static_cast<float>(State.CurrentXP) / static_cast<float>(State.XPToNextLevel);
		}
	}
	return 0.0f;
}

int32 ULanunProgressionWidgetBase::GetAttributePoints() const
{
	if (CachedProgressionSubsystem)
	{
		return CachedProgressionSubsystem->GetProgressionState().UnallocatedAttributePoints;
	}
	return 0;
}

int32 ULanunProgressionWidgetBase::GetSkillPoints() const
{
	if (CachedProgressionSubsystem)
	{
		return CachedProgressionSubsystem->GetProgressionState().UnallocatedSkillPoints;
	}
	return 0;
}

void ULanunProgressionWidgetBase::Refresh()
{
	if (!CachedProgressionSubsystem)
	{
		return;
	}

	const FHeroProgressionState& State = CachedProgressionSubsystem->GetProgressionState();

	if (State.Level != LastLevel)
	{
		LastLevel = State.Level;
		OnLevelChanged.Broadcast(State.Level);
	}

	if (State.CurrentXP != LastXP || State.XPToNextLevel != LastXPToNext)
	{
		LastXP = State.CurrentXP;
		LastXPToNext = State.XPToNextLevel;
		OnXPChanged.Broadcast(State.CurrentXP, State.XPToNextLevel);
	}

	if (State.UnallocatedAttributePoints != LastAP)
	{
		LastAP = State.UnallocatedAttributePoints;
		OnAttributePointsChanged.Broadcast(State.UnallocatedAttributePoints);
	}

	if (State.UnallocatedSkillPoints != LastSP)
	{
		LastSP = State.UnallocatedSkillPoints;
		OnSkillPointsChanged.Broadcast(State.UnallocatedSkillPoints);
	}
}

void ULanunProgressionWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	Refresh();
}
