#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LanunProgressionUtility.generated.h"

UENUM(BlueprintType)
enum class EFactionRelation : uint8
{
	Hostile,
	Neutral,
	Allied
};

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

	/**
	 * Maintenance drag 28% + tagihan Senin 00:00.
	 * Jika DayOfWeek == 0 (Senin) dan Hour == 0, potong 28% dari Rupiah lalu kurangi FixedMondayTax.
	 * Mengembalikan Rupiah sisa (minimal 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Economy|Maintenance")
	static int64 ApplyMondayMaintenance(int64 CurrentRupiah, int32 DayOfWeek, int32 Hour, int64 FixedMondayTax);

	/** Persentase drag maintenanceSenin (28%). */
	UFUNCTION(BlueprintPure, Category = "Lanun|Economy|Maintenance")
	static float GetMondayMaintenanceDragPercent();

	/**
	 * Diminishing returns atribut Hero.
	 * Raw 1–50 = 1.0x, 51–80 = 0.5x, 81–100 = 0.25x. Effective cap = 70.
	 * Input atribut raw harus dibatasi 0–100 oleh pemanggil.
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Attributes|Effective")
	static float CalculateEffectiveAttribute(int32 RawAttribute);

	UFUNCTION(BlueprintPure, Category = "Lanun|Reputation|Faction")
	static EFactionRelation GetFactionRelation(int32 Reputation);

	/**
	 * Peluang mutiny harian jika CrewLoyalty < -50.
	 * Rumus: clamp(abs(CrewLoyalty + 50) × 1.5%, 0%, 100%).
	 * Mengembalikan 0 jika CrewLoyalty >= -50.
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Reputation|Crew")
	static float CalculateMutinyChancePercent(int32 CrewLoyalty);

	/**
	 * Formula sewa dermuka harian: BaseRent × (1 − TerritoryPopularity / 200).
	 * TerritoryPopularity dibatasi -100…+100, faktor biaya 0.5…1.5.
	 * Hasil dibulatkan ke integer Rupiah (floor).
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|Economy|Rent")
	static int64 CalculateDockRent(int64 BaseRent, int32 TerritoryPopularity);

	/**
	 * Biaya unlock skill tree berdasarkan tier.
	 * Tier 1 = 1 SP, Tier 2 = 2 SP, Tier 3 = 3 SP, Tier 4/Ultimate = 5 SP.
	 * Mengembalikan 0 untuk tier invalid.
	 */
	UFUNCTION(BlueprintPure, Category = "Lanun|SkillTree|Cost")
	static int32 GetSkillUnlockCost(int32 Tier);
};
