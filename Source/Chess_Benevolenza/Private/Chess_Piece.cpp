// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess_Piece.h"
#include "GameField.h"
#include "Tile.h"
#include "Chess_GameMode.h"

// Sets default values
AChess_Piece::AChess_Piece()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// metto false perché non mi serve
	PrimaryActorTick.bCanEverTick = false;
	PieceColor = EPieceColor::NOCOLOR;
	PieceType = EPieceType::NOTYPE;

}

// Called when the game starts or when spawned
void AChess_Piece::BeginPlay()
{
	Super::BeginPlay();
	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	GameMode->GField->OnResetEvent.AddDynamic(this, &AChess_Piece::SelfDestroy);
}

// Called every frame
void AChess_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AChess_Piece::SetGridPosition(const int32 InX, const int32 InY)
{
	PieceGridPosition.Set(InX, InY);

}
FVector2D AChess_Piece::GetGridPosition()							// avevo messo come argomento AChess_Piece* Piece ma non credo sia necessario basta fare puntatoreAoggettoPiece->GetGridPosition()
{
	return PieceGridPosition;
}


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

void AChess_Piece::MovePieceFromToPosition(const FVector2D OldPosition, const FVector2D NewPosition)
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

	// riempio la tile della newPosition
	ATile* NewPositionTile = GField->TileMap[NewPosition];
	NewPositionTile->SetTileStatus(ETileStatus::OCCUPIED);
	if(GetPieceColor() == EPieceColor::WHITE)
	{
		NewPositionTile->SetTileOwner(0);
	}
	if (GetPieceColor() == EPieceColor::BLACK)
	{
		NewPositionTile->SetTileOwner(1);
	}


	if (GField->PiecesMap.Contains(OldPosition))
	{
		// Se la vecchia posizione è già presente nella mappa, aggiorna il suo valore
		GField->PiecesMap[OldPosition] = nullptr;
	}
	else
	{
		// Se la vecchia posizione non è presente nella mappa impossibile teoricamente
		//GField->PiecesMap.Add(OldPosition, nullptr);
		UE_LOG(LogTemp, Error, TEXT("OldPosition non esisteva nella PiecesMap, in MovePieceFromToPosition"));
	}

	if (GField->PiecesMap.Contains(NewPosition))
	{
		// Se la nuova posizione è già presente nella mappa, aggiorna il suo valore
		GField->PiecesMap[NewPosition] = this;
	}
	else
	{
		// Se la nuova posizione non è presente nella mappa, aggiungi una nuova voce
		GField->PiecesMap.Add(NewPosition, this);
	}

	
	// aggiorno ReversePiecesMap 
	//GField->ReversePiecesMap[this] = FVector2D(NewPosition.X, NewPosition.Y);

	// sposto il blueprint del Piece
	FVector NewLocation = FVector(GField->AGameField::GetRelativeLocationByXYPosition(NewPosition.X, NewPosition.Y) + FVector(0, 0, 20));
	SetActorLocation(NewLocation);


}



void AChess_Piece::MoveClonedPieceFromToPosition(/*AGameField* CloneOfGameField, */ FVector2D OldPosition, FVector2D NewPosition)					///aaaaaaaaaaaaaaaaaaaAAAAAAAAAAAAAAAAAAAAAAAA
{
	SetGridPosition(NewPosition.X, NewPosition.Y);
	/*
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
	*/


	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare IsAttackValid"));
		return;
	}
	AGameField* ClonedGameField = GameMode->GetClonedGameField();

	if (!ClonedGameField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField non valido per fare IsAttackValid"));
		return;
	}

	// svuoto la tile della OldPosition
	ATile* OldPositionTile = ClonedGameField->TileMap[OldPosition];
	OldPositionTile->EmptyTile();

	// riempio la tile della newPosition
	ATile* NewPositionTile = ClonedGameField->TileMap[NewPosition];
	NewPositionTile->SetTileStatus(ETileStatus::OCCUPIED);
	if (GetPieceColor() == EPieceColor::WHITE)
	{
		NewPositionTile->SetTileOwner(0);
	}
	if (GetPieceColor() == EPieceColor::BLACK)
	{
		NewPositionTile->SetTileOwner(1);
	}


	if (ClonedGameField->PiecesMap.Contains(OldPosition))
	{
		// Se la vecchia posizione è già presente nella mappa, aggiorna il suo valore (dovrebbe essere contenuta)
		ClonedGameField->PiecesMap[OldPosition] = nullptr;
	}
	else
	{
		// Se la vecchia posizione non è presente nella mappa (impossibile teoricamente, perché sono state tutte inizializzate)
		ClonedGameField->PiecesMap.Add(OldPosition, nullptr);
		UE_LOG(LogTemp, Error, TEXT("OldPosition non esisteva nella PiecesMap, in MoveClonedPieceFromToPosition"));
	}

	if (ClonedGameField->PiecesMap.Contains(NewPosition))
	{
		// Se la nuova posizione è già presente nella mappa, aggiorna il suo valore
		ClonedGameField->PiecesMap[NewPosition] = this;
	}
	else
	{
		// Se la nuova posizione non è presente nella mappa, aggiungi una nuova voce (dovrebbe essere persente poiché tutte inizializzate)
		ClonedGameField->PiecesMap.Add(NewPosition, this);
	}


	// aggiorno ReversePiecesMap 
	//ClonedGameField->ReversePiecesMap[this] = FVector2D(NewPosition.X, NewPosition.Y);

	// sposto il blueprint del Piece
	FVector NewLocation = FVector(ClonedGameField->AGameField::GetRelativeLocationByXYPosition(NewPosition.X, NewPosition.Y) + FVector(0, 0, 20));
	this->SetActorLocation(NewLocation);

}


void AChess_Piece::ClonedPieceIsEaten(/*AGameField* CloneOfGameField,*/ FVector2D& EatenPiecePosition)
{
	/*
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
	*/

	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode non valido per fare IsAttackValid"));
		return;
	}
	AGameField* ClonedGameField = GameMode->GetClonedGameField();

	if (!ClonedGameField)
	{
		UE_LOG(LogTemp, Error, TEXT("GField non valido per fare IsAttackValid"));
		return;
	}

	ATile* Tile = ClonedGameField->TileMap.FindRef(EatenPiecePosition);
	Tile->EmptyTile();

	// rimuovi la pedina dalla mappa della scacchiera
	ClonedGameField->PiecesMap[EatenPiecePosition] = nullptr;

	// rimuovi la pedina dagli array delle pedine
	ClonedGameField->PiecesArray.Remove(this);
	EPieceColor EatenPieceColor = this->GetPieceColor();
	if (EatenPieceColor == EPieceColor::WHITE)
	{
		ClonedGameField->WhitePiecesArray.Remove(this);
	}
	else if (EatenPieceColor == EPieceColor::BLACK)
	{
		ClonedGameField->BlackPiecesArray.Remove(this);
	}

	// rimuovi dalla mappa puntatore-posizione sulla griglia
	//ClonedGameField->ReversePiecesMap.Remove(this);

	// distruggo
	SelfDestroy();
}


/*											NON VA QUI VA NEL GAMEFIELD
AChess_Piece* AChess_Piece::ClonePiece()
{

	FVector Location = (AGameField::GetRelativeLocationByXYPosition(x, y)) + FVector(0, 0, 20);


	AChess_Piece* CopiedPiece = GetWorld()->SpawnActor<AChess_Piece>(AGameField::GetChessPieceBP(PieceColor, PieceType), Location, FRotator::ZeroRotator);
	if (CopiedPiece)
	{
		// Copia i dati del pezzo originale nella copia
		CopiedPiece->SetGridPosition(PieceGridPosition.X, PieceGridPosition.Y);
		CopiedPiece->SetPieceType(PieceType);
		CopiedPiece->SetPieceColor(PieceColor);
	}
	return CopiedPiece;
}
*/


TArray<FVector2D> AChess_Piece::CalculatePossibleMoves()

{
	// Restituisce un array vuoto di default
	return TArray<FVector2D>();
}

bool AChess_Piece::IsMoveValid(const FVector2D& Move)
{
	// Verifica se il movimento è all'interno della scacchiera
	if (Move.X < 0 || Move.X >= 8 || Move.Y < 0 || Move.Y >= 8)
	{
		return false;
	}
	return true;
}


void AChess_Piece::PieceIsEaten(FVector2D& EatenPiecePosition)                             
{

	AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
	AGameField* GField = GameMode->GetGField();
	if (GameMode && GField)
	{
		ATile* Tile = GField->TileMap.FindRef(EatenPiecePosition);
		Tile->EmptyTile();

		// rimuovi la pedina dalla mappa della scacchiera
		GField->PiecesMap[EatenPiecePosition] = nullptr;

		// rimuovi la pedina dagli array delle pedine
		GField->PiecesArray.Remove(this); 
		EPieceColor EatenPieceColor = this->GetPieceColor();
		if (EatenPieceColor == EPieceColor::WHITE)
		{
			GField->WhitePiecesArray.Remove(this);
		}
		else if (EatenPieceColor == EPieceColor::BLACK)
		{
			GField->BlackPiecesArray.Remove(this);
		}

		// rimuovi dalla mappa puntatore-posizione sulla griglia
		//GField->ReversePiecesMap.Remove(this);

		SelfDestroy();
	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GameMode o GField non valido per fare PieceIsEaten"));
		return;

	}

}


void  AChess_Piece::SelfDestroy()
{
	Destroy();
}

/*
AChess_Piece* AChess_Piece::ClonePieces()
{
	return nullptr;
}
*/