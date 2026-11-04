#include "LanunHUDWidgetBase.h"

#include "LanunCharacterHeroBase.h"

void ULanunHUDWidgetBase::NativeConstruct()
{
    Super::NativeConstruct();

    UWorld* World = GetWorld();
    if (World)
    {
        CachedCalendarSubsystem = World->GetSubsystem<UCalendarWorldSubsystem>();
        if (CachedCalendarSubsystem)
        {
            CachedCalendarSubsystem->OnHourChanged.AddDynamic(this, &ULanunHUDWidgetBase::HandleHourChanged);
            CachedCalendarSubsystem->OnDayChanged.AddDynamic(this, &ULanunHUDWidgetBase::HandleDayChanged);

            OnTimeUpdated(
                CachedCalendarSubsystem->CurrentHour,
                CachedCalendarSubsystem->CurrentMinute,
                CachedCalendarSubsystem->GetPasaranJawa(),
                CachedCalendarSubsystem->GetMusimAngin());
        }
    }

    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController && PlayerController->GetPawn())
    {
        ALanunCharacterHeroBase* Hero = Cast<ALanunCharacterHeroBase>(PlayerController->GetPawn());
        if (Hero && Hero->GetAttributeSet())
        {
            const ULanunAttributeSetBase* Attributes = Hero->GetAttributeSet();
            OnHealthChanged(Attributes->GetHealth(), Attributes->GetMaxHealth());
            OnStaminaChanged(Attributes->GetStamina(), Attributes->GetMaxStamina());
        }
    }
}

void ULanunHUDWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    APlayerController* PlayerController = GetOwningPlayer();
    if (PlayerController && PlayerController->GetPawn())
    {
        ALanunCharacterHeroBase* Hero = Cast<ALanunCharacterHeroBase>(PlayerController->GetPawn());
        if (Hero && Hero->GetAttributeSet())
        {
            const ULanunAttributeSetBase* Attributes = Hero->GetAttributeSet();
            OnHealthChanged(Attributes->GetHealth(), Attributes->GetMaxHealth());
            OnStaminaChanged(Attributes->GetStamina(), Attributes->GetMaxStamina());
        }
    }
}

void ULanunHUDWidgetBase::HandleHourChanged(int32 NewHour, int32 NewMinute)
{
    if (CachedCalendarSubsystem)
    {
        OnTimeUpdated(NewHour, NewMinute, CachedCalendarSubsystem->GetPasaranJawa(), CachedCalendarSubsystem->GetMusimAngin());
    }
}

void ULanunHUDWidgetBase::HandleDayChanged(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim)
{
    if (CachedCalendarSubsystem)
    {
        OnTimeUpdated(CachedCalendarSubsystem->CurrentHour, CachedCalendarSubsystem->CurrentMinute, Pasaran, Musim);
    }
}
