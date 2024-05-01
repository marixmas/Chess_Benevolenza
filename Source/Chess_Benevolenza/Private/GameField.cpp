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

/*
AGameField* AGameField::CloneGameField()
{
	AGameField* ClonedNewGameField = GetWorld()->SpawnActor<AGameField>(AGameField::StaticClass());
	if (ClonedNewGameField)
	{
		// Copia l'array delle tessere
		for (ATile* Tile : TileArray)
		{
			// Copia la tessera e aggiungila alla nuova scacchiera
			ATile* NewTile = Tile->CloneTile();
			ClonedNewGameField->TileArray.Add(NewTile);
			ClonedNewGameField->TileMap.Add(NewTile->GetGridPosition(), NewTile);
		}

		// Copia l'array dei pezzi
		TArray<AChess_Piece*> CopiedPiecesArray = ClonePiecesArray();
		ClonedNewGameField->PiecesArray = CopiedPiecesArray;
		// Aggiungi i pezzi alla mappa dei pezzi
		for (AChess_Piece* Piece : CopiedPiecesArray)
		{
			ClonedNewGameField->PiecesMap.Add(Piece->GetGridPosition(), Piece);
		}

		// copia gli altri due array di pezzi secondo il colore
		TArray<AChess_Piece*> ClonedWhitePieces;
		TArray<AChess_Piece*> ClonedBlackPieces;

		// scorre il primo array WhitePiecesArray
		for (AChess_Piece* OriginalPiece : WhitePiecesArray)
		{
			// Clone the piece using the assumed ClonePiece function
			AChess_Piece* ClonedPiece = OriginalPiece->ClonePiece();
			if (ClonedPiece)
			{
				ClonedWhitePieces.Add(ClonedPiece);
			}
		}

		// scorre il secondo array BlackPiecesArray
		for (AChess_Piece* OriginalPiece : BlackPiecesArray)
		{
			// clona il  pezzo 
			AChess_Piece* ClonedPiece = OriginalPiece->ClonePiece();
			if (ClonedPiece)
			{
				ClonedBlackPieces.Add(ClonedPiece);
			}
		}

		// assegna gli array clonati dei pezzi alla nuova gamefield clonata
		ClonedNewGameField->WhitePiecesArray = ClonedWhitePieces;
		ClonedNewGameField->BlackPiecesArray = ClonedBlackPieces;

	}

	return ClonedNewGameField;

}
*/

/*
AGameField* AGameField::CloneGameField()
{
	AGameField* ClonedNewGameField = CloneEmptyGameField();

	if (ClonedNewGameField)
	{
		// Copia l'array dei pezzi del giocatore bianco
		TArray<AChess_Piece*> ClonedWhitePiecesArray = ClonePiecesArray(EPieceColor::WHITE);
		ClonedNewGameField->WhitePiecesArray = ClonedWhitePiecesArray;
		// Aggiungi i pezzi bianchi alla mappa dei pezzi del campo di gioco clonato
		for (AChess_Piece* Piece : ClonedWhitePiecesArray)
		{
			ClonedNewGameField->PiecesMap.Add(Piece->GetGridPosition(), Piece);
		}

		// Copia l'array dei pezzi del giocatore nero (modifica in base al tuo codice)
		TArray<AChess_Piece*> ClonedBlackPiecesArray = ClonePiecesArray(EPieceColor::BLACK);
		ClonedNewGameField->BlackPiecesArray = ClonedBlackPiecesArray;
		// Aggiungi i pezzi neri alla mappa dei pezzi del campo di gioco clonato
		for (AChess_Piece* Piece : ClonedBlackPiecesArray)
		{
			ClonedNewGameField->PiecesMap.Add(Piece->GetGridPosition(), Piece);
		}
	}
	return ClonedNewGameField;
}
*/

/*
TArray<AChess_Piece*> AGameField::ClonePiecesArray(EPieceColor Color = EPieceColor::NONE)
{
	TArray<AChess_Piece*> CopiedPiecesArray;
	for (AChess_Piece* Piece : PiecesArray)
	{
		if (Color == EPieceColor::NONE || Piece->GetPieceColor() == Color)
		{
			// Crea una copia del pezzo e aggiungilo all'array delle copie
			AChess_Piece* CopiedPiece = Piece->ClonePiece();
			CopiedPiecesArray.Add(CopiedPiece);
		}
	}
	return CopiedPiecesArray;
}

*/



/*
AGameField* AGameField::CloneEmptyGameField()							 														/// HA SENSO
{
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());

	AGameField* ClonedNewGameField = GetWorld()->SpawnActor<AGameField>(GameMode->GameFieldClass);
	if (ClonedNewGameField)
		{
			// Copia l'array delle tessere
			for (ATile* Tile : TileArray)
			{
				// Copia la tessera e aggiungila alla nuova scacchiera
				ATile* NewTile = Tile->CloneTile();
				ClonedNewGameField->TileArray.Add(NewTile);
				ClonedNewGameField->TileMap.Add(NewTile->GetGridPosition(), NewTile);
			}
		}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Il puntatore ClonedNewGameField è nullo!"));
	}
	
	return ClonedNewGameField;
	
	
}
*/

/*
void AGameField::CloneAllPiecesToField(AGameField* TargetField)																// MESSA PER ULTIMA, HA SENSO
{
	/*
	// pulisco il clone 
	for (AChess_Piece* OldClonePiece : TargetField->PiecesArray)
	{
		OldClonePiece->Destroy();
	}

	TargetField->WhitePiecesArray.Empty();
	TargetField->BlackPiecesArray.Empty();
	TargetField->PiecesArray.Empty();
	TargetField->PiecesMap.Empty();
	TargetField->ReversePiecesMap.Empty();

	*/

/*
// Inizializza le mappe per tutte le caselle
	for (int32 x = 0; x < Size; x++)
	{
		for (int32 y = 0; y < Size; y++)
		{
			FVector2D GridPosition(x, y);
			TargetField->PiecesMap.Add(GridPosition, nullptr);
			//TargetField->ReversePiecesMap.Add(nullptr, GridPosition);							 mi sa di nooo

		}
	}

	// Clona tutti i pezzi bianchi
	for (AChess_Piece* OriginalPiece : WhitePiecesArray)
	{
		AChess_Piece* ClonedPiece = ClonePiece(OriginalPiece);
		if (ClonedPiece)
		{
			TargetField->WhitePiecesArray.Add(ClonedPiece);
			TargetField->PiecesArray.Add(ClonedPiece);
			TargetField->PiecesMap[ClonedPiece->GetGridPosition()] = ClonedPiece;
			//TargetField->ReversePiecesMap[ClonedPiece] = ClonedPiece->GetGridPosition();						// mi sa di noo    		
			///////TargetField->ReversePiecesMap.Add(ClonedPiece) = ClonedPiece->GetGridPosition();										voglio togliere da ovunque reversepiecemap
		}
	}

	// Clona tutti i pezzi neri
	for (AChess_Piece* OriginalPiece : BlackPiecesArray)
	{
		AChess_Piece* ClonedPiece = ClonePiece(OriginalPiece);
		if (ClonedPiece)
		{
			TargetField->BlackPiecesArray.Add(ClonedPiece);
			TargetField->PiecesArray.Add(ClonedPiece);
			TargetField->PiecesMap[ClonedPiece->GetGridPosition()] = ClonedPiece;
			//TargetField->ReversePiecesMap[ClonedPiece] = ClonedPiece->GetGridPosition();	
			/////////TargetField->ReversePiecesMap.Add(ClonedPiece) = ClonedPiece->GetGridPosition();

		}
	}

	

}
*/
	


/*

TArray<AChess_Piece*> AGameField::ClonePiecesArray()																//CREDO SIA DA TOGLIERE
{
	TArray<AChess_Piece*> CopiedPiecesArray;
	for (AChess_Piece* Piece : PiecesArray)
	{
		// Crea una copia del pezzo e lo aggiunge all'array delle copie
		AChess_Piece* CopiedPiece = Piece->ClonePiece();
		CopiedPiecesArray.Add(CopiedPiece);
	}
	return CopiedPiecesArray;
}
*/




/*
AChess_Piece* AGameField::ClonePiece(AChess_Piece* PieceToClone)
{

	FVector Location = (GetRelativeLocationByXYPosition (PieceToClone->GetGridPosition().X, PieceToClone->GetGridPosition().Y) ) + FVector(0, 0, -40);   


	AChess_Piece* CopiedPiece = GetWorld()->SpawnActor<AChess_Piece>(GetChessPieceBP(PieceToClone->GetPieceColor(), PieceToClone->GetPieceType()), Location, FRotator::ZeroRotator);
	if (CopiedPiece)
	{
		// Copia i dati del pezzo originale nella copia
		CopiedPiece->SetGridPosition(PieceToClone->GetGridPosition().X, PieceToClone->GetGridPosition().Y);
		CopiedPiece->SetPieceType(PieceToClone->GetPieceType()); 
		CopiedPiece->SetPieceColor(PieceToClone->GetPieceColor()); 
	}
	return CopiedPiece;
}

*/





/*
//
//
//
//
//
*/


/*
bool AGameField::SimulateMoveAndCheck(AChess_Piece* PieceToSimulate, FVector2D PositionToMoveTo)
{
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AGameField* GField = GameMode->GetGField();


	// clono scacchiera e pezzi
	AGameField* ClonedGameField = GField->CloneEmptyGameField();
	GField->CloneAllPiecesToField(ClonedGameField);

	// risalgo dalla pedina dell'originale alla pedina del clone
	AChess_Piece* ClonedPieceToSimulate = ClonedGameField->PiecesMap.FindRef(PieceToSimulate->GetGridPosition()); 
	

	// faccio la mossa sul clone
	ClonedPieceToSimulate->MovePieceFromToPosition(ClonedPieceToSimulate->GetGridPosition(), PositionToMoveTo);

	// se nella position finale c'é una pedina la mangio  (non controllo se sia del colore giusto perché le mosse passate qui dentro sono giá state controllate prima altrove)
	if (ClonedGameField->PiecesMap.FindRef(PositionToMoveTo))
	{
		(ClonedGameField->PiecesMap.FindRef(PositionToMoveTo))->PieceIsEaten(PositionToMoveTo);
	}
	else {}

	// é scacco?
	if (PieceToSimulate->GetPieceColor() == EPieceColor::WHITE)
	{
		if (GameMode->IsKingInCheck(ClonedGameField, 0))
		{
			// é scacco

			for (AChess_Piece* piece : ClonedGameField->PiecesArray)
			{
				piece->Destroy();
			}

			for (ATile* tile : ClonedGameField->TileArray)
			{
				tile->Destroy();
			}

			ClonedGameField->Destroy();


			return true;
		}
		else
		{
			// non è scacco

			for (AChess_Piece* piece : ClonedGameField->PiecesArray)
			{
				piece->Destroy();
			}

			for (ATile* tile : ClonedGameField->TileArray)
			{
				tile->Destroy();
			}

			ClonedGameField->Destroy();

			return false;
		}

	}

	else if (PieceToSimulate->GetPieceColor() == EPieceColor::BLACK)
	{
		if (GameMode->IsKingInCheck(ClonedGameField, 1))
		{
			// é scacco

			for (AChess_Piece* piece : ClonedGameField->PiecesArray)
			{
				piece->Destroy();
			}

			for (ATile* tile : ClonedGameField->TileArray)
			{
				tile->Destroy();
			}

			ClonedGameField->Destroy();

			return true;
		}
		else
		{
			// non è scacco

			for (AChess_Piece* piece : ClonedGameField->PiecesArray)
			{
				piece->Destroy();
			}

			for (ATile* tile : ClonedGameField->TileArray)
			{
				tile->Destroy();
			}

			ClonedGameField->Destroy();

			return false;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SimulateMoveAndCheck su un pezzo non Bianco e non Nero"));
		
		return false;
	}
}




*/



/*
//
//
//
//
//
*/



















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
