// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyle.h"

class FSuperManagerStyle
{
public:
    static void InitializeIcons();

    static void ShutDown();

public:
    static FName GetStyleSetName() { return CustomStyleSetName;} 

private:
    static FName CustomStyleSetName;

    static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();

    static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;
};
