// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Bishop.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_Bishop::CalculatePossibleMoves()
{
    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode non valido per calcolare le mosse possibili del vescovo"));
        return TArray<FVector2D>();
    }

    AGameField* GField = GameMode->GetGField();
    if (!GField)
    {
        UE_LOG(LogTemp, Error, TEXT("GField non valido per calcolare le mosse possibili del vescovo"));
        return TArray<FVector2D>();
    }

    PossibleMoves.Empty();
    FVector2D CurrentPosition = GetGridPosition();

    // Definisci le direzioni diagonali in cui il vescovo può muoversi
    const TArray<FVector2D> Directions = { FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1) };

    // Per ogni direzione, controlla le possibili mosse fino a quando si raggiunge il bordo della scacchiera o si incontra un'altra pedina
    for (const FVector2D& Direction : Directions)
    {
        for (int32 Step = 1; Step < GField->Size; ++Step)
        {
            // Calcola la nuova posizione in base alla direzione e al passo
            FVector2D NewPosition = CurrentPosition + (Direction * Step);

            // Verifica se la nuova posizione è valida sulla scacchiera
            if (IsMoveValid(NewPosition))
            {
                AChess_Piece* PieceAtPosition = GField->PiecesMap.FindRef(NewPosition);
                if (!PieceAtPosition)
                {
                    PossibleMoves.Add(NewPosition);
                }
                else if (PieceAtPosition->GetPieceColor() != GetPieceColor())
                {
                    PossibleMoves.Add(NewPosition);
                    break;
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    return PossibleMoves;
}

/*
bool AChess_Piece_Bishop::IsMoveValid(const FVector2D& Move)
{
    return false;
}
*/
