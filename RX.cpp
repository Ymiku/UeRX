// Copyright © 2018-2020 Tencent. All Rights Reserved.

#include "RX.h"

FRXModelBase* URXInstance::Start()
{
	if(Root == nullptr)
	{
		Root = new FRXRoot();
	}
	
	if(!bCanReuse)
	{
		delete Root->Next;
		Root->Next = nullptr;
	}
	
	Root->RXInstance = this;
	ActiveModel = Root;
	return Root;

	//this->Start()->Execute([this](){ActiveModel = nullptr; })->ExecuteWhen([](){},[](){return false;});
}

void URXInstance::SetCanReuse(bool bCanReuse)
{
	this->bCanReuse = bCanReuse;
}

bool URXInstance::IsRunning()
{
	return ActiveModel != nullptr;
}

void URXInstance::Tick(float DeltaTime)
{
	if(ActiveModel == nullptr)
	{
		return;
	}
	
	ActiveModel->Tick(DeltaTime);
}

URXInstance::~URXInstance()
{
	if(Root != nullptr)
	{
		delete Root;
		Root = nullptr;
	}
	
	ActiveModel = nullptr;
}

void FRXModelBase::Tick(float DeltaTime)
{

}

FRXModelBase::~FRXModelBase()
{
	if(Next != nullptr)
	{
		delete Next;
	}
	
	Next = nullptr;
	RXInstance = nullptr;
}


void FRXModelBase::MoveNext()
{
	if(Next == nullptr)
	{
		RXInstance->ActiveModel = nullptr;
		return;
	}
	RXInstance->ActiveModel = Next;
}

template <class T>
FORCEINLINE T* FRXModelBase::Get()
{
	if (RXInstance->bCanReuse)
	{
		return static_cast<T*>(Next);
	}
	return new T();
}

void FRXRoot::Tick(float DeltaTime)
{
	MoveNext();
}

FRXModelBase* FRXModelBase::Execute(std::function<void()> Func)
{
	FRXExecute* Model = Get<FRXExecute>();
	Model->RXInstance = this->RXInstance;
	Model->Func = std::move(Func);
	Next = Model;
	return Next;
}
void FRXExecute::Tick(float DeltaTime)
{
	Func();
	MoveNext();
}

FRXModelBase* FRXModelBase::Wait(float WaitTime)
{
	FRXWait* Model = Get<FRXWait>();
	Model->RXInstance = this->RXInstance;
	Model->WaitTime = WaitTime;
	Next = Model;
	return Next;
}

void FRXWait::Tick(float DeltaTime)
{
	WaitTime -= DeltaTime;
	if(WaitTime <= 0.f)
	{
		MoveNext();
	}
}

FRXModelBase* FRXModelBase::ExecuteWhen(std::function<void()> Func, std::function<bool()> CondFunc)
{
	FRXExecuteWhen* Model = Get<FRXExecuteWhen>();
	Model->RXInstance = this->RXInstance;
	Model->Func = std::move(Func);
	Model->CondFunc = std::move(CondFunc);
	Next = Model;
	return Next;
}

void FRXExecuteWhen::Tick(float DeltaTime)
{
	if(CondFunc())
	{
		Func();
		MoveNext();
	}
}
FRXModelBase* FRXModelBase::ExecuteUtil(std::function<void()> Func, std::function<bool()> CondFunc)
{
	FRXExecuteUtil* Model = Get<FRXExecuteUtil>();
	Model->RXInstance = this->RXInstance;
	Model->Func = std::move(Func);
	Model->CondFunc = std::move(CondFunc);
	Next = Model;
	return Next;
}

void FRXExecuteUtil::Tick(float DeltaTime)
{
	Func();
	if(CondFunc())
	{
		MoveNext();
	}
}

FRXModelBase* FRXModelBase::ExecuteContinuous(std::function<void()> Func, float Time)
{
	FRXExecuteContinuous* Model = Get<FRXExecuteContinuous>();
	Model->RXInstance = this->RXInstance;
	Model->Func = std::move(Func);
	Model->TimeCount = Time;
	Next = Model;
	return Next;
}

void FRXExecuteContinuous::Tick(float DeltaTime)
{
	Func();
	TimeCount -= FApp::GetDeltaTime();
	if(TimeCount <= 0.f)
	{
		MoveNext();
	}
}

void FRXModelBase::GoToBegin()
{
	FRXGoToBegin* Model = Get<FRXGoToBegin>();
	Next = Model;
}

void FRXGoToBegin::Tick(float DeltaTime)
{
	RXInstance->ActiveModel = RXInstance->Root;
	MoveNext();
}
