// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "Widgets/Views/SListView.h"
#include "DebugHeader.h"

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

    StoredAssetData = InArgs._AssetsDataToStore;

	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	ChildSlot
	[
		SNew(SVerticalBox)

		//First vertical slot for title text
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("Advance Deletion")))
				.Font(TitleTextFont)
				.Justification(ETextJustify::Center)
				.ColorAndOpacity(FColor::White)
		]

		//Secondslot for drop down to specify the listing condition
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]

		//Thirdslot for the asset list
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)

            +SScrollBox::Slot()
            [
                SNew(SListView<TSharedPtr<FAssetData>>)
                .ItemHeight(24.f)
                .ListItemsSource(&StoredAssetData)
                .OnGenerateRow(this, &SAdvanceDeletionTab::OnGenerateRowForList)
            ]
		]

		//Fourthslot for 3 buttons
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SHorizontalBox)
        ]
	];
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(
    TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
    if(!AssetDataToDisplay.IsValid()) 
        return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);

    const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

    TSharedRef<STableRow<TSharedPtr<FAssetData>>> RowWidget = 
    SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
    [
        SNew(SHorizontalBox)

        // First Slot for check box
        +SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Center)
        .FillWidth(0.05f)
        [
            ConstructCheckBox(AssetDataToDisplay)
        ]

        // Second Slot for displaying asset class name
        + SHorizontalBox::Slot()
        [
            SNew(STextBlock)
                .Text(FText::FromString(DisplayAssetName))
        ]

        // Third Slot for displaying asset name
         
        // Third Slot for a button
    ];

    return RowWidget;
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
    TSharedRef<SCheckBox> Result = SNew(SCheckBox)
    .Type(ESlateCheckBoxType::CheckBox)
    .OnCheckStateChanged(this, &SAdvanceDeletionTab::OnCheckBoxStateChanged, AssetDataToDisplay)
    .Visibility(EVisibility::Visible);

    return Result;
}

void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
    switch (NewState)
    {
    case ECheckBoxState::Checked:
        DebugHeader::Print(AssetData->AssetName.ToString() + TEXT("Is Checked"), FColor::Blue);
        break;
    case ECheckBoxState::Unchecked:
        DebugHeader::Print(AssetData->AssetName.ToString() + TEXT("Is Unchecked"), FColor::Blue);
        break;
    case ECheckBoxState::Undetermined:
        DebugHeader::Print(AssetData->AssetName.ToString() + TEXT("Is Undetermined"), FColor::Blue);
        break;
    default:
        break;
    }

    
}
