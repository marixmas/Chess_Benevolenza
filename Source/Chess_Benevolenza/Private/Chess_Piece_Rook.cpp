// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Rook.h"
#include "Chess_GameMode.h"

EPieceType AChess_Piece_Rook::GetPieceType() const
{
    // Restituisce il tipo specifico del pezzo
    return EPieceType::ROOK;
}

void AChess_Piece_Rook::SpecialAction() const
{
    // Implementazione delle azioni specifiche per la torre
}

TArray<FVector2D> AChess_Piece_Rook::CalculatePossibleMoves()
{
    // TArray<FVector2D> PossibleMoves; così mi da errore di sovrascrizione della PossibleMoves variabile di classe dichiarata in Chess_Piece

    // Implementazione specifica per il pezzo Pawn

    return PossibleMoves;
}
