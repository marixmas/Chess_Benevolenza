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

	WinSize = 3;
	// size of the field (3x3)
	Size = 8;

	// tile dimension
	TileSize = 120;

	// tile padding dimension
	CellPadding = 0;
	
	// bool per controllare che la gamefield venga spawnata una sola volta
	//bFieldGenerated = false;

	// bool per controllare che i pezzi vengano spawnati una sola volta
	//bPiecesGenerated = false;
}

void AGameField::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//normalized tilepadding
	NormalizedCellPadding = FMath::RoundToDouble(((TileSize + CellPadding) / TileSize) * 100) / 100;
}

// Add a boolean flag to track whether the field has been generated
//bool bFieldGenerated = false;

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
	Super::BeginPlay();

	// Check if the field has already been generated
	//if (!bFieldGenerated)
	//{
		GenerateField();
	//	bFieldGenerated = true;

	//	if (!bPiecesGenerated)
	//	{
			GeneratePieces();
	//		bPiecesGenerated = true;
	//	}
	
	//}
}


// Assumi che TileSize rappresenti la dimensione di ciascuna tile della scacchiera da 
/*

AGameField* AChess_GameMode::GetGField() const
{
	return GField;
}

*/

void AGameField::GeneratePieces()
{
	//if (!bPiecesGenerated)
	//{
		//AChess_GameMode* GameMode = Cast<AChess_GameMode>(GetWorld()->GetAuthGameMode());
		//AGameField* GField = GameMode->GetGField();
		// controlla se il puntatore GField non è nullo
		/*if (GField != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("HA FUNZIONATO LA TUA ROBAAA. CIAO DA GAMEFIELD.CPP!"));
		}
		else
		{
			// estisci il caso in cui il puntatore GField sia nullo
			UE_LOG(LogTemp, Warning, TEXT("Il puntatore GField è    nullo!"));
		}
		*/
		// float TileSize = GField->TileSize;					// il debugger mi diceva che se dichiaro TileSize. il membro della classe verrà nascosto quindi boh provo a togliere

		//const float TileScale = TileSize / 100;					// lo vuole in ogni iterazione del for allora lo metto nella funzione SpawnChessPiece

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




			}
		}
		//bPiecesGenerated = true;
	
	
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
	PiecesMap.Add(FVector2D(x, y), Obj);
	ReversePiecesMap.Add(Obj, FVector2D(x, y));								/// no???? nel dubbio

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

void AGameField::SpawnTile(int32 x, int32 y)														// necessaria insieme ad altre cose che devo ancora fare SE NON FUNZINOA SET MATERIAL PER HIGHLIGHT DELLE TILE DEI SUGGERIMENTI MOSSE
{
	FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
	
		ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass1, Location, FRotator::ZeroRotator);
		const float TileScale = TileSize / 100;
		Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
		Obj->SetGridPosition(x, y);
		TileArray.Add(Obj);
		TileMap.Add(FVector2D(x, y), Obj);

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
        GameMode->IsGameOver = false;
        GameMode->MoveCounter = 0;
        GameMode->ChoosePlayerAndStartGame();
    }
}

void AGameField::GenerateField()
{
	//if (!bFieldGenerated)
	//{
		UE_LOG(LogTemp, Error, TEXT("è stata chiamata GenerateField"));

		/*
		// Pulizia dell'array TileArray
		for (ATile* Tile : TileArray)
		{
			Tile->Destroy(); // Oppure un'altra azione per rimuovere l'oggetto dal mondo
		}
		TileArray.Empty();

		// Pulizia della mappa TileMap
		TileMap.Empty();
		*/

		for (int32 x = 0; x < Size; x++)
		{
			for (int32 y = 0; y < Size; y++)
			{
				FVector Location = AGameField::GetRelativeLocationByXYPosition(x, y);
				if ((x + y) % 2 == 0)
				{
					ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass1, Location, FRotator::ZeroRotator);
					Obj->SetTileMaterial(TileMaterial1);
					const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					TileArray.Add(Obj);
					TileMap.Add(FVector2D(x, y), Obj);

				}
				else
				{
					ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass2, Location, FRotator::ZeroRotator);
					Obj->SetTileMaterial(TileMaterial2);
					const float TileScale = TileSize / 100;
					Obj->SetActorScale3D(FVector(TileScale, TileScale, 0.2));
					Obj->SetGridPosition(x, y);
					TileArray.Add(Obj);
					TileMap.Add(FVector2D(x, y), Obj);
				}
			}
		}
		//bFieldGenerated = true;
	//}
	//else {}
}

FVector2D AGameField::GetPosition(const FHitResult& Hit)
{
	return Cast<ATile>(Hit.GetActor())->GetGridPosition();
}

TArray<ATile*>& AGameField::GetTileArray()
{
	// TODO: inserire l'istruzione return qui			DONE
	return TileArray;
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

bool AGameField::IsWinPosition(const FVector2D Position) const
{
	const int32 Offset = WinSize - 1;
	// vertical lines
	for (int32 i = Position[0] - Offset; i <= Position[0]; i++)
	{
		if (IsWinLine(FVector2D(i, Position[1]), FVector2D(i + Offset, Position[1])))
		{
			return true;
		}
	}

	// horizontal lines
	for (int32 i = Position[1] - Offset; i <= Position[1]; i++)
	{
		if (IsWinLine(FVector2D(Position[0], i), FVector2D(Position[0], i + Offset)))
		{
			return true;
		}
	}

	// diagonal lines
	for (int32 i = -Offset; i <= 0; i++)
	{
		if (IsWinLine(FVector2D(Position[0] + i, Position[1] + i), FVector2D(Position[0] + Offset + i, Position[1] + Offset + i)))
		{
			return true;
		}
		if (IsWinLine(FVector2D(Position[0] + i, Position[1] - i), FVector2D(Position[0] + Offset + i, Position[1] - Offset - i)))
		{
			return true;
		}
	}

	return false;
}

inline bool AGameField::IsWinLine(const FVector2D Begin, const FVector2D End) const
{
	return IsValidPosition(Begin) && IsValidPosition(End) && AllEqual(GetLine(Begin, End));
}

inline bool AGameField::IsValidPosition(const FVector2D Position) const
{
	return 0 <= Position[0] && Position[0] < Size && 0 <= Position[1] && Position[1] < Size;
}

TArray<int32> AGameField::GetLine(const FVector2D Begin, const FVector2D End) const
{
	int32 xSign;
	if (Begin[0] == End[0])
	{
		xSign = 0;
	}
	else
	{
		xSign = Begin[0] < End[0] ? 1 : -1;
	}

	int32 ySign;
	if (Begin[1] == End[1])
	{
		ySign = 0;
	}
	else
	{
		ySign = Begin[1] < End[1] ? 1 : -1;
	}

	TArray<int32> Line;
	int32 x = Begin[0] - xSign;
	int32 y = Begin[1] - ySign;
	do
	{
		x += xSign;
		y += ySign;
		Line.Add((TileMap[FVector2D(x, y)])->GetOwner());
	} while (x != End[0] || y != End[1]);

	return Line;
}

bool AGameField::AllEqual(const TArray<int32>& Array) const
{
	if (Array.Num() == 0)
	{
		return false;
	}
	const int32 Value = Array[0];

	if (Value == NOT_ASSIGNED)
	{
		return false;
	}

	for (int32 i = 1; i < Array.Num(); i++)
	{
		if (Value != Array[i])
		{
			return false;
		}
	}

	return true;
}

/*
AGameField* AGameField::GetGField() const
{
	return GField;
}
*/

/*
// Called every frame
void AGameField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/
