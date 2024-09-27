// Copyright The Knights of Unity. All rights reserved.

#include "Validators/PrintStringValidator.h"

bool UPrintStringValidator::CanValidateAsset_Implementation(UObject* InAsset) const
{
	return Cast<UBlueprint>(InAsset) != nullptr;
}

EDataValidationResult UPrintStringValidator::ValidateLoadedAsset_Implementation(UObject* InAsset, TArray<FText>& ValidationErrors)
{
	auto BP = Cast<UBlueprint>(InAsset);
	if(BP)
	{
		TArray<UEdGraph*> BPGraphs;
		BP->GetAllGraphs(BPGraphs);
		for(const auto BPGraph : BPGraphs)
		{
			for(const auto Node : BPGraph->Nodes)
			{
				// Check UE PrintString and PrintText
				FText NodeTitle = Node->GetNodeTitle(ENodeTitleType::ListView);
				if(NodeTitle.EqualTo(FText::FromString(TEXT("Print String"))) ||
					NodeTitle.EqualTo(FText::FromString(TEXT("Print Text"))))
				{
					UEdGraphPin* ExecInPin = Node->FindPin(TEXT("execute"), EEdGraphPinDirection::EGPD_Input);
					bool IsPrintConnected = (ExecInPin ? ExecInPin->LinkedTo.Num() > 0 : false);
					UEdGraphPin* PrintToScreenPin = Node->FindPin(TEXT("bPrintToScreen"));
					bool IsPrintingToScreen = (PrintToScreenPin ? PrintToScreenPin->DefaultValue.Equals(TEXT("true")) : false);
					if(IsPrintConnected && IsPrintingToScreen)
					{
						AssetFails(InAsset, FText::FromString(TEXT("Please disable prints to screen for all PrintString() and PrintText() in this BP")), ValidationErrors);
						return EDataValidationResult::Invalid;
					}
				}
			}
		}
	}

	AssetPasses(InAsset);
	return EDataValidationResult::Valid;
}
