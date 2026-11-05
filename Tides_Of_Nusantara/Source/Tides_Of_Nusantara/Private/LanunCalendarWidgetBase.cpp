#include "LanunCalendarWidgetBase.h"

#define LOCTEXT_NAMESPACE "LanunCalendarUI"

ULanunCalendarWidgetBase::ULanunCalendarWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ULanunCalendarWidgetBase::SetCalendarSubsystem(UCalendarWorldSubsystem* InCalendarSubsystem)
{
	CachedCalendarSubsystem = InCalendarSubsystem;
	Refresh();
}

int32 ULanunCalendarWidgetBase::GetYear() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CurrentYear : 1521;
}

int32 ULanunCalendarWidgetBase::GetMonth() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CurrentMonth : 1;
}

int32 ULanunCalendarWidgetBase::GetDayOfMonth() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CurrentDayOfMonth : 1;
}

int32 ULanunCalendarWidgetBase::GetHour() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CurrentHour : 6;
}

int32 ULanunCalendarWidgetBase::GetMinute() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CurrentMinute : 0;
}

int32 ULanunCalendarWidgetBase::GetDayOfWeek() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->GetDayOfWeek() : 0;
}

FText ULanunCalendarWidgetBase::GetDayNameText(int32 DayIndex) const
{
	switch (DayIndex % 7)
	{
	case 0: return LOCTEXT("Senin", "Senin");
	case 1: return LOCTEXT("Selasa", "Selasa");
	case 2: return LOCTEXT("Rabu", "Rabu");
	case 3: return LOCTEXT("Kamis", "Kamis");
	case 4: return LOCTEXT("Jumat", "Jumat");
	case 5: return LOCTEXT("Sabtu", "Sabtu");
	case 6: return LOCTEXT("Minggu", "Minggu");
	default: return LOCTEXT("Senin", "Senin");
	}
}

EMusimAngin ULanunCalendarWidgetBase::GetMusimAngin() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->GetMusimAngin() : EMusimAngin::MusonBarat;
}

int64 ULanunCalendarWidgetBase::GetWeeklyMaintenanceCost() const
{
	return CachedCalendarSubsystem ? CachedCalendarSubsystem->CalculateWeeklyGarrisonMaintenance() : 0;
}

void ULanunCalendarWidgetBase::Refresh()
{
	if (!CachedCalendarSubsystem)
	{
		return;
	}

	if (GetDayOfMonth() != LastDayOfMonth)
	{
		LastDayOfMonth = GetDayOfMonth();
		OnWidgetDayChanged.Broadcast(LastDayOfMonth);
	}

	if (GetHour() != LastHour || GetMinute() != LastMinute)
	{
		LastHour = GetHour();
		LastMinute = GetMinute();
		OnWidgetHourChanged.Broadcast(LastHour, LastMinute);
	}

	const int64 MaintenanceCost = GetWeeklyMaintenanceCost();
	if (MaintenanceCost != LastMaintenanceCost)
	{
		LastMaintenanceCost = MaintenanceCost;
	}
}

void ULanunCalendarWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	Refresh();
}

#undef LOCTEXT_NAMESPACE
