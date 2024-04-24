// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Pawn.h"

TArray<FVector2D> AChess_Piece_Pawn::CalculatePossibleMoves()
{
    /*
    // TArray<FVector2D> PossibleMoves; così mi da errore di sovrascrizione della PossibleMoves variabile di classe dichiarata in Chess_Piece

    // Implementazione specifica per il pezzo Pawn

    PossibleMoves.Empty();

    // Direzione del movimento in base al colore del pedone
    int32 ForwardDirection = (PieceColor == EPieceColor::WHITE) ? 1 : -1;

    // Calcola la posizione attuale del pedone
    int32 CurrentX = FMath::RoundToInt(TileGridPosition.X);
    int32 CurrentY = FMath::RoundToInt(TileGridPosition.Y);

    // Mossa normale in avanti
    FVector2D ForwardMove = FVector2D(CurrentX, CurrentY + ForwardDirection);
    if (!GameMode->IsTileOccupied(ForwardMove))
    {
        PossibleMoves.Add(ForwardMove);

        // Se il pedone non si è ancora mosso, può fare una mossa doppia
        if ((PieceColor == EPieceColor::WHITE && CurrentY == 1) || (PieceColor == EPieceColor::BLACK && CurrentY == 6))
        {
            FVector2D DoubleForwardMove = FVector2D(CurrentX, CurrentY + (2 * ForwardDirection));
            if (!GameMode->IsTileOccupied(DoubleForwardMove))
            {
                PossibleMoves.Add(DoubleForwardMove);
            }
        }
    }

    // Mosse di cattura diagonali
    FVector2D CaptureMoveLeft = FVector2D(CurrentX - 1, CurrentY + ForwardDirection);
    FVector2D CaptureMoveRight = FVector2D(CurrentX + 1, CurrentY + ForwardDirection);
    if (GameMode->IsTileOccupiedByOpponent(CaptureMoveLeft, PieceColor))
    {
        PossibleMoves.Add(CaptureMoveLeft);
    }
    if (GameMode->IsTileOccupiedByOpponent(CaptureMoveRight, PieceColor))
    {
        PossibleMoves.Add(CaptureMoveRight);
    }

    return PossibleMoves;
    */
    return PossibleMoves;
}
