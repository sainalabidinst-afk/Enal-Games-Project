#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LanunEncounterPacingSubsystem.generated.h"

UENUM(BlueprintType)
enum class EEncounterTriggerReason : uint8
{
    TimeElapsed,
    DistanceTraveled,
    Manual
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEncounterReady, EEncounterTriggerReason, Reason, float, TimeSinceLastEncounter);

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunEncounterPacingSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

	// UTickableWorldSubsystem profiling
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(ULanunEncounterPacingSubsystem, STATGROUP_Tickables);
	}

	/** Jeda minimum antara encounter (detik). Default 90 detik. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Encounter|Pacing", meta = (ClampMin = "0.0"))
	float EncounterCooldownSeconds = 90.0f;

	/** Jarak minimum sejak encounter terakhir (satuan Unreal, default ~1500 UU = 1.5 mil laut skala rappresentasi). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Encounter|Pacing", meta = (ClampMin = "0.0"))
	float MinDistanceSinceLastEncounter = 1500.0f;

	/** Jumlah encounter simultan maksimal yang diizinkan. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Encounter|Pacing", meta = (ClampMin = "0"))
	int32 MaxSimultaneousEncounters = 1;

	/** Apakah encounter saat ini sedang aktif (cooldown atau sedang berlangsung). */
	UFUNCTION(BlueprintPure, Category = "Encounter|Pacing")
	bool IsEncounterOnCooldown() const;

	/** Apakah sistem boleh memicu encounter baru (tidak cooldown, belum mencapai batas simultan). */
	UFUNCTION(BlueprintPure, Category = "Encounter|Pacing")
	bool CanTriggerEncounter() const;

	/** Waktu yang telah berlalu sejak encounter terakhir (detik). */
	UFUNCTION(BlueprintPure, Category = "Encounter|Pacing")
	float GetTimeSinceLastEncounter() const;

	/** Tandai encounter baru telah dimulai (reset cooldown dan penambah jumlah aktif). */
	UFUNCTION(BlueprintCallable, Category = "Encounter|Pacing")
	void OnEncounterStarted();

	/** Tandai encounter telah selesai (kurangi jumlah aktif). */
	UFUNCTION(BlueprintCallable, Category = "Encounter|Pacing")
	void OnEncounterEnded();

	/** Jumlah encounter yang sedang aktif. */
	UFUNCTION(BlueprintPure, Category = "Encounter|Pacing")
	int32 GetActiveEncounterCount() const;

	UPROPERTY(BlueprintAssignable, Category = "Encounter|Events")
	FOnEncounterReady OnEncounterReady;

private:
	float TimeSinceLastEncounter = 0.0f;
	int32 ActiveEncounterCount = 0;
	float LastEncounterTime = -9999.0f;
};
