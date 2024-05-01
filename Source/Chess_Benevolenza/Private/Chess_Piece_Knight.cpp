// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Knight.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_Knight::CalculatePossibleMoves()
{
    AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare IsAttackValid"));
        return TArray<FVector2D>();
    }

    AGameField* GField = GameMode->GetGField();
    if (!GField)
    {
        UE_LOG(LogTemp, Error, TEXT("GField non valido per fare IsAttackValid"));
        return TArray<FVector2D>();
    }

    PossibleMoves.Empty();

    // Ottieni la posizione attuale del cavallo
    FVector2D CurrentPosition = GetGridPosition();

    // Definisci gli spostamenti possibili del cavallo
    const TArray<FVector2D> KnightMoves = {
        FVector2D(1, 2), FVector2D(-1, 2), FVector2D(1, -2), FVector2D(-1, -2),
        FVector2D(2, 1), FVector2D(-2, 1), FVector2D(2, -1), FVector2D(-2, -1)
    };

    // Per ogni spostamento possibile, verifica se è valido e se lo è, aggiungilo alla lista delle mosse possibili
    for (const FVector2D& Move : KnightMoves)
    {
        FVector2D NewPosition = CurrentPosition + Move;

        // Verifica se la nuova posizione è valida sulla scacchiera
        if (IsMoveValid(NewPosition))
        {
            // Ottieni il pezzo sulla nuova posizione (se presente)
            AChess_Piece* PieceAtPosition = GField->PiecesMap.FindRef(NewPosition);

            // Se non c'è un pezzo sulla nuova posizione o se c'è un pezzo avversario, aggiungi la mossa alla lista delle possibili mosse
            if (!PieceAtPosition || PieceAtPosition->GetPieceColor() != GetPieceColor() )
            {
                PossibleMoves.Add(NewPosition);
            }
        }
    }

    return PossibleMoves;
}

/*
bool AChess_Piece_Knight::IsMoveValid(const FVector2D& Move)                // uso quella della classe base
{
    return false;
}
*/

