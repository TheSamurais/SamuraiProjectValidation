// Copyright The Knights of Unity. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "EditorValidatorBase.h"
#include "PrintStringValidator.generated.h"

/**
 * 
 */
UCLASS()
class UPrintStringValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

public:
	virtual bool CanValidateAsset_Implementation(UObject* InAsset) const override;
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;
};
