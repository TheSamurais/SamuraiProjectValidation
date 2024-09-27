// Copyright The Knights of Unity. All rights reserved.


#include "Validators/CyclomaticComplexityValidator.h"

#include "EditorValidatorSubsystem.h"
#include "SPVSettings.h"

class UDataValidationSettings;

bool UCyclomaticComplexityValidator::CanValidateAsset_Implementation(UObject* InAsset) const
{
	return Cast<UBlueprint>(InAsset) != nullptr;
}

EDataValidationResult UCyclomaticComplexityValidator::ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors)
{
	uint32 BPCyclomaticComplexity = 0;
	auto BP = Cast<UBlueprint>(InAsset);
	TArray<TObjectPtr<UEdGraphNode>> BPEntryNodes;
	GetAllConnectedEntryNodesInBP(BP, BPEntryNodes);

	for(auto BPEntryNode : BPEntryNodes)
	{
		VisitedNodesIds.Empty();
		uint32 Complexity = CountBranches(BPEntryNode, 1);
		BPCyclomaticComplexity += Complexity;
		
		if(GetDefault<USPVSettings>()->bEnableVerboseLogging)
		{
			LogFunctionComplexity(InAsset, BPEntryNode, Complexity);
		}
	}

	const auto Message = FString::Format(TEXT("{0} total Cyclomatic Complexity = {1}"), {*InAsset->GetName(), *FString::FromInt(BPCyclomaticComplexity)});
	FMessageLog("AssetCheck").Info(FText::FromString(Message));
	AssetPasses(InAsset);
	return EDataValidationResult::Valid;
}

void UCyclomaticComplexityValidator::GetAllConnectedEntryNodesInBP(UBlueprint* BP, TArray<TObjectPtr<UEdGraphNode>>& BPEntryNodes) const
{
	if(!BP)
	{
		return;
	}
	
	TArray<UEdGraph*> BPGraphs;

	// Find all Entry Nodes in this BP (first Node in the function, event Nodes in Event Graph)
	BP->GetAllGraphs(BPGraphs);
	for(const auto BPGraph : BPGraphs)
	{
		for(const auto Node : BPGraph->Nodes)
		{
			UEdGraphPin* ExecInPin = Node->FindPin(TEXT("execute"), EEdGraphPinDirection::EGPD_Input);
			UEdGraphPin* ExecOutPin = Node->FindPin(TEXT("then"), EEdGraphPinDirection::EGPD_Output);
			// If the Node has no ExecIn pin but has ExecOut pin and ExecOut is connected to something, save the Entry Node 
			if(!ExecInPin && ExecOutPin && !ExecOutPin->LinkedTo.IsEmpty())
			{
				BPEntryNodes.Add(Node);
			}
		}
	}
}

uint32 UCyclomaticComplexityValidator::CountBranches(TObjectPtr<UEdGraphNode> Node, uint32 BranchCounter)
{
	// If Node does not exist, do not increase complexity 
	if(!Node)
	{
		return BranchCounter;
	}
	FText NodeTitle = Node->GetNodeTitle(ENodeTitleType::ListView);
	VisitedNodesIds.Add(Node->GetUniqueID());

	TArray<UEdGraphPin*> ExecOutPins;
	TArray<TObjectPtr<UEdGraphNode>> UniqueNodes;
	GetExecOutPins(Node, ExecOutPins);
	GetUniqueNodesConnectedToPins(ExecOutPins, UniqueNodes);

	// The first exec flow is already counted in the incoming BranchCounter, therefore decrease num flows by 1.
	// However, we only count flows connected to something, and we don't count this something twice, that's why we have unique Nodes. 
	BranchCounter += FMath::Clamp(UniqueNodes.Num() - 1, 0, UniqueNodes.Num());
	
	for(auto UniqueNode : UniqueNodes)
	{
		if(!VisitedNodesIds.Contains(UniqueNode->GetUniqueID()))
		{
			BranchCounter = CountBranches(UniqueNode, BranchCounter);
		}
	}

	return BranchCounter;
}

void UCyclomaticComplexityValidator::GetExecOutPins(TObjectPtr<UEdGraphNode> Node, TArray<UEdGraphPin*>& ExecOutPins) const
{
	if(Node)
	{
		ExecOutPins = Node->Pins.FilterByPredicate([](UEdGraphPin* Pin)
		{
			// If Pin is an ExecOut type, save it
			if (Pin
				&& Pin->PinType.PinCategory == TEXT("exec")
				&& Pin->Direction == EEdGraphPinDirection::EGPD_Output)
			{
				return true;
			}
			return false;
		});
	}
}

void UCyclomaticComplexityValidator::GetUniqueNodesConnectedToPins(TArray<UEdGraphPin*>& ExecOutPins, TArray<TObjectPtr<UEdGraphNode>>& UniqueNodes) const
{
	// For each ExecOut...
	for(const auto ExecOutPin : ExecOutPins)
	{
		// if ExecOut is connected to Node, add this Node to the list once.
		if(!ExecOutPin->LinkedTo.IsEmpty())
		{
			UniqueNodes.AddUnique(ExecOutPin->LinkedTo[0]->GetOwningNode());
		}
	}
}

void UCyclomaticComplexityValidator::LogFunctionComplexity(UObject* InAsset, TObjectPtr<UEdGraphNode> BPEntryNode, uint32 Complexity)
{
	const auto Message = FString::Format(
			TEXT("{0}::{1} Cyclomatic Complexity = {2}"), {
				*InAsset->GetName(),
				BPEntryNode->GetNodeTitle(ENodeTitleType::ListView).ToString(),
				*FString::FromInt(Complexity)
			});
	FMessageLog("AssetCheck").Info(FText::FromString(Message));
}
