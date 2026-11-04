#include "LanunDamageExecutionCalculation.h"

#include "LanunAttributeSetBase.h"

struct FLanunDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
    DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorWeight);

    FLanunDamageStatics()
    {
        DEFINE_ATTRIBUTE_CAPTUREDEF(ULanunAttributeSetBase, Health, Target, false);
        DEFINE_ATTRIBUTE_CAPTUREDEF(ULanunAttributeSetBase, ArmorWeight, Target, false);
    }
};

static const FLanunDamageStatics& DamageStatics()
{
    static FLanunDamageStatics Statics;
    return Statics;
}

ULanunDamageExecutionCalculation::ULanunDamageExecutionCalculation()
{
    RelevantAttributesToCapture.Add(DamageStatics().HealthDef);
    RelevantAttributesToCapture.Add(DamageStatics().ArmorWeightDef);
}

void ULanunDamageExecutionCalculation::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluationParameters;
    EvaluationParameters.SourceTags = SourceTags;
    EvaluationParameters.TargetTags = TargetTags;

    float ArmorWeight = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
        DamageStatics().ArmorWeightDef,
        EvaluationParameters,
        ArmorWeight);
    ArmorWeight = FMath::Max(ArmorWeight, 0.0f);

    const FGameplayTag DamageTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Data.Damage")), false);
    const float RawDamage = DamageTag.IsValid()
        ? Spec.GetSetByCallerMagnitude(DamageTag, false, 0.0f)
        : 0.0f;

    const float ArmorMitigationFactor = FMath::Clamp(1.0f - (ArmorWeight * 0.005f), 0.25f, 1.0f);
    const float MitigatedDamage = RawDamage * ArmorMitigationFactor;

    if (MitigatedDamage > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
            DamageStatics().HealthProperty,
            EGameplayModOp::Additive,
            -MitigatedDamage));
    }
}
