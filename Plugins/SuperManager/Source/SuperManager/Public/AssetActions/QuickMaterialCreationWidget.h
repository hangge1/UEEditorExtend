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

#pragma region QuickMaterialCreation

    bool ProcessSeletedData(const TArray<FAssetData>& SeletedAssets, 
        TArray<UTexture2D*>& OutSeletedTexturesArray, FString& OutSeletedTexturesData);

    bool CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck);

    UMaterial* CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPut);
#pragma endregion
};
