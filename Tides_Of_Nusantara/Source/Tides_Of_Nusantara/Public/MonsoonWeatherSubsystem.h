#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CalendarWorldSubsystem.h"
#include "MonsoonWeatherSubsystem.generated.h"

UENUM(BlueprintType)
enum class EWeatherState : uint8
{
    Clear,
    Cloudy,
    LightRain,
    HeavyStorm,
    TyphoonSquall
};

USTRUCT(BlueprintType)
struct FWeatherParameters
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WaveAmplitudeMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float VolumetricFogDensity = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float SunLightIntensityMultiplier = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float RainIntensity = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
    float WindSpeedMultiplier = 1.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeatherStateChanged, EWeatherState, NewWeatherState, const FWeatherParameters&, Parameters);

UCLASS()
class TIDES_OF_NUSANTARA_API UMonsoonWeatherSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UFUNCTION(BlueprintCallable, Category = "Monsoon Weather")
    void SetTargetWeatherState(EWeatherState NewState, bool bImmediate = false);

    UFUNCTION(BlueprintCallable, Category = "Monsoon Weather")
    void UpdateWeatherDynamics(float DeltaTime);

    UFUNCTION(BlueprintPure, Category = "Monsoon Weather")
    EWeatherState GetCurrentWeatherState() const { return CurrentWeatherState; }

    UFUNCTION(BlueprintPure, Category = "Monsoon Weather")
    FWeatherParameters GetCurrentWeatherParameters() const { return CurrentParameters; }

    UFUNCTION(BlueprintCallable, Category = "Monsoon Weather")
    void TriggerRandomStormSequence();

    UPROPERTY(BlueprintAssignable, Category = "Monsoon Weather|Events")
    FOnWeatherStateChanged OnWeatherStateChanged;

private:
    UPROPERTY()
    EWeatherState CurrentWeatherState = EWeatherState::Clear;

    UPROPERTY()
    EWeatherState TargetWeatherState = EWeatherState::Clear;

    UPROPERTY()
    FWeatherParameters CurrentParameters;

    UPROPERTY()
    FWeatherParameters TargetParameters;

    UPROPERTY()
    UCalendarWorldSubsystem* CachedCalendarSubsystem = nullptr;

    float InterpSpeed = 0.25f;

    FWeatherParameters GetPresetParametersForState(EWeatherState State) const;

    UFUNCTION()
    void HandleHourChanged(int32 NewHour, int32 NewMinute);

    UFUNCTION()
    void HandleDayChanged(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim);
};
