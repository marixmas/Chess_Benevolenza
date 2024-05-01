// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Pawn.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_Pawn::CalculatePossibleMoves()
{
    PossibleMoves.Empty();

    // Ottieni la posizione corrente del pedone
    FVector2D CurrentPosition = GetGridPosition();
    int32 CurrentX = CurrentPosition.X;
    int32 CurrentY = CurrentPosition.Y;

    // Direzione del movimento del pedone basata sul colore
    int32 Direction = (PieceColor == EPieceColor::WHITE) ? 1 : -1;

    // Mossa di avanzamento di una casella
    FVector2D ForwardMove = FVector2D(CurrentX + Direction, CurrentY);
    if (IsMoveValid(ForwardMove))
    {
        PossibleMoves.Add(ForwardMove);
    }

    // Prima mossa speciale di avanzamento di due caselle per il pedone
    if ((PieceColor == EPieceColor::WHITE && CurrentX == 1) || (PieceColor == EPieceColor::BLACK && CurrentX == 6))
    {
        FVector2D DoubleForwardMove = FVector2D(CurrentX + 2 * Direction, CurrentY);
        if (IsMoveValid(DoubleForwardMove))
        {
            AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
            AGameField* GField = GameMode->GetGField();
            if (GameMode && GField)
            {
                ATile* TileBeforeDestinationTile = GField->TileMap.FindRef(FVector2D(CurrentX + 1 * Direction, CurrentY));
                if (!TileBeforeDestinationTile->IsOccupied())
                {
                    PossibleMoves.Add(DoubleForwardMove);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare CalculatePossibleMoves() di Pawn"));
            }
        }
    }

    // Mossa di attacco diagonale destro
    FVector2D DiagonalRightMove = FVector2D(CurrentX + Direction, CurrentY + 1);
    if (IsAttackValid(DiagonalRightMove))
    {
        PossibleMoves.Add(DiagonalRightMove);
    }

    // Mossa di attacco diagonale sinistro
    FVector2D DiagonalLeftMove = FVector2D(CurrentX + Direction, CurrentY - 1);
    if (IsAttackValid(DiagonalLeftMove))
    {
        PossibleMoves.Add(DiagonalLeftMove);
    }

    return PossibleMoves;
}

bool AChess_Piece_Pawn::IsMoveValid(const FVector2D& Move) 
{
    // Verifica se il movimento è all'interno della scacchiera
    if (Move.X < 0 || Move.X >= 8 || Move.Y < 0 || Move.Y >= 8)
    {
        return false;
    }

    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        AGameField* GField = GameMode->GetGField();
        if (GField)
        {
            ATile* DestinationTile = GField->TileMap.FindRef(FVector2D(Move.X, Move.Y));

            if (!DestinationTile || DestinationTile->IsOccupied())
            {
                return false;
            }
        }
    }
    return true;
}

bool AChess_Piece_Pawn::IsAttackValid(const FVector2D& Attack) 
{
    // Verifica se l'attacco è all'interno della scacchiera
    if (Attack.X < 0 || Attack.X >= 8 || Attack.Y < 0 || Attack.Y >= 8)
    {
        return false;
    }

    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare IsAttackValid di Pawn"));
        return false;
    }

    AGameField* GField = GameMode->GetGField();

    if (!GField)
    {
        UE_LOG(LogTemp, Error, TEXT("GField non valido per fare IsAttackValid di Pawn"));
        return false;
    }

    ATile* DestinationTile = GField->TileMap.FindRef(FVector2D(Attack.X, Attack.Y));

    if (!DestinationTile)
    {
        UE_LOG(LogTemp, Error, TEXT("puntatore a DestinationTile non valido per fare IsAttackValid di Pawn"));
        return false;
    }

    if (DestinationTile->IsOccupied())
    {
        if ((PieceColor == EPieceColor::WHITE && DestinationTile->GetOwner() == 0) || (PieceColor == EPieceColor::BLACK && DestinationTile->GetOwner() == 1))
        {
            return false;
        }

        else if ((PieceColor == EPieceColor::WHITE && DestinationTile->GetOwner() == 1) || (PieceColor == EPieceColor::BLACK && DestinationTile->GetOwner() == 0))
        {
            return true;
        }
    }
    return false;
    
 }

    

