// Copyright The Knights of Unity. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SPVSettings.generated.h"

/**
 * 
 */
UCLASS(Config = Engine, defaultconfig)
class SAMURAIPROJECTVALIDATIONEDITOR_API USPVSettings : public UObject
{
	GENERATED_BODY()

public:
	/** Allows to enable verbose validation logging:
	 * - Prints validation metrics for each function.
	 */ 
	UPROPERTY(Config, EditAnywhere, Category = "Data Validation")
	bool bEnableVerboseLogging {false};
};
