// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece.h"
#include "GameField.h"
#include "Tile.h"
#include "Chess_GameMode.h"

// Sets default values
AChess_Piece::AChess_Piece()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//metto false
	PrimaryActorTick.bCanEverTick = false;

}

void AChess_Piece::SetGridPosition(const int32 InX, const int32 InY)
{
	PieceGridPosition.Set(InX, InY);

}
FVector2D AChess_Piece::GetGridPosition()							// avevo messo come argomento AChess_Piece* Piece ma non credo sia necessario basta fare puntatoreAoggettoPiece->GetGridPosition()
{
	return PieceGridPosition;
}
/*
void AChess_Piece::SetTileStatus(ETileStatus TileStatus)
{
	ATile->GetTileStatus();
	ATile->Status = TileStatus;
}
*/

void AChess_Piece::SetPieceType(EPieceType NewPieceType)
{
	PieceType = NewPieceType;
}

void AChess_Piece::SetPieceColor(EPieceColor NewPieceColor)
{
	PieceColor = NewPieceColor;
}

EPieceColor AChess_Piece::GetPieceColor()
{
	return PieceColor;
}

EPieceType AChess_Piece::GetPieceType() const
{
	return PieceType;
}

void AChess_Piece::MovePieceFromToPosition(const FVector2D& OldPosition, const FVector2D& NewPosition)
{
	SetGridPosition(NewPosition.X, NewPosition.Y);


	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare IsAttackValid"));
		return;
	}
	AGameField* GField = GameMode->GetGField();

	if (!GField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField non valido per fare IsAttackValid")); 
		return;
	}
	
	// svuoto la tile della OldPosition
	ATile* OldPositionTile = GField->TileMap[OldPosition];

	OldPositionTile->EmptyTile();

	if (GField->PiecesMap.Contains(OldPosition))
	{
		// Se la vecchia posizione � gi� presente nella mappa, aggiorna il suo valore
		GField->PiecesMap[OldPosition] = nullptr;
	}
	else
	{
		// Se la vecchia posizione non � presente nella mappa impossibile
		//GField->PiecesMap.Add(OldPosition, nullptr);
		UE_LOG(LogTemp, Error, TEXT("OldPosition non esisteva nella PiecesMap, in MovePieceFromToPosition"));
	}

	
	// aggiorno PiecesMap 
	//GField->PiecesMap[FVector2D(NewPosition.X, NewPosition.Y)] = this;				 non funziona bc se nessun pezoz ] mai stato li c'e un puntatore nullo

	if (GField->PiecesMap.Contains(NewPosition))
	{
		// Se la nuova posizione � gi� presente nella mappa, aggiorna il suo valore
		GField->PiecesMap[NewPosition] = this;
	}
	else
	{
		// Se la nuova posizione non � presente nella mappa, aggiungi una nuova voce
		GField->PiecesMap.Add(NewPosition, this);
	}

	
	// aggiorno ReversePiecesMap 
	GField->ReversePiecesMap[this] = FVector2D(NewPosition.X, NewPosition.Y);

	FVector NewLocation = FVector(GField->AGameField::GetRelativeLocationByXYPosition(NewPosition.X, NewPosition.Y) + FVector(0, 0, 20));
	SetActorLocation(NewLocation);


	


}


// Called when the game starts or when spawned
void AChess_Piece::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	//GameMode->GField->OnResetEvent.AddDynamic(this, &AChess_Piece::SelfDestroy);				//ho dovuto bc non mi andava la AddDynamic
}

// Called every frame
void AChess_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//TArray<FVector2D> AChess_Piece::CalculatePossibleMoves()
TArray<FVector2D> AChess_Piece::CalculatePossibleMoves()

{
	// Restituisce un array vuoto di default
	return TArray<FVector2D>();
}

bool AChess_Piece::IsMoveValid(const FVector2D& Move)
{
	// Verifica se il movimento � all'interno della scacchiera
	if (Move.X < 0 || Move.X >= 8 || Move.Y < 0 || Move.Y >= 8)
	{
		return false;
	}
	return true;
}

bool AChess_Piece::IsAttackValid(const FVector2D& Attack)
{
	return true;
}

void  AChess_Piece::SelfDestroy()
{
	Destroy();
}

