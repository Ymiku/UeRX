#pragma once

#include "CoreMinimal.h"
#include <functional>
#include "RX.generated.h"

class FRXExecute;
class FRXModelBase;
class FRXRoot;

UCLASS()
class URXInstance : public UObject
{
	GENERATED_BODY()
public:
	~URXInstance();
	FRXModelBase* Start();
	void Tick(float DeltaTime);
	bool IsRunning();
	void SetCanReuse(bool bCanReuse);
	
public:
	FRXModelBase* ActiveModel;
	FRXRoot* Root;

	bool bCanReuse = false;
	
};

class FRXModelBase
{
public:
	FRXModelBase* Next = nullptr;
	URXInstance* RXInstance = nullptr;// RXInstance肯定不会在FRXModelBase的生命周期中被释放

public:
	virtual void Tick(float DeltaTime);
	void MoveNext();
	template <class T>
	FORCEINLINE T* Get();
	FRXModelBase* Execute(std::function<void()> Func);
	FRXModelBase* Wait(float WaitTime);
	FRXModelBase* ExecuteWhen(std::function<void()> Func, std::function<bool()> CondFunc);
	FRXModelBase* ExecuteUtil(std::function<void()> Func, std::function<bool()> CondFunc);
	FRXModelBase* ExecuteContinuous(std::function<void()> Func, float Time);
	void GoToBegin();
	virtual ~FRXModelBase();
};

class FRXRoot : public FRXModelBase
{
	void Tick(float DeltaTime) override;
};

class FRXExecute : public FRXModelBase
{
public:
	std::function<void()> Func;
	void Tick(float DeltaTime) override;
};

class FRXWait : public FRXModelBase
{
public:
	float WaitTime;
	void Tick(float DeltaTime) override;
};

class FRXExecuteWhen : public FRXModelBase
{
public:
	std::function<void()> Func;
	std::function<bool()> CondFunc;
	void Tick(float DeltaTime) override;
};

class FRXExecuteUtil : public FRXModelBase
{
public:
	std::function<void()> Func;
	std::function<bool()> CondFunc;
	void Tick(float DeltaTime) override;
};

class FRXExecuteContinuous : public FRXModelBase
{
public:
	std::function<void()> Func;
	float TimeCount = 0.f;
	void Tick(float DeltaTime) override;
};

class FRXGoToBegin : public FRXModelBase
{
public:
	void Tick(float DeltaTime) override;
};
