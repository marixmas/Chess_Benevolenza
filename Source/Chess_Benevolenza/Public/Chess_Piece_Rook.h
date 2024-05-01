// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chess_Piece.h"
#include "Chess_Piece_Rook.generated.h"

/**
 * 
 */
UCLASS()
class CHESS_BENEVOLENZA_API AChess_Piece_Rook : public AChess_Piece
{
	GENERATED_BODY()

public:
	
	virtual TArray<FVector2D> CalculatePossibleMoves() override;
	//virtual bool IsMoveValid(const FVector2D& Move) override;
	//bool IsAttackValid(const FVector2D& Attack) override;

};

