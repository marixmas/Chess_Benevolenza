// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Chess_PlayerInterface.generated.h"

															//non mi sembra utile qui, c'� gi� altrove, nel ttt c'�
UENUM()
enum class EColorOfPieces : uint8
{
	WHITE,
	BLACK,
	NONE
};



// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UChess_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CHESS_BENEVOLENZA_API IChess_PlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	int32 PlayerNumber;
	EColorOfPieces ColorOfPieces;						// no??

	virtual void OnTurn() {};
	virtual void OnWin() {};
	virtual void OnLose() {};


};
