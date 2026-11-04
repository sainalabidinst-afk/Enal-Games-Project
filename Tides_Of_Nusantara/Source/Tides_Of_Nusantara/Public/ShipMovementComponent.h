#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShipMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ESailState : uint8
{
    NoSails,
    HalfSails,
    FullSails
};

UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent))
class TIDES_OF_NUSANTARA_API UShipMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShipMovementComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Config")
    FName ShipID = FName(TEXT("SHIP_Phinisi_Flagship"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Movement")
    ESailState CurrentSailState = ESailState::NoSails;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Movement", meta = (ClampMin = "0.0"))
    float MaxBaseThrust = 150000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ship Movement", meta = (ClampMin = "0.0"))
    float BaseRudderTorque = 80000.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Movement")
    float CurrentRudderInput = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ship Movement")
    float CurrentSailAngleInput = 0.0f;

    UFUNCTION(BlueprintCallable, Category = "Ship Movement|Rudder")
    void AdjustRudderInput(float Value);

    UFUNCTION(BlueprintCallable, Category = "Ship Movement|Sails")
    void AdjustSailAngleInput(float Value);

    UFUNCTION(BlueprintCallable, Category = "Ship Movement|Sails")
    void SetSailState(ESailState NewSailState);

};
