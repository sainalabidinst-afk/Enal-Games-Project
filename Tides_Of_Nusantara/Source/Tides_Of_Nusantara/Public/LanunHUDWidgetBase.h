#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CalendarWorldSubsystem.h"
#include "LanunAttributeSetBase.h"
#include "LanunHUDWidgetBase.generated.h"

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunHUDWidgetBase : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Lanun UI|Events")
    void OnHealthChanged(float NewHealth, float MaxHealth);

    UFUNCTION(BlueprintImplementableEvent, Category = "Lanun UI|Events")
    void OnStaminaChanged(float NewStamina, float MaxStamina);

    UFUNCTION(BlueprintImplementableEvent, Category = "Lanun UI|Events")
    void OnTimeUpdated(int32 Hour, int32 Minute, EPasaranJawa Pasaran, EMusimAngin Musim);

protected:
    UPROPERTY()
    UCalendarWorldSubsystem* CachedCalendarSubsystem = nullptr;

    UFUNCTION()
    void HandleHourChanged(int32 NewHour, int32 NewMinute);

    UFUNCTION()
    void HandleDayChanged(int32 TotalDaysPassed, EPasaranJawa Pasaran, EMusimAngin Musim);
};
