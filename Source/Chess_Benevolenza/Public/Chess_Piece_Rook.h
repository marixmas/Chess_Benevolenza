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
	// Sovrascrivi la funzione GetPieceType() per restituire il tipo corretto
	virtual EPieceType GetPieceType() const override;

	// Implementazione della funzione virtuale
	virtual void SpecialAction() const override;
	
};
