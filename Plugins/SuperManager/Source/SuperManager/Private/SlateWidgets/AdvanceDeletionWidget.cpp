// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"

#include "Widgets/Views/SListView.h"

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
		.AutoHeight()
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
    const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

    TSharedRef<STableRow<TSharedPtr<FAssetData>>> RowWidget = 
    SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
    [
        SNew(STextBlock)
        .Text(FText::FromString(DisplayAssetName))
    ];

    return RowWidget;
}
