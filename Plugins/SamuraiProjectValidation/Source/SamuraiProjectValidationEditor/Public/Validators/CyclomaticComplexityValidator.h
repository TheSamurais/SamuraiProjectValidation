// Copyright The Knights of Unity. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "CyclomaticComplexityValidator.generated.h"

/**
 * 
 */
UCLASS()
class UCyclomaticComplexityValidator : public UEditorValidatorBase
{
	GENERATED_BODY()

public:
	virtual bool CanValidateAsset_Implementation(UObject* InAsset) const override;
	virtual EDataValidationResult ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors) override;

protected:
	void GetAllConnectedEntryNodesInBP(UBlueprint* BP, TArray<TObjectPtr<UEdGraphNode>>& BPEntryNodes) const;
	uint32 CountBranches(TObjectPtr<UEdGraphNode> Node, uint32 BranchCounter);
	void GetExecOutPins(TObjectPtr<UEdGraphNode> Node, TArray<UEdGraphPin*>& ExecOutPins) const;
	void GetUniqueNodesConnectedToPins(TArray<UEdGraphPin*>& ExecOutPins, TArray<TObjectPtr<UEdGraphNode>>& UniqueNodes) const;
	void LogFunctionComplexity(UObject* InAsset, TObjectPtr<UEdGraphNode> BPEntryNode, uint32 Complexity);

private:
	TArray<uint32> VisitedNodesIds;
};
