// Melee Combat System. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Online/Auth.h"
#include "Online/OnlineServices.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Online/TitleFile.h"
#include "OnlineSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOnlineSubsystem, All, All);

DECLARE_MULTICAST_DELEGATE(FOnLogin);

struct OnlineUserInfo
{
	int32 LocalUserIndex = -1;
	FPlatformUserId PlatformUserId;
	UE::Online::FAccountId AccountId;
};

UCLASS()
class MELEECOMBATSYSTEM_API UOnlineSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

  public:
	FOnLogin OnLoginDelegate;

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void Login(const FPlatformUserId& PlatformUserId);

	/** Called to read the Game's Title File from the backend services and return the contents */
	FString ReadTitleFile(FString Filename, FPlatformUserId PlatformUserId);

  protected:
	void HandleLogin(const UE::Online::TOnlineResult<UE::Online::FAuthLogin>& Result);

	void RegisterLocalOnlineUser(FPlatformUserId PlatformUserId);

	void RetrieveTitleFile(FString Filename, FPlatformUserId PlatformUserId);

	void HandleEnumerateFiles(const UE::Online::TOnlineResult<UE::Online::FTitleFileEnumerateFiles>& EnumerateFilesResult,
							  OnlineUserInfo OnlineUser,
							  FString Filename);

	void HandleReadFile(const UE::Online::TOnlineResult<UE::Online::FTitleFileReadFile>& ReadFileResult, FString Filename);

  private:
	UE::Online::IOnlineServicesPtr OnlineServices  = nullptr;
	UE::Online::IAuthPtr AuthInterface			   = nullptr;
	UE::Online::ITitleFilePtr TitleFileInterface   = nullptr;

	UE::Online::EOnlineServices OnlineServicesType = UE::Online::EOnlineServices::None;

	UE::Online::FTitleFileContents TitleFileContent;

	TMap<FPlatformUserId, OnlineUserInfo> OnlineUserInfos;
};
