// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SGraphPin.h"

class SGraphPin_BitmaskEnum : public SGraphPin
{
public:
    SLATE_BEGIN_ARGS(SGraphPin_BitmaskEnum) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEdGraphPin* InPin);

protected:
    virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
    TSharedRef<SWidget> GenerateEnumBitmaskWidget();

    UEnum* GetResolvedEnum() const;
};
