#include "MonsoonWeatherSubsystem.h"

void UMonsoonWeatherSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    CurrentParameters = GetPresetParametersForState(EWeatherState::Clear);
    TargetParameters = CurrentParameters;

    UWorld* World = GetWorld();
    if (World)
    {
        CachedCalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
        if (CachedCalendarSubsystem)
        {
            CachedCalendarSubsystem->OnHourChanged.AddDynamic(this, &UMonsoonWeatherSubsystem::HandleHourChanged);
            CachedCalendarSubsystem->OnDayChanged.AddDynamic(this, &UMonsoonWeatherSubsystem::HandleDayChanged);
        }
    }
}

FWeatherParameters UMonsoonWeatherSubsystem::GetPresetParametersForState(EWeatherState State) const
{
    FWeatherParameters Parameters;

    switch (State)
    {
    case EWeatherState::Cloudy:
        Parameters.WaveAmplitudeMultiplier = 1.6f;
        Parameters.VolumetricFogDensity = 0.025f;
        Parameters.SunLightIntensityMultiplier = 0.65f;
        Parameters.RainIntensity = 0.0f;
        Parameters.WindSpeedMultiplier = 1.8f;
        break;
    case EWeatherState::LightRain:
        Parameters.WaveAmplitudeMultiplier = 2.2f;
        Parameters.VolumetricFogDensity = 0.045f;
        Parameters.SunLightIntensityMultiplier = 0.45f;
        Parameters.RainIntensity = 0.35f;
        Parameters.WindSpeedMultiplier = 2.2f;
        break;
    case EWeatherState::HeavyStorm:
        Parameters.WaveAmplitudeMultiplier = 4.5f;
        Parameters.VolumetricFogDensity = 0.09f;
        Parameters.SunLightIntensityMultiplier = 0.2f;
        Parameters.RainIntensity = 0.8f;
        Parameters.WindSpeedMultiplier = 4.0f;
        break;
    case EWeatherState::TyphoonSquall:
        Parameters.WaveAmplitudeMultiplier = 7.0f;
        Parameters.VolumetricFogDensity = 0.18f;
        Parameters.SunLightIntensityMultiplier = 0.05f;
        Parameters.RainIntensity = 1.0f;
        Parameters.WindSpeedMultiplier = 6.0f;
        break;
    case EWeatherState::Clear:
    default:
        Parameters.WaveAmplitudeMultiplier = 1.0f;
        Parameters.VolumetricFogDensity = 0.008f;
        Parameters.SunLightIntensityMultiplier = 1.0f;
        Parameters.RainIntensity = 0.0f;
        Parameters.WindSpeedMultiplier = 1.0f;
        break;
    }

    return Parameters;
}

void UMonsoonWeatherSubsystem::SetTargetWeatherState(EWeatherState NewState, bool bImmediate)
{
    TargetWeatherState = NewState;
    TargetParameters = GetPresetParametersForState(NewState);

    if (bImmediate)
    {
        CurrentWeatherState = TargetWeatherState;
        CurrentParameters = TargetParameters;
        OnWeatherStateChanged.Broadcast(CurrentWeatherState, CurrentParameters);
    }
}

void UMonsoonWeatherSubsystem::UpdateWeatherDynamics(float DeltaTime)
{
    if (DeltaTime <= 0.0f)
    {
        return;
    }

    CurrentParameters.WaveAmplitudeMultiplier = FMath::FInterpTo(CurrentParameters.WaveAmplitudeMultiplier, TargetParameters.WaveAmplitudeMultiplier, DeltaTime, InterpSpeed);
    CurrentParameters.VolumetricFogDensity = FMath::FInterpTo(CurrentParameters.VolumetricFogDensity, TargetParameters.VolumetricFogDensity, DeltaTime, InterpSpeed);
    CurrentParameters.SunLightIntensityMultiplier = FMath::FInterpTo(CurrentParameters.SunLightIntensityMultiplier, TargetParameters.SunLightIntensityMultiplier, DeltaTime, InterpSpeed);
    CurrentParameters.RainIntensity = FMath::FInterpTo(CurrentParameters.RainIntensity, TargetParameters.RainIntensity, DeltaTime, InterpSpeed);
    CurrentParameters.WindSpeedMultiplier = FMath::FInterpTo(CurrentParameters.WindSpeedMultiplier, TargetParameters.WindSpeedMultiplier, DeltaTime, InterpSpeed);

    if (CurrentWeatherState != TargetWeatherState &&
        FMath::IsNearlyEqual(CurrentParameters.WaveAmplitudeMultiplier, TargetParameters.WaveAmplitudeMultiplier, 0.1f))
    {
        CurrentWeatherState = TargetWeatherState;
    }

    OnWeatherStateChanged.Broadcast(CurrentWeatherState, CurrentParameters);
}

void UMonsoonWeatherSubsystem::TriggerRandomStormSequence()
{
    const int32 Chance = FMath::RandRange(1, 100);
    SetTargetWeatherState(Chance > 70 ? EWeatherState::TyphoonSquall : EWeatherState::HeavyStorm);
}

void UMonsoonWeatherSubsystem::HandleHourChanged(int32 NewHour, int32 NewMinute)
{
    if ((NewHour == 5 || NewHour == 18) && CurrentWeatherState == EWeatherState::Clear)
    {
        SetTargetWeatherState(EWeatherState::Cloudy);
    }
    else if (NewHour == 10 && CurrentWeatherState == EWeatherState::Cloudy)
    {
        SetTargetWeatherState(EWeatherState::Clear);
    }
}

void UMonsoonWeatherSubsystem::HandleDayChanged(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim)
{
    const int32 StormRoll = FMath::RandRange(1, 100);

    if (Musim == EMusimAngin::MusonBarat)
    {
        if (StormRoll <= 35)
        {
            SetTargetWeatherState(EWeatherState::HeavyStorm);
        }
        else if (StormRoll <= 60)
        {
            SetTargetWeatherState(EWeatherState::LightRain);
        }
        else
        {
            SetTargetWeatherState(EWeatherState::Clear);
        }
    }
    else if (Musim == EMusimAngin::Pancaroba)
    {
        SetTargetWeatherState(StormRoll <= 20 ? EWeatherState::TyphoonSquall : EWeatherState::Clear);
    }
    else
    {
        SetTargetWeatherState(StormRoll <= 10 ? EWeatherState::LightRain : EWeatherState::Clear);
    }
}
