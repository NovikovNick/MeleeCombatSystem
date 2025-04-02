// Melee Combat System. All Rights Reserved.

#include "Online/OnlineSubsystem.h"

#include "Online/CoreOnline.h"
#include "Online/OnlineResult.h"
#include "Online/OnlineAsyncOpHandle.h"
#include "Online/OnlineError.h"
#include "Online/OnlineServices.h"
#include "Online/Auth.h"
#include "Online/TitleFile.h"

DEFINE_LOG_CATEGORY(LogOnlineSubsystem);

/// <summary>
/// Whether to create this subsystem. For simplicity, the subsystem is only
///	created on clients and standalone games, not servers. This function
///	is often used to limit creation of subsystems to a server or client.
///	Be sure to null-check subsystem before usage!
///
/// Also if some setup of Online Services will be broken, skip subsystem creation.
/// </summary>
/// <param name="Outer"></param>
/// <returns>Boolean whether or not to create this subsystem</returns>
bool UOnlineSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
#if UE_SERVER
	return false;
#else
	TSharedPtr<UE::Online::IOnlineServices> Services = UE::Online::GetServices();
	if (Services.IsValid()						   //
		&& Services->GetAuthInterface().IsValid()  //
		&& Services->GetTitleFileInterface().IsValid())
	{
		return Super::ShouldCreateSubsystem(Outer);
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Unable to setup online interfaces"));
		return false;
	}
#endif
}

/// <summary>
/// Initialize called after the Game Instance is initialized
/// </summary>
/// <param name="Collection">Collection of subsystems that are initialized by the game instance</param>
void UOnlineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Initialize Services ptr
	OnlineServices = UE::Online::GetServices();
	check(OnlineServices.IsValid());

	// Verify Services type
	OnlineServicesType = OnlineServices->GetServicesProvider();
	if (OnlineServices.IsValid())
	{
		// Initialize Interface ptrs
		AuthInterface = OnlineServices->GetAuthInterface();
		check(AuthInterface.IsValid());

		TitleFileInterface = OnlineServices->GetTitleFileInterface();
		check(TitleFileInterface.IsValid());

		UE_LOG(LogOnlineSubsystem, Display, TEXT("Initialized successufully"));
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Error: Failed to initialize services"));
	}
}

/// <summary>
/// Deinitialize called before the Game Instance is deinitialized/shutdown
/// </summary>
void UOnlineSubsystem::Deinitialize()
{
	UE_LOG(LogOnlineSubsystem, Display, TEXT("OnlineSampleOnlineSubsystem deinitialized."));

	// Unbind event handles and reset struct info
	OnlineServices.Reset();
	AuthInterface.Reset();
	TitleFileInterface.Reset();
	OnlineServicesType = UE::Online::EOnlineServices::None;

	// Deinitialize parent class
	Super::Deinitialize();
}

void UOnlineSubsystem::Login(const FPlatformUserId& PlatformUserId)
{
	if (AuthInterface->IsLoggedIn(PlatformUserId) && OnlineUserInfos.Contains(PlatformUserId))
	{
		UE_LOG(LogOnlineSubsystem, Display, TEXT("Already logged"));
		return;
	}

	if (OnlineServicesType == UE::Online::EOnlineServices::Null)
	{
		UE_LOG(LogOnlineSubsystem,
			   Display,
			   TEXT("Online Services Null plugin is used. Users are automatically registered with the Null services so they do not "
					"need to call a login function. Skip login"));
		RegisterLocalOnlineUser(PlatformUserId);
		return;
	}

	UE::Online::FAuthLogin::Params Params;
	Params.PlatformUserId  = PlatformUserId;
	Params.CredentialsType = UE::Online::LoginCredentialsType::AccountPortal;

	AuthInterface->Login(MoveTemp(Params)).OnComplete(this, &ThisClass::HandleLogin);

	UE_LOG(LogOnlineSubsystem, Display, TEXT("Login started"));
}

/// <summary>
/// Obtain title file and read its contents.
/// </summary>
/// <param name="Filename">File to read</param>
/// <param name="PlatformUserId">User to read file for</param>
/// <returns>FileString with contents of Filename</returns>
FString UOnlineSubsystem::ReadTitleFile(FString Filename, FPlatformUserId PlatformUserId)
{
	using namespace UE::Online;

	RetrieveTitleFile(Filename, PlatformUserId);
	FString FileString = FString(TitleFileContent.Num(), UTF8_TO_TCHAR(TitleFileContent.GetData()));
	UE_LOG(LogOnlineSubsystem, Display, TEXT("Reading Title File: %s"), *Filename);
	return FileString;
}

void UOnlineSubsystem::HandleLogin(const UE::Online::TOnlineResult<UE::Online::FAuthLogin>& Result)
{
	if (Result.IsOk())
	{
		const TSharedRef<UE::Online::FAccountInfo> AccountInfo = Result.GetOkValue().AccountInfo;
		UE_LOG(LogOnlineSubsystem, Display, TEXT("login success"));
		RegisterLocalOnlineUser(AccountInfo->PlatformUserId);
	}
	else
	{
		UE::Online::FOnlineError Error = Result.GetErrorValue();
		UE_LOG(LogOnlineSubsystem, Display, TEXT("login error"));
	}
}

/// <summary>
/// Register online user with local registry OnlineUserInfos
/// </summary>
/// <param name="PlatformUserId">Platform user id of user to register</param>
void UOnlineSubsystem::RegisterLocalOnlineUser(FPlatformUserId PlatformUserId)
{
	using namespace UE::Online;

	FAuthGetLocalOnlineUserByPlatformUserId::Params GetUserParams;
	GetUserParams.PlatformUserId = PlatformUserId;
	if (AuthInterface.IsValid())
	{
		TOnlineResult<FAuthGetLocalOnlineUserByPlatformUserId> AuthGetResult =
			AuthInterface->GetLocalOnlineUserByPlatformUserId(MoveTemp(GetUserParams));

		if (AuthGetResult.IsOk())
		{
			FAuthGetLocalOnlineUserByPlatformUserId::Result& LocalOnlineUser = AuthGetResult.GetOkValue();
			TSharedRef<FAccountInfo> UserAccountInfo						 = LocalOnlineUser.AccountInfo;
			FAccountInfo UserAccountInfoContent								 = *UserAccountInfo;
			if (!OnlineUserInfos.Contains(UserAccountInfoContent.PlatformUserId))
			{
				OnlineUserInfo NewUser{UserAccountInfoContent.AccountId.GetHandle(), PlatformUserId, UserAccountInfoContent.AccountId};
				OnlineUserInfos.Add(PlatformUserId, NewUser);
			}
			else
			{
				UE_LOG(LogOnlineSubsystem,
					   Display,
					   TEXT("Local User with platform user id %d already registered."),
					   PlatformUserId.GetInternalId());
			}
		}
		else
		{
			FOnlineError ErrorResult = AuthGetResult.GetErrorValue();
			UE_LOG(LogOnlineSubsystem, Error, TEXT("Get Local Online User Error: %s"), *ErrorResult.GetLogString());
		}
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Auth Interface pointer invalid."));
	}
}

/// <summary>
/// EnumerateFiles, GetEnumeratedFiles, and ReadFile. This implementation uses a lambda function to handle the OnComplete callback.
/// </summary>
/// <param name="Filename">File to read</param>
/// <param name="PlatformUserId">User to retrieve file for</param>
void UOnlineSubsystem::RetrieveTitleFile(FString Filename, FPlatformUserId PlatformUserId)
{
	using namespace UE::Online;

	FAccountId LocalAccountId;
	OnlineUserInfo* OnlineUser = OnlineUserInfos.Find(PlatformUserId);
	if (OnlineUser)
	{
		FTitleFileEnumerateFiles::Params Params;
		Params.LocalAccountId = OnlineUser->AccountId;
		if (TitleFileInterface.IsValid())
		{
			TitleFileInterface->EnumerateFiles(MoveTemp(Params))
				.OnComplete(this, &ThisClass::HandleEnumerateFiles, *OnlineUser, Filename);
		}
		else
		{
			UE_LOG(LogOnlineSubsystem, Error, TEXT("Title File Interface pointer invalid."));
		}
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Could not find user with Platform User Id: %d"), PlatformUserId.GetInternalId());
	}
}

/// <summary>
/// Handle the asynchronous EnumerateFiles function. Log if there is a failure. Success calls the asynchronous ReadFile function
/// handled by HandleReadFile.
/// </summary>
/// <param name="EnumerateFilesResult">Result of Enumerate Files attempt</param>
/// <param name="OnlineUser">User that queried for title file</param>
/// <param name="Filename">Name of file user queried</param>
void UOnlineSubsystem::HandleEnumerateFiles(
	const UE::Online::TOnlineResult<UE::Online::FTitleFileEnumerateFiles>& EnumerateFilesResult,
	OnlineUserInfo OnlineUser,
	FString Filename)
{
	using namespace UE::Online;

	if (EnumerateFilesResult.IsOk())
	{
		FTitleFileGetEnumeratedFiles::Params GetParams;
		GetParams.LocalAccountId							  = OnlineUser.AccountId;

		TOnlineResult<FTitleFileGetEnumeratedFiles> GetResult = TitleFileInterface->GetEnumeratedFiles(MoveTemp(GetParams));
		if (GetResult.IsOk())
		{
			FTitleFileGetEnumeratedFiles::Result& CachedFiles = GetResult.GetOkValue();
			int32 FileIndex									  = CachedFiles.Filenames.Find(Filename);
			if (FileIndex == INDEX_NONE)
			{
				UE_LOG(LogOnlineSubsystem, Error, TEXT("Title File \"%s\" not found!"), *Filename);
			}
			else
			{
				FTitleFileReadFile::Params ReadParams;
				ReadParams.LocalAccountId = OnlineUser.AccountId;
				ReadParams.Filename		  = Filename;

				TitleFileInterface->ReadFile(MoveTemp(ReadParams)).OnComplete(this, &ThisClass::HandleReadFile, Filename);
			}
		}
		else
		{
			UE_LOG(LogOnlineSubsystem, Error, TEXT("Get Title File Error: %s"), *GetResult.GetErrorValue().GetLogString());
		}
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Enum Title File Error: %s"), *EnumerateFilesResult.GetErrorValue().GetLogString());
	}
}

/// <summary>
/// Handle the asynchronous ReadFile function. Log if there is a failure. Success populates the user's TitleFileContent.
/// </summary>
/// <param name="ReadFileResult">Result of ReadFile attempt</param>
/// <param name="Filename">Name of file to read</param>
void UOnlineSubsystem::HandleReadFile(const UE::Online::TOnlineResult<UE::Online::FTitleFileReadFile>& ReadFileResult,
									  FString Filename)
{
	using namespace UE::Online;

	if (ReadFileResult.IsOk())
	{
		const FTitleFileReadFile::Result& ReadFileResultValue = ReadFileResult.GetOkValue();
		TitleFileContent									  = *ReadFileResultValue.FileContents;
	}
	else
	{
		UE_LOG(LogOnlineSubsystem, Error, TEXT("Read Title File Error: %s"), *ReadFileResult.GetErrorValue().GetLogString());
	}
}
