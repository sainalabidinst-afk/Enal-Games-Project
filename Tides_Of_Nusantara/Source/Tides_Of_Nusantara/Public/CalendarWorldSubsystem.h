#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AbilitySystemInterface.h"
#include "LanunAttributeSetBase.h"
#include "CalendarWorldSubsystem.generated.h"

UENUM(BlueprintType)
enum class EPasaranJawa : uint8
{
	Legi,
	Pahing,
	Pon,
	Wage,
	Kliwon
};

UENUM(BlueprintType)
enum class EMusimAngin : uint8
{
	MusonBarat,
	Pancaroba,
	MusonTimur
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHourChanged, int32, NewHour, int32, NewMinute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDayChanged, int32, TotalDaysPassed, EPasaranJawa, Pasaran, EMusimAngin, Musim);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrewWagesUnpaid, int32, LoyaltyPenalty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaintenanceUnpaid, int64, UnpaidAmount);

UCLASS()
class TIDES_OF_NUSANTARA_API UCalendarWorldSubsystem : public UTickableWorldSubsystem
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
		RETURN_QUICK_DECLARE_CYCLE_STAT(UCalendarWorldSubsystem, STATGROUP_Tickables);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Calendar|Config")
	float RealSecondsPerGameDay = 2700.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 CurrentYear = 1521;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 CurrentMonth = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 CurrentDayOfMonth = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 CurrentHour = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 CurrentMinute = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Calendar|State")
	int32 TotalDaysPassed = 0;

	/** Upah harian per awak kapal. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Wages", meta = (ClampMin = "0"))
	int32 DailyWagePerCrew = 15;

	/** Upah harian per perwira. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Wages", meta = (ClampMin = "0"))
	int32 DailyWagePerOfficer = 50;

	/** Jumlah awak kapal aktif (diatur dari CrewOfficerSubsystem/ShipFleetSubsystem). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Wages", meta = (ClampMin = "0"))
	int32 ActiveCrewCount = 0;

	/** Jumlah perwira aktif. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Wages", meta = (ClampMin = "0"))
	int32 ActiveOfficerCount = 0;

	/** Biaya pemeliharaan benteng per minggu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int64 WeeklyFortMaintenance = 2500;

	/** Biaya pemeliharaan tambang per minggu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int64 WeeklyMineMaintenance = 1200;

	/** Jumlah benteng yang dimiliki pemain. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int32 OwnedFortCount = 0;

	/** Jumlah tambang yang dimiliki pemain. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int32 OwnedMineCount = 0;

	/** Biaya rute perdagangan abstract per minggu. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int64 WeeklyRouteMaintenance = 0;

	/** Tagihan tetap Senin tambahan pada drag 28%. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maintenance|Weekly", meta = (ClampMin = "0"))
	int64 FixedMondayTax = 0;

	UPROPERTY(BlueprintAssignable, Category = "Calendar|Events")
	FOnHourChanged OnHourChanged;

	UPROPERTY(BlueprintAssignable, Category = "Calendar|Events")
	FOnDayChanged OnDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Maintenance|Events")
	FOnCrewWagesUnpaid OnCrewWagesUnpaid;

	UPROPERTY(BlueprintAssignable, Category = "Maintenance|Events")
	FOnMaintenanceUnpaid OnMaintenanceUnpaid;

	UFUNCTION(BlueprintPure, Category = "Calendar|Calculations")
	EPasaranJawa GetPasaranJawa() const;

	UFUNCTION(BlueprintPure, Category = "Calendar|Calculations")
	EMusimAngin GetMusimAngin() const;

	UFUNCTION(BlueprintPure, Category = "Calendar|Calculations")
	FVector GetWindDirectionVector() const;

	UFUNCTION(BlueprintPure, Category = "Calendar|Calculations")
	float GetTidalZOffset() const;

	/**
	 * Hari dalam minggu (0 = Senin, 1 = Selasa, …, 6 = Minggu).
	 * Digunakan untuk pemicu tagihan Senin 00:00.
	 */
	UFUNCTION(BlueprintPure, Category = "Calendar|Calculations")
	int32 GetDayOfWeek() const;

	/** Hitung total upah harian kru dan perwira. */
	UFUNCTION(BlueprintCallable, Category = "Maintenance|Calculation")
	int64 CalculateDailyCrewWages() const;

	/** Hitung total biaya pemeliharaan aset mingguan. */
	UFUNCTION(BlueprintCallable, Category = "Maintenance|Calculation")
	int64 CalculateWeeklyGarrisonMaintenance() const;

	/** Referensi ke ULanunAttributeSetBase pemain (diatur secara eksternal dari karakter pemain). */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Maintenance|State")
	class ULanunAttributeSetBase* PlayerAttributeSet = nullptr;

private:
	void HandleDayChanged(int32 InTotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim);

	float ElapsedRealSecondsCurrentDay = 0.0f;
};
