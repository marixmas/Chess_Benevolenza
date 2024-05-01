// Fill out your copyright notice in the Description page of Project Settings.


#include "GameField.h"
#include "Chess_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Chess_Piece.h"				//	palese da cambiare??vvv fatto!?


// Sets default values
AGameField::AGameField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// è uno spreco di risorse tickare una scacchiera
	PrimaryActorTick.bCanEverTick = false;

	// size of the field (3x3)
	Size = 8;

	// tile dimension
	TileSize = 120;

	// tile padding dimension
	CellPadding = 0;

	TileArray.Empty();
	TileMap.Empty();
	PiecesArray.Empty();
	WhitePiecesArray.Empty();
	BlackPiecesArray.Empty();
	PiecesMap.Empty();
	//ReversePiecesMap.Empty();

}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tile padding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();

	//GenerateField();												// faccio da gamemode perché mi servirá  per clonare la gamefield nella funzione IsCheckmate di GameMode
	//GeneratePieces();												// meglio farlo da GameMode () per fare la AddDynamic Destroy dei Chess_Piece

}

void AGameField::GeneratePieces()
{

	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			if (x == 1)
			{
				SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::PAWN);
			}
			else if (x == 6)
			{
				SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::PAWN);
			}
			else if (x == 0)
			{
				if (y == 0 || y == 7)
				{
					SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::ROOK);
				}
				else if (y == 1 || y == 6)
				{
					SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::KNIGHT);
				}
				else if (y == 2 || y == 5)
				{
					SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::BISHOP);
				}
				else if (y == 3)
				{
					SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::QUEEN);
				}
				else if (y == 4)
				{
					SpawnChessPiece(x, y, EPieceColor::WHITE, EPieceType::KING);
				}
			}
			else if (x == 7)
			{
				if (y == 0 || y == 7)
				{
					SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::ROOK);
				}
				else if (y == 1 || y == 6)
				{
					SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::KNIGHT);
				}
				else if (y == 2 || y == 5)
				{
					SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::BISHOP);
				}
				else if (y == 3)
				{
					SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::QUEEN);
				}
				else if (y == 4)
				{
					SpawnChessPiece(x, y, EPieceColor::BLACK, EPieceType::KING);
				}
			}
			else
			{
				PiecesMap.Add(FVector2D(x, y), nullptr);
				//ReversePiecesMap.Add(nullptr, FVector2D(x, y));
			}
		}
	}
}

void AGameField::SpawnChessPiece(int32 x, int32 y, EPieceColor PieceColor, EPieceType PieceType)
{
	int32 PlayerOwner = (PieceColor == EPieceColor::WHITE) ? 0 : 1;

	FVector Location = (AGameField::GetRelativeLocationByXYPosition(x, y)) + FVector(0, 0, 20);

	const float TileScale = TileSize / 100;

	AChess_Piece* Obj = GetWorld()->SpawnActor<AChess_Piece>(GetChessPieceBP(PieceColor, PieceType), Location, FRotator::ZeroRotator);
	Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
	Obj->SetGridPosition(x, y);
	PiecesArray.Add(Obj);
	if (PlayerOwner == 0)
	{
		WhitePiecesArray.Add(Obj);
	}

	else if (PlayerOwner == 1)
	{
		BlackPiecesArray.Add(Obj);
	}
	PiecesMap.Add(FVector2D(x, y), Obj);
	//ReversePiecesMap.Add(Obj, FVector2D(x, y));								

	// ottengo il puntatore alla tile corrispondente
	ATile* TargetTile = TileMap.FindRef(FVector2D(x, y));
	// controllo il puntatore
	if (TargetTile)
	{
		// imposta stato e owner della tile
		TargetTile->SetTileStatus(ETileStatus::OCCUPIED);
		TargetTile->SetTileOwner(PlayerOwner);
	}
	else
	{
		// se il puntatore è nullo
		UE_LOG(LogTemp, Warning, TEXT("Il puntatore TargetTile è nullo! Impossibile impostare stato e owner della tile."));
	}

	// inizializzo il Pezzo Base (Chess_Piece) appena spawnato
	Obj->SetPieceColor(PieceColor);
	// imposto il tipo della pedina che occupa la tile
	Obj->SetPieceType(PieceType);


}

UClass* AGameField::GetChessPieceBP(EPieceColor PieceColor, EPieceType PieceType) 
{
	// Seleziona il blueprint corretto in base al colore e al tipo di pezzo
	switch (PieceType)
	{
	case EPieceType::PAWN:
		return (PieceColor == EPieceColor::WHITE) ? ChessPiecePawnBpWhite : ChessPiecePawnBpBlack;
	case EPieceType::ROOK:
		return (PieceColor == EPieceColor::WHITE) ? ChessPieceRookBpWhite : ChessPieceRookBpBlack;
	case EPieceType::KNIGHT:
		return (PieceColor == EPieceColor::WHITE) ? ChessPieceKnightBpWhite : ChessPieceKnightBpBlack;
	case EPieceType::BISHOP:
		return (PieceColor == EPieceColor::WHITE) ? ChessPieceBishopBpWhite : ChessPieceBishopBpBlack;
	case EPieceType::QUEEN:
		return (PieceColor == EPieceColor::WHITE) ? ChessPieceQueenBpWhite : ChessPieceQueenBpBlack;
	case EPieceType::KING:
		return (PieceColor == EPieceColor::WHITE) ? ChessPieceKingBpWhite : ChessPieceKingBpBlack;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Il puntatore al ChessPieceBP è nullo! Impossibile impostare il BP corretto al pezzo appena spawnato."));
		// Se il tipo di pezzo non è gestito, restituisci nullptr o un blueprint di default
		return nullptr;
	}
}


void AGameField::PromotionToQueen(AChess_Piece* PawnToPromote)
{
	// Controlla che il pedone da promuovere non sia già una regina
	if (PawnToPromote->GetPieceType() != EPieceType::PAWN)
	{
		UE_LOG(LogTemp, Warning, TEXT("Il pedone selezionato non è un pedone e non può essere promosso."));
	}

	// Ottieni la posizione del pedone da promuovere
	FVector2D PawnPosition = PawnToPromote->GetGridPosition();


	// Spawni una nuova regina al posto del pedone
	SpawnChessPiece(PawnPosition.X, PawnPosition.Y, PawnToPromote->GetPieceColor(), EPieceType::QUEEN);

	// Aggiorna le strutture dati
	// Rimuovi il pedone dalla lista dei pezzi
	PiecesArray.Remove(PawnToPromote);
	if (PawnToPromote->GetPieceColor() == EPieceColor::WHITE)
	{
		WhitePiecesArray.Remove(PawnToPromote);
	}
	else
	{
		BlackPiecesArray.Remove(PawnToPromote);
	}
	
	// Aggiorna le mappe dei pezzi
	PiecesMap.Remove(PawnPosition);
	//ReversePiecesMap.Remove(PawnToPromote);

	// Rimuovi il pedone dal campo di gioco
	PawnToPromote->Destroy();
}



void AGameField::ResetField()
{
	for (ATile* Obj : TileArray)
	{
		Obj->SetTileStatus(ETileStatus::EMPTY);
		Obj->SetTileOwner(NOT_ASSIGNED);
	}

	// send broadcast event to registered objects 
	OnResetEvent.Broadcast();

	 AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
		// TileArray non va svuotata dato che quando faccio reset dal bottone dell'interfaccia non cancello la GameField ma solo i Pieces
		// TileMap non va svuotata per la stessa ragione
		
		for (AChess_Piece* piece : GameMode->GetGField()->PiecesArray)
		{
			piece->Destroy();
		}

		// svuoto gli array con i puntatori ai pezzi poiché rigenero i pezzi
		PiecesArray.Empty();
		WhitePiecesArray.Empty();
		BlackPiecesArray.Empty();

		// svuoto la mappa con i puntatori ai pezzi poiché rigenero i pezzi
		PiecesMap.Empty();

		// svuoto la mappa con le posizioni ai pezzi poiché rigenero i pezzi
		//ReversePiecesMap.Empty();
		
        GameMode->IsGameOver = false;
        GameMode->MoveCounter = 0;

		GeneratePieces();

        GameMode->ChooseHumanPlayerAndStartGame();

    }
	else if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ptr a GameMode nullo in ResetField"));
	}
}


void AGameField::GenerateField()
{
	
		UE_LOG(LogTemp, Error, TEXT("è stata chiamata GenerateField"));
		for (int32 x = 0; x < Size; x++)
		{
			for (int32 y = 0; y < Size; y++)
			{
				FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
				if ((x + y) % 2 == 0)
				{
					ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);
					Obj->SetTileMaterial(TileMaterial1);
					const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					TileArray.Add(Obj);
					TileMap.Add(FVector2D(x, y), Obj);

				}
				else
				{
					ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);					
					Obj->SetTileMaterial(TileMaterial2);
					const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					TileArray.Add(Obj);
					TileMap.Add(FVector2D(x, y), Obj);
				}
			}
		}
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AGameField::GetTileArray()
{
	return TileArray;
}

TArray<AChess_Piece*>& AGameField::GetPiecesArray()
{
	return PiecesArray;
}

TArray<AChess_Piece*>& AGameField::GetWhitePiecesArray()
{
	return WhitePiecesArray;
}

TArray<AChess_Piece*>& AGameField::GetBlackPiecesArray()
{
	return BlackPiecesArray;
}

TArray<AChess_Piece*> AGameField::GetCopyOfPiecesArray()
{
	return PiecesArray;
}

TArray<AChess_Piece*> AGameField::GetCopyOfWhitePiecesArray()
{
	return WhitePiecesArray;
}

TArray<AChess_Piece*> AGameField::GetCopyOfBlackPiecesArray()
{
	return BlackPiecesArray;
}

FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
	return TileSize * NormalizedCellPadding * FVector(InX, InY, 0);
}

FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
	const double x = Location[0] / (TileSize * NormalizedCellPadding);
	const double y = Location[1] / (TileSize * NormalizedCellPadding);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("x=%f,y=%f"), x, y));
	return FVector2D(x, y);
}



/*
// Called every frame
void AGameField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
