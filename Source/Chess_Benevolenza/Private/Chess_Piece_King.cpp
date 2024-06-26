// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_King.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_King::CalculatePossibleMoves()
{
    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare CalculatePossibleMoves"));
        return TArray<FVector2D>();
    }

    AGameField* GField = GameMode->GetGField();
    if (!GField)
    {
        UE_LOG(LogTemp, Error, TEXT("GField non valido per fare CalculatePossibleMoves"));
        return TArray<FVector2D>();
    }

    PossibleMoves.Empty();

    // Ottieni la posizione attuale del re
    FVector2D CurrentPosition = GetGridPosition();

    // Definisci le direzioni possibili in cui il re pu� muoversi
    const TArray<FVector2D> Directions = {
        FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1),
        FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1)
    };

    // Per ogni direzione, controlla le possibili mosse
    for (const FVector2D& Direction : Directions)
    {
        // Calcola la nuova posizione
        FVector2D NewPosition = CurrentPosition + Direction;

        // Se la mossa � valida, aggiungi la posizione ai possibili movimenti
        if (IsMoveValid(NewPosition))
        {
            PossibleMoves.Add(NewPosition);
        }
    }

    return PossibleMoves;
}

bool AChess_Piece_King::IsMoveValid(const FVector2D& Move)
{
    // Verifica se il movimento � all'interno della scacchiera
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

            if (!DestinationTile)
            {
                return false;
            }
            // tile vuota
            else if (DestinationTile->GetOwner() == -1)
            {
                return true;
            }
            // non posso mangiare un pezzo dello stesso colore
            else if (DestinationTile->GetOwner() == 0 && PieceColor == EPieceColor::WHITE)
            {
                return false;
            }
            else if (DestinationTile->GetOwner() == 1 && PieceColor == EPieceColor::BLACK)
            {
                return false;
            }
            else
            { 
                return false;
            }
           
        }
    }

    return true;
}


