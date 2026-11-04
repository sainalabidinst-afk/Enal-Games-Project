#include "LanunProgressionUtility.h"

#include "Math/UnrealMathUtility.h"

int32 ULanunProgressionUtility::CalculateRequiredXPForLevel(int32 CurrentLevel)
{
	if (CurrentLevel < 1 || CurrentLevel >= 100)
	{
		return 0;
	}

	const double L = static_cast<double>(CurrentLevel);
	const double XP = 500.0 + 50.0 * L + 9.0 * FMath::Pow(L, 1.8);
	return FMath::FloorToInt(XP);
}

int32 ULanunProgressionUtility::CalculateCumulativeXPForLevel(int32 TargetLevel)
{
	if (TargetLevel <= 1)
	{
		return 0;
	}

	int32 Total = 0;
	for (int32 L = 1; L < TargetLevel && L < 100; ++L)
	{
		Total += CalculateRequiredXPForLevel(L);
	}
	return Total;
}

int64 ULanunProgressionUtility::CalculateFastTravelCost(int32 SectorDistance, int32 CrewCount, int64 DailyRationCostPerCrew)
{
	if (SectorDistance < 0 || CrewCount < 0 || DailyRationCostPerCrew < 0)
	{
		return 0;
	}

	const int64 DistanceCost = static_cast<int64>(SectorDistance) * 500LL;
	const int64 CrewCost = static_cast<int64>(CrewCount) * DailyRationCostPerCrew;
	return DistanceCost + CrewCost;
}

int64 ULanunProgressionUtility::ApplyMondayMaintenance(int64 CurrentRupiah, int32 DayOfWeek, int32 Hour, int64 FixedMondayTax)
{
	if (DayOfWeek != 0 || Hour != 0)
	{
		return CurrentRupiah;
	}

	// Drag 28%
	const int64 DragAmount = FMath::FloorToInt64(static_cast<double>(CurrentRupiah) * 0.28);
	int64 Remaining = CurrentRupiah - DragAmount;

	// Tagihan Senin tetap
	if (FixedMondayTax > 0 && Remaining >= FixedMondayTax)
	{
		Remaining -= FixedMondayTax;
	}
	else if (FixedMondayTax > 0)
	{
		Remaining = 0;
	}

	return FMath::Max<int64>(Remaining, 0);
}

float ULanunProgressionUtility::GetMondayMaintenanceDragPercent()
{
	return 28.0f;
}

float ULanunProgressionUtility::CalculateEffectiveAttribute(int32 RawAttribute)
{
	const int32 Clamped = FMath::Clamp(RawAttribute, 0, 100);

	const float Part1 = FMath::Min(Clamped, 50) * 1.0f;
	const float Part2 = FMath::Clamp(Clamped - 50, 0, 30) * 0.5f;
	const float Part3 = FMath::Clamp(Clamped - 80, 0, 20) * 0.25f;

	return FMath::Min(Part1 + Part2 + Part3, 70.0f);
}

EFactionRelation ULanunProgressionUtility::GetFactionRelation(int32 Reputation)
{
	if (Reputation < -30)
	{
		return EFactionRelation::Hostile;
	}
	if (Reputation > 30)
	{
		return EFactionRelation::Allied;
	}
	return EFactionRelation::Neutral;
}

float ULanunProgressionUtility::CalculateMutinyChancePercent(int32 CrewLoyalty)
{
	if (CrewLoyalty >= -50)
	{
		return 0.0f;
	}

	const float Chance = FMath::Abs(static_cast<float>(CrewLoyalty + 50)) * 1.5f;
	return FMath::Clamp(Chance, 0.0f, 100.0f);
}

int64 ULanunProgressionUtility::CalculateDockRent(int64 BaseRent, int32 TerritoryPopularity)
{
	const int32 ClampedPopularity = FMath::Clamp(TerritoryPopularity, -100, 100);
	const float Factor = 1.0f - (static_cast<float>(ClampedPopularity) / 200.0f);
	const double Result = static_cast<double>(BaseRent) * static_cast<double>(Factor);
	return FMath::FloorToInt64(Result);
}

int32 ULanunProgressionUtility::GetSkillUnlockCost(int32 Tier)
{
	switch (Tier)
	{
	case 1:
		return 1;
	case 2:
		return 2;
	case 3:
		return 3;
	case 4:
		return 5;
	default:
		return 0;
	}
}
