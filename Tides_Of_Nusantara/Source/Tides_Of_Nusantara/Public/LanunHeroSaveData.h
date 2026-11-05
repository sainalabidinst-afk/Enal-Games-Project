#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LanunHeroSaveData.generated.h"

/** Data progresi Hero untuk persistensi save/load. */
USTRUCT(BlueprintType)
struct FLanunHeroSaveData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	int64 CurrentXP = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	int32 AttributePoints = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	int32 SkillPoints = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	TMap<FGameplayTag, int32> RawAttributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hero|Save")
	TArray<FGameplayTag> UnlockedSkills;
};
