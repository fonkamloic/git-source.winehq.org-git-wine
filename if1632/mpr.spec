name mpr
type win32

0009 stub DllCanUnloadNow
0010 stub DllGetClassObject
0025 stdcall MultinetGetConnectionPerformanceA(ptr ptr) MultinetGetConnectionPerformance32A
0026 stub MultinetGetConnectionPerformanceW
0027 stub MultinetGetErrorTextA
0028 stub MultinetGetErrorTextW
0029 stub NPSAuthenticationDialogA
0030 stub NPSCopyStringA
0031 stub NPSDeviceGetNumberA
0032 stub NPSDeviceGetStringA
0033 stub NPSGetProviderHandleA
0034 stub NPSGetProviderNameA
0035 stub NPSGetSectionNameA
0036 stub NPSNotifyGetContextA
0037 stub NPSNotifyRegisterA
0038 stub NPSSetCustomTextA
0039 stub NPSSetExtendedErrorA
0040 stub PwdChangePasswordA
0041 stub PwdChangePasswordW
0042 stub PwdGetPasswordStatusA
0043 stub PwdGetPasswordStatusW
0044 stub PwdSetPasswordStatusA
0045 stub PwdSetPasswordStatusW
0046 stub WNetAddConnection2A
0047 stub WNetAddConnection2W
0048 stub WNetAddConnection3A
0049 stub WNetAddConnection3W
0050 stub WNetAddConnectionA
0051 stub WNetAddConnectionW
0052 stub WNetCachePassword
0053 stub WNetCancelConnection2A
0054 stub WNetCancelConnection2W
0055 stub WNetCancelConnectionA
0056 stub WNetCancelConnectionW
0057 stub WNetCloseEnum
0060 stub WNetConnectionDialog
0058 stub WNetConnectionDialog1A
0059 stub WNetConnectionDialog1W
0063 stub WNetDisconnectDialog
0061 stub WNetDisconnectDialog1A
0062 stub WNetDisconnectDialog1W
0064 stub WNetEnumCachedPasswords
0065 stub WNetEnumResourceA
0066 stub WNetEnumResourceW
0067 stub WNetFormatNetworkNameA
0068 stub WNetFormatNetworkNameW
0069 stdcall WNetGetCachedPassword(ptr long ptr ptr long) WNetGetCachedPassword
0070 stdcall WNetGetConnectionA(ptr ptr ptr) WNetGetConnection32A
0071 stub WNetGetConnectionW
0072 stub WNetGetHomeDirectoryA
0073 stub WNetGetHomeDirectoryW
0074 stub WNetGetLastErrorA
0075 stub WNetGetLastErrorW
0076 stub WNetGetNetworkInformationA
0077 stub WNetGetNetworkInformationW
0078 stub WNetGetProviderNameA
0079 stub WNetGetProviderNameW
0080 stdcall WNetGetResourceInformationA(ptr ptr ptr ptr) WNetGetResourceInformation32A
0081 stub WNetGetResourceInformationW
0082 stub WNetGetResourceParentA
0083 stub WNetGetResourceParentW
0084 stub WNetGetUniversalNameA
0085 stub WNetGetUniversalNameW
0086 stub WNetGetUserA
0087 stub WNetGetUserW
0088 stub WNetLogoffA
0089 stub WNetLogoffW
0090 stub WNetLogonA
0091 stub WNetLogonW
0092 stdcall WNetOpenEnumA(long long ptr ptr) WNetOpenEnum32A
0093 stub WNetOpenEnumW
0094 stub WNetRemoveCachedPassword
0095 stub WNetRestoreConnectionA
0096 stub WNetRestoreConnectionW
0097 stub WNetSetConnectionA
0098 stub WNetSetConnectionW
0099 stub WNetUseConnectionA
0100 stub WNetUseConnectionW
0101 stub WNetVerifyPasswordA
0102 stub WNetVerifyPasswordW 
#additions
0103 stub WNetRestoreConnection
