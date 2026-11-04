#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LanunProgressionUtility.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunProgressionUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * XP yang diperlukan untuk naik dari level L ke L+1 (L = 1…99).
	 * Rumus: floor(500 + 50 * L + 9 * L^1.8)
	 * Mengembalikan 0 untuk level < 1 atau level >= 100 (cap).
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Progression|XP")
	static int32 CalculateRequiredXPForLevel(int32 CurrentLevel);

	/**
	 * Total XP kumulatif untuk mencapai level target (jumlah threshold L=1 sampai L=TargetLevel-1).
	 * Mengembalikan 0 untuk TargetLevel <= 1.
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Progression|XP")
	static int32 CalculateCumulativeXPForLevel(int32 TargetLevel);

	/**
	 * Biaya fast travel antar sektor: SektorDistance × 500 Rp + CrewCount × DailyRationCost.
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Progression|Travel")
	static int64 CalculateFastTravelCost(int32 SectorDistance, int32 CrewCount, int64 DailyRationCostPerCrew);
};
