#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "RivalNemesisSubsystem.generated.h"

UENUM(BlueprintType)
enum class ERivalRelationshipState : uint8
{
	Unmet,
	Neutral,
	Rival,
	Nemesis,      // Grudge < -60
	Ally,         // Grudge > +50
	Subordinated  // Faksi tunduk / membayar upeti
};

USTRUCT(BlueprintType)
struct FRivalTacticalMemory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Tactics")
	int32 TotalEncounters = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Tactics")
	float RammingWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Tactics")
	float BroadsideWeight = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Tactics")
	float BoardingWeight = 0.0f;

	void RecordAction(FName ActionType)
	{
		TotalEncounters++;
		if (ActionType == "Ramming")
		{
			RammingWeight += 1.0f;
		}
		else if (ActionType == "Broadside")
		{
			BroadsideWeight += 1.0f;
		}
		else if (ActionType == "Boarding")
		{
			BoardingWeight += 1.0f;
		}
	}

	FName GetDominantTactic() const
	{
		if (RammingWeight >= BroadsideWeight && RammingWeight >= BoardingWeight)
		{
			return "Ramming";
		}
		if (BroadsideWeight >= RammingWeight && BroadsideWeight >= BoardingWeight)
		{
			return "Broadside";
		}
		return "Boarding";
	}
};

USTRUCT(BlueprintType)
struct FRivalProfile
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Profile")
	FName CaptainID = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Profile")
	FText CaptainName;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Profile")
	ERivalRelationshipState State = ERivalRelationshipState::Unmet;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Profile")
	int32 GrudgeScore = 0; // Rentang -100 s/d +100

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Tactics")
	FRivalTacticalMemory TacticalMemory;

	UPROPERTY(BlueprintReadOnly, Category = "Nemesis|Profile")
	FGameplayTagContainer CounterPerks;
};

UCLASS()
class TIDES_OF_NUSANTARA_API URivalNemesisSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Catat aksi taktis pertempuran terhadap kapten rival. */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Nemesis")
	void RecordCombatAction(FName CaptainID, FName ActionType);

	/** Ubah skor dendam terhadap kapten rival (diklimp ke -100…+100). */
	UFUNCTION(BlueprintCallable, Category = "Lanun|Nemesis")
	void ModifyGrudgeScore(FName CaptainID, int32 Delta);

	/** Ambil profil lengkap kapten rival. */
	UFUNCTION(BlueprintPure, Category = "Lanun|Nemesis")
	FRivalProfile GetRivalProfile(FName CaptainID) const;

	/** Tentukan state hubungan berdasarkan GrudgeScore. */
	UFUNCTION(BlueprintPure, Category = "Lanun|Nemesis")
	static ERivalRelationshipState GetRelationshipStateFromGrudge(int32 GrudgeScore);

private:
	UPROPERTY()
	TMap<FName, FRivalProfile> RivalRegistry;
};
