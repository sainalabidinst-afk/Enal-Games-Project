#include "CalendarWorldSubsystem.h"
#include "Math/UnrealMathUtility.h"

void UCalendarWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const float SecondsPerGameMinute = RealSecondsPerGameDay / 1440.0f;
    ElapsedRealSecondsCurrentDay = (CurrentHour * 60.0f + CurrentMinute) * SecondsPerGameMinute;
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
