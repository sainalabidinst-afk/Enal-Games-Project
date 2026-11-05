#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LanunHeroProgressionSubsystem.h"
#include "LanunProgressionWidgetBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChanged, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChanged, int64, NewXP, int64, XPToNext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributePointsChanged, int32, NewAP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillPointsChanged, int32, NewSP);

/**
 * Base class untuk UI Progression Hero.
 * Bind ke ULanunHeroProgressionSubsystem untuk menampilkan Level, XP Bar, AP, SP.
 */
UCLASS()
class TIDES_OF_NUSANTARA_API ULanunProgressionWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	ULanunProgressionWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** Set referensi ke ULanunHeroProgressionSubsystem. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	void SetProgressionSubsystem(class ULanunHeroProgressionSubsystem* InProgressionSubsystem);

	/** Ambil level Hero saat ini. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	int32 GetLevel() const;

	/** Ambil XP saat ini. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	int64 GetCurrentXP() const;

	/** Ambil XP yang diperlukan untuk level berikutnya. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	int64 GetXPToNextLevel() const;

	/** Ambil persentase XP bar (0.0 - 1.0). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	float GetXPPercent() const;

	/** Ambil poin atribut yang belum dialokasikan. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	int32 GetAttributePoints() const;

	/** Ambil poin skill yang belum dialokasikan. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	int32 GetSkillPoints() const;

	/** Segarkan tampilan dan broadcast event jika ada perubahan. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|UI|Progression")
	void Refresh();

	/** Event saat level berubah. */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Events")
	FOnLevelChanged OnLevelChanged;

	/** Event saat XP berubah. */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Events")
	FOnXPChanged OnXPChanged;

	/** Event saat Attribute Points berubah. */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Events")
	FOnAttributePointsChanged OnAttributePointsChanged;

	/** Event saat Skill Points berubah. */
	UPROPERTY(BlueprintAssignable, Category = "Lanun|UI|Events")
	FOnSkillPointsChanged OnSkillPointsChanged;

protected:
	/** Referensi ke Progression Subsystem. */
	UPROPERTY()
	class ULanunHeroProgressionSubsystem* CachedProgressionSubsystem = nullptr;

	/** Nilai terakhir untuk deteksi perubahan. */
	int32 LastLevel = 0;
	int64 LastXP = 0;
	int64 LastXPToNext = 0;
	int32 LastAP = 0;
	int32 LastSP = 0;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
