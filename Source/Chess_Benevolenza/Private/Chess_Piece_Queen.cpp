// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Queen.h"
#include "Chess_GameMode.h"

TArray<FVector2D> AChess_Piece_Queen::CalculatePossibleMoves()
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

    // Ottieni la posizione attuale della regina
    FVector2D CurrentPosition = GetGridPosition();

    // Definisci le direzioni possibili in cui la regina può muoversi: sopra, sotto, sinistra, destra e diagonali
    const TArray<FVector2D> Directions = {
        FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1),  // Orizzontali e Verticali
        FVector2D(1, 1), FVector2D(1, -1), FVector2D(-1, 1), FVector2D(-1, -1) // Diagonali
    };

    // Per ogni direzione, controlla le possibili mosse fino a quando si raggiunge il bordo della scacchiera o si incontra un'altra pedina
    for (const FVector2D& Direction : Directions)
    {
        for (int32 Step = 1; Step < GField->Size; ++Step)
        {
            // Calcola la nuova posizione in base alla direzione e al passo
            FVector2D NewPosition = CurrentPosition + (Direction * Step);

            // Verifica se la nuova posizione è valida sulla scacchiera
            if (!IsMoveValid(NewPosition))
            {
                break;
            }

            // Ottieni il pezzo sulla nuova posizione (se presente)
            AChess_Piece* PieceAtPosition = GField->PiecesMap.FindRef(NewPosition);

            // Se non c'è un pezzo sulla nuova posizione, aggiungi la mossa alla lista delle possibili mosse
            if (!PieceAtPosition)
            {
                PossibleMoves.Add(NewPosition);
            }
            // Se c'è un pezzo sulla nuova posizione, verifica se è un pezzo avversario e se sì, aggiungi la mossa e interrompi il ciclo
            else if (PieceAtPosition->GetPieceColor() != GetPieceColor())
            {
                PossibleMoves.Add(NewPosition);
                break;
            }
            // Se c'è un pezzo sulla nuova posizione, ma è un pezzo amico, interrompi il ciclo
            else
            {
                break;
            }
        }
    }

    return PossibleMoves;
}


/*
bool AChess_Piece_Queen::IsMoveValid(const FVector2D& Move)                         // uso quella della classe base
{
    return false;
}
*/