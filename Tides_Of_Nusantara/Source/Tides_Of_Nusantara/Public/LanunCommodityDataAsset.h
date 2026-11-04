#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TradeEconomySubsystem.h"
#include "LanunCommodityDataAsset.generated.h"

UCLASS(BlueprintType)
class TIDES_OF_NUSANTARA_API ULanunCommodityDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Identity")
	FName CommodityID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Identity")
	FText DisplayName = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Identity")
	FText Description = FText::GetEmpty();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Visuals")
	TSoftObjectPtr<UTexture2D> IconTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Economy")
	ECommodityType CommodityType = ECommodityType::Pala;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Economy")
	float BasePricePerTon = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Economy")
	float PriceVolatilityRate = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Commodity Economy")
	FName PrimaryOriginPortID = FName("PORT_Banda");

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("CommodityData", GetFName());
	}
};
