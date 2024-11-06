// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANAGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
    
#pragma region QuickMaterialCreationCore

    UFUNCTION(BlueprintCallable)
    void CreateQuickMaterialFromSeletedTextures();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromSeletedTextures")
    bool bCustomMaterialName = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CreateMaterialFromSeletedTextures", meta = (EditCondition = "bCustomMaterialName"))
    FString MaterialName = TEXT("M_");

#pragma endregion

private:

#pragma region QuickMaterialCreationCore

    bool ProcessSeletedData(const TArray<FAssetData>& SeletedAssets, 
        TArray<UTexture2D*>& OutSeletedTexturesArray, FString& OutSeletedTexturesData);

#pragma endregion
};
