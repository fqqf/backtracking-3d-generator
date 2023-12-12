// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <stack>
#include "Engine/World.h"
#include "SpaceMaze.generated.h"


UCLASS()
class MAZE3D_API ASpaceMaze : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpaceMaze();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	USceneComponent* Scene;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Visited;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Blue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> Wall;
	FVector FindRandomStartPosition(const TArray<TArray<int32>>& Maze);
	int32 CountMazeCells(const TArray<TArray<int32>>& Maze);
	void SpawnWallBetweenCells(FVector& CellOnePosition, FVector& CellTwoPosition);
	void LoadAndSpawnItems();
	void SpawnItem(FVector& Position, UClass* Class, FRotator Rotation = FRotator::ZeroRotator);
	FString FileName = TEXT("C:\\Users\\user\\Documents\\Unreal Projects\\maze3d\\output.txt");
	std::stack<FVector> BacktrackingStack;

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void printDebug(FVector DebugCube);
	TArray<TArray<int32>> MazeArray;
	enum {
		EDGE_MAZE = 0x1,
		CELL_MAZE = 0x2,
		// В клетках между которыми нету PATH, не будет прохода
		PATH_NORTH = 0x04,
		PATH_EAST = 0x08,
		PATH_SOUTH = 0x10,
		PATH_WEST = 0x20,
		CELL_VISITED = 0x40,
	};

	enum Direction
	{
		NORTH = 0,
		SOUTH = 1,
		EAST = 2,
		WEST = 3
	};
};