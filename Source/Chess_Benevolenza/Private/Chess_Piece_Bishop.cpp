// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Bishop.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_Bishop::CalculatePossibleMoves()
{
   // TArray<FVector2D> PossibleMoves; così mi da errore di sovrascrizione della PossibleMoves variabile di classe dichiarata in Chess_Piece

    // Implementazione specifica per il pezzo Pawn

    return PossibleMoves;
}
