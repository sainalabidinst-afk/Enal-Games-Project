#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CalendarWorldSubsystem.h"
#include "LanunCalendarWidgetBase.generated.h"

// Gunakan nama unik untuk delegate widget agar tidak tabrakan UHT signature
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCalendarWidgetDayChanged, int32, NewDay);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCalendarWidgetHourChanged, int32, NewHour, int32, NewMinute);

/**
 * Base class untuk UI Kalender & Maintenance.
 * Bind ke UCalendarWorldSubsystem untuk menampilkan hari, jam, dan estimasi tagihan mingguan.
 */
UCLASS()
class TIDES_OF_NUSANTARA_API ULanunCalendarWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	ULanunCalendarWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** Set referensi ke UCalendarWorldSubsystem. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	void SetCalendarSubsystem(class UCalendarWorldSubsystem* InCalendarSubsystem);

	/** Ambil tahun saat ini. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetYear() const;

	/** Ambil bulan saat ini (1-12). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetMonth() const;

	/** Ambil hari dalam bulan (1-30). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetDayOfMonth() const;

	/** Ambil jam saat ini (0-23). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetHour() const;

	/** Ambil menit saat ini (0-59). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetMinute() const;

	/** Ambil hari dalam minggu (0 = Senin, 6 = Minggu). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int32 GetDayOfWeek() const;

	/** Ambil nama hari dalam Bahasa Indonesia. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	FText GetDayNameText(int32 DayIndex) const;

	/** Ambil musim angin saat ini. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	EMusimAngin GetMusimAngin() const;

	/** Ambil estimasi biaya maintenance mingguan (benteng + tambang + rute). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	int64 GetWeeklyMaintenanceCost() const;

	/** Segarkan tampilan. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Calendar")
	void Refresh();

	/** Event saat hari berubah (widget-specific). */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Calendar")
	FOnCalendarWidgetDayChanged OnWidgetDayChanged;

	/** Event saat jam berubah (widget-specific). */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Calendar")
	FOnCalendarWidgetHourChanged OnWidgetHourChanged;

protected:
	/** Referensi ke Calendar Subsystem. */
	UPROPERTY()
	class UCalendarWorldSubsystem* CachedCalendarSubsystem = nullptr;

	/** Nilai terakhir untuk deteksi perubahan. */
	int32 LastDayOfMonth = 0;
	int32 LastHour = 0;
	int32 LastMinute = 0;
	int64 LastMaintenanceCost = 0;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
