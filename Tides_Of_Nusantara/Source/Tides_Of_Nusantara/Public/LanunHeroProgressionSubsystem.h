#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LanunAttributeSetBase.h"
#include "LanunHeroSaveData.h"
#include "LanunHeroProgressionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FHeroProgressionState
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int32 Level = 1;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int64 CurrentXP = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int64 XPToNextLevel = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int32 UnallocatedAttributePoints = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int32 UnallocatedSkillPoints = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int32 TotalAttributePointsSpent = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Progression")
    int32 TotalSkillPointsSpent = 0;
};

USTRUCT(BlueprintType)
struct FHeroAttributeRaw
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Attributes")
    int32 Strength = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Attributes")
    int32 Agility = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Attributes")
    int32 Leadership = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Hero|Attributes")
    int32 Mysticism = 0;
};

UCLASS()
class TIDES_OF_NUSANTARA_API ULanunHeroProgressionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Ambil state progresi Hero saat ini. */
    UFUNCTION(BlueprintPure, Category = "Hero|Progression")
    FHeroProgressionState GetProgressionState() const;

    /** Ambil nilai atribut raw Hero saat ini. */
    UFUNCTION(BlueprintPure, Category = "Hero|Attributes")
    FHeroAttributeRaw GetAttributeRaw() const;

    /** Tambah XP ke Hero. Mengembalikan true jika level naik. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    bool AddXP(int64 Amount);

    /** Alokasikan satu Attribute Point ke atribut yang dipilih. Mengembalikan true jika berhasil. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    bool AllocateAttributePoint(EHeroAttributeType AttributeType);

    /** Alokasikan satu Skill Point (biaya ditentukan oleh tier skill di luar subsistem ini). Mengembalikan true jika cukup poin. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    bool SpendSkillPoint(int32 Cost);

    /** Kembalikan semua poin yang telah dialokasikan (respec). Biaya: Level × 1000 Rp, dipanggil oleh pemanggil. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    void RespecAll();

    /** Atur referensi ke ULanunAttributeSetBase untuk menerapkan atribut ke ASC. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    void SetAttributeSet(class ULanunAttributeSetBase* InAttributeSet);

    /** Terapkan nilai atribut raw ke ULanunAttributeSetBase (ASC). */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression")
    void ApplyAttributesToASC() const;

    /** Muat data progresi Hero dari save file. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression|Save")
    void LoadHeroData(const FLanunHeroSaveData& SaveData);

    /** Ambil semua atribut raw sebagai map untuk persistensi. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Attributes")
    TMap<FGameplayTag, int32> GetAllRawAttributes() const;

    /** Ambil daftar skill yang ter-unlock untuk persistensi. */
    UFUNCTION(BlueprintCallable, Category = "Hero|Progression|Save")
    TArray<FGameplayTag> GetUnlockedSkills() const;

    /** Ambil nilai Rupiah dari AttributeSet yang dicache. */
    UFUNCTION(BlueprintCallable, Category = "Currency|Rupiah")
    int64 GetRupiahFromASC() const;

    /** Set nilai Rupiah ke AttributeSet yang dicache. */
    UFUNCTION(BlueprintCallable, Category = "Currency|Rupiah")
    void SetRupiahToASC(int64 Amount);

private:
    void OnLevelUp(int32 NewLevel);

    FHeroProgressionState ProgressionState;
    FHeroAttributeRaw AttributeRaw;

    class ULanunAttributeSetBase* CachedAttributeSet = nullptr;
};
