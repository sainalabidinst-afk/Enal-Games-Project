#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CrewOfficerSubsystem.h"
#include "LanunOfficerDataAsset.generated.h"

UCLASS(BlueprintType)
class TIDES_OF_NUSANTARA_API ULanunOfficerDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Identity")
	FName OfficerID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Identity")
	FText OfficerName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Identity")
	FText BioDescription = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Visuals")
	TSoftObjectPtr<UTexture2D> PortraitTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Attributes")
	EOfficerRole PreferredRole = EOfficerRole::Nakhoda;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Attributes", meta = (ClampMin = "1", ClampMax = "100"))
	int32 LeadershipSkill = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Attributes", meta = (ClampMin = "1", ClampMax = "100"))
	int32 NavigationSkill = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Officer Attributes", meta = (ClampMin = "1", ClampMax = "100"))
	int32 GunnerySkill = 50;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("OfficerData", GetFName());
	}
};
