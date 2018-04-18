// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "IUnitState.h"
/**
 * 
 */
class MYPROJECT_API InteractState : public IUnitState
{
public:

	InteractState();
	~InteractState();

	virtual void Enter(AUnit& unit) override;
	virtual void Exit(AUnit& unit) override;
	virtual void Update(AUnit& unit) override;
	virtual FName GetName() const override { return "InteractState"; }
	
};
