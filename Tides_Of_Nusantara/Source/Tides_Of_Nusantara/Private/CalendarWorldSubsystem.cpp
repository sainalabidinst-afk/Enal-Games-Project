#include "CalendarWorldSubsystem.h"
#include "Math/UnrealMathUtility.h"
#include "LanunAttributeSetBase.h"
#include "LanunProgressionUtility.h"

void UCalendarWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnDayChanged.AddDynamic(this, &UCalendarWorldSubsystem::HandleDayChanged);

	const float SecondsPerGameMinute = RealSecondsPerGameDay / 1440.0f;
	ElapsedRealSecondsCurrentDay = (CurrentHour * 60.0f + CurrentMinute) * SecondsPerGameMinute;
}

void UCalendarWorldSubsystem::Deinitialize()
{
	OnDayChanged.RemoveDynamic(this, &UCalendarWorldSubsystem::HandleDayChanged);
	Super::Deinitialize();
}

bool UCalendarWorldSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UCalendarWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RealSecondsPerGameDay <= 0.0f || DeltaTime <= 0.0f)
	{
		return;
	}

	ElapsedRealSecondsCurrentDay += DeltaTime;

	const float GameSecondsPerRealSecond = 86400.0f / RealSecondsPerGameDay;
	const int32 TotalGameSeconds = FMath::FloorToInt(ElapsedRealSecondsCurrentDay * GameSecondsPerRealSecond);
	const int32 NewHour = (TotalGameSeconds / 3600) % 24;
	const int32 NewMinute = (TotalGameSeconds / 60) % 60;

	const bool bMinuteChanged = NewMinute != CurrentMinute;

	CurrentHour = NewHour;
	CurrentMinute = NewMinute;

	if (bMinuteChanged)
	{
		OnHourChanged.Broadcast(CurrentHour, CurrentMinute);
	}

	while (ElapsedRealSecondsCurrentDay >= RealSecondsPerGameDay)
	{
		ElapsedRealSecondsCurrentDay -= RealSecondsPerGameDay;
		++TotalDaysPassed;
		++CurrentDayOfMonth;

		if (CurrentDayOfMonth > 30)
		{
			CurrentDayOfMonth = 1;
			++CurrentMonth;

			if (CurrentMonth > 12)
			{
				CurrentMonth = 1;
				++CurrentYear;
			}
		}

		OnDayChanged.Broadcast(TotalDaysPassed, GetPasaranJawa(), GetMusimAngin());
	}
}

EPasaranJawa UCalendarWorldSubsystem::GetPasaranJawa() const
{
	return static_cast<EPasaranJawa>(TotalDaysPassed % 5);
}

EMusimAngin UCalendarWorldSubsystem::GetMusimAngin() const
{
	if (CurrentMonth >= 11 || CurrentMonth <= 3)
	{
		return EMusimAngin::MusonBarat;
	}

	if (CurrentMonth >= 5 && CurrentMonth <= 9)
	{
		return EMusimAngin::MusonTimur;
	}

	return EMusimAngin::Pancaroba;
}

FVector UCalendarWorldSubsystem::GetWindDirectionVector() const
{
	switch (GetMusimAngin())
	{
	case EMusimAngin::MusonBarat:
		return FVector(0.866f, 0.5f, 0.0f).GetSafeNormal();
	case EMusimAngin::MusonTimur:
		return FVector(-0.866f, -0.5f, 0.0f).GetSafeNormal();
	default:
		return FVector(0.1f, 0.9f, 0.0f).GetSafeNormal();
	}
}

float UCalendarWorldSubsystem::GetTidalZOffset() const
{
	const float LunarPhaseAngle = (static_cast<float>(CurrentDayOfMonth) / 30.0f) * 2.0f * UE_PI;
	const float TideAmplitude = FMath::Abs(FMath::Sin(LunarPhaseAngle));
	return FMath::Lerp(-150.0f, 150.0f, TideAmplitude);
}

int32 UCalendarWorldSubsystem::GetDayOfWeek() const
{
	return TotalDaysPassed % 7;
}

int64 UCalendarWorldSubsystem::CalculateDailyCrewWages() const
{
	return (static_cast<int64>(ActiveCrewCount) * DailyWagePerCrew) +
		(static_cast<int64>(ActiveOfficerCount) * DailyWagePerOfficer);
}

int64 UCalendarWorldSubsystem::CalculateWeeklyGarrisonMaintenance() const
{
	return (static_cast<int64>(OwnedFortCount) * WeeklyFortMaintenance) +
		(static_cast<int64>(OwnedMineCount) * WeeklyMineMaintenance) +
		WeeklyRouteMaintenance;
}

void UCalendarWorldSubsystem::HandleDayChanged(int32 InTotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim)
{
	ULanunAttributeSetBase* AttributeSet = PlayerAttributeSet;
	if (!AttributeSet)
	{
		return;
	}

	// 1. Biaya upah harian kru dan perwira
	const int64 DailyWages = CalculateDailyCrewWages();
	if (DailyWages > 0)
	{
		if (AttributeSet->CanAffordRupiah(DailyWages))
		{
			AttributeSet->SpendRupiah(DailyWages);
		}
		else
		{
			OnCrewWagesUnpaid.Broadcast(-15);
		}
	}

	// 2. Maintenance drag mingguan (hanya Senin 00:00)
	if (GetDayOfWeek() == 0 && CurrentHour == 0)
	{
		const int64 WeeklyMaintenance = CalculateWeeklyGarrisonMaintenance();
		const int64 CurrentRupiah = AttributeSet->GetRupiah();

		// Terapkan drag 28% + fixed tax via utility
		const int64 RemainingRupiah = ULanunProgressionUtility::ApplyMondayMaintenance(
			CurrentRupiah, GetDayOfWeek(), CurrentHour, FixedMondayTax);

		const int64 Deducted = CurrentRupiah - RemainingRupiah;
		if (Deducted > 0)
		{
			AttributeSet->SpendRupiah(Deducted);
		}

		// Biaya pemeliharaan benteng/tambang tambahan
		if (WeeklyMaintenance > 0)
		{
			if (AttributeSet->CanAffordRupiah(WeeklyMaintenance))
			{
				AttributeSet->SpendRupiah(WeeklyMaintenance);
			}
			else
			{
				OnMaintenanceUnpaid.Broadcast(WeeklyMaintenance);
			}
		}
	}
}
