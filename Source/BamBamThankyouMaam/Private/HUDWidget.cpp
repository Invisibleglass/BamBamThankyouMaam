// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/ProgressBar.h"

void UHUDWidget::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

