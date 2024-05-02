// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UENUM()
enum class ESign : uint8
{
	B,
	N,
	V
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CHESS_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	int32 PlayerNumber;
	ESign Sign;

	virtual void OnTurn() {};
	virtual void OnWin() {};
	virtual void OnLose() {};
	virtual void OnTie() {};
	virtual void IsCheck() {};
};
