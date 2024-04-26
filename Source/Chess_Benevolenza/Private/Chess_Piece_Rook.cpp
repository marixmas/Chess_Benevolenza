// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece_Rook.h"
#include "Chess_GameMode.h"

/*
EPieceType AChess_Piece_Rook::GetPieceType() const
{
    // Restituisce il tipo specifico del pezzo
    return EPieceType::ROOK;
}
*/

/*
void AChess_Piece_Rook::SpecialAction() const
{
    // Implementazione delle azioni specifiche per la torre
}
*/

TArray<FVector2D> AChess_Piece_Rook::CalculatePossibleMoves()
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


    // Ottieni la posizione attuale della torre
    FVector2D CurrentPosition = GetGridPosition();
   // int32 CurrentX = CurrentPosition.X;
   // int32 CurrentY = CurrentPosition.Y;


    // Definisci le direzioni possibili in cui la torre può muoversi: sopra, sotto, sinistra e destra
    const TArray<FVector2D> Directions = { FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1) };

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
            else
            {
                // Se la nuova posizione non è valida, interrompi il ciclo
                break;
            }
        }
    }

    return PossibleMoves;
}
/*
bool AChess_Piece_Rook::IsMoveValid(const FVector2D& Move)
{
    // Verifica se il movimento è all'interno della scacchiera
    if (Move.X < 0 || Move.X >= 8 || Move.Y < 0 || Move.Y >= 8)
    {
        return false;
    }

    return true;
}

*/