#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
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

UCLASS()
class TIDES_OF_NUSANTARA_API UCalendarWorldSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Tick(float DeltaTime) override;
    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

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

    UPROPERTY(BlueprintAssignable, Category = "Calendar|Events")
    FOnHourChanged OnHourChanged;

    UPROPERTY(BlueprintAssignable, Category = "Calendar|Events")
    FOnDayChanged OnDayChanged;

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

private:
    float ElapsedRealSecondsCurrentDay = 0.0f;
};
