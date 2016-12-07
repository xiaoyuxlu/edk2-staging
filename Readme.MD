This branch is used to develop the HTTPS boot feature including TLS and related libraries.

The branch owner:
Jiaxin Wu < jiaxin.wu@intel.com >

## Feature Introduction
The security of HTTPS boot is that of the underlying TLS. In simple terms, HTTPS boot refers to the use of HTTP boot over TLS session. **TlsDxe** driver takes advantage of OpenSLL library, including **BaseCryptLib** and new wrapped **TlsLib**, And also, **OpensslLib** module is required to enable 'openssl/ssl' function. **HttpDxe** driver in this branch consumes TlsDxe driver to support HTTPS feature. The HTTP instance can be able to determine whether to use HTTP or HTTPS feature by according to the different schemes ("http://" or "https://") in the boot file URI. For detailed feature introduction, please refer to [EDKII HTTPS Boot Getting Started Guide](https://github.com/tianocore-docs/Docs/blob/master/White_Papers/EDKIIHttps_TLS_BootGettingStartedGuide_07.pdf).

#### Related Protocols
The following protocols are related to HTTPS boot:
```
HTTP Service Binding Protocol
HTTP Protocol
HTTP Utilities Protocol
TLS Service Binding Protocol
TLS Protocol
TLS Configuration Protocol
```

#### Related Modules
The following modules are related to HTTPS boot:
```
MdeModulePkg/Library/DxeHttpLib/DxeHttpLib.inf
NetworkPkg/HttpBootDxe/HttpBootDxe.inf
NetworkPkg/HttpDxe/HttpDxe.inf
NetworkPkg/HttpUtilitiesDxe/HttpUtilitiesDxe.inf
CryptoPkg/Library/OpensslLib/OpensslLib.inf
CryptoPkg/Library/BaseCryptLib/BaseCryptLib.inf
CryptoPkg/Library/TlsLib/TlsLib.inf
NetworkPkg/TlsDxe/TlsDxe.inf
NetworkPkg/TlsAuthConfigDxe/TlsAuthConfigDxe.inf
```

#### HTTPS Authentication
TLS supports three authentication modes ([RFC5246](https://tools.ietf.org/html/rfc5246)):
```
1. Total anonymity: the server and client will not authenticate each other.
2. One-way authentication: server authentication with an unauthenticated client.
3. Two-way authentication: authentication of both parties.
```
Currently, HTTPS boot feature only support server authentication with an unauthenticated client mode. Others are not in our current feature support scope. To support one-way authentication mode, server CA certificate is required by Client. Private variable is used to configure this CA certificate. **EFI_SIGNATURE_LIST** format is used for this variable. In sum, the Server CA certificate must be configured first to enable HTTPS boot feature. The variable name and GUID are defined as below.
```
#define EFI_TLS_CA_CERTIFICATE_GUID \
  { \
    0xfd2340D0, 0x3dab, 0x4349, { 0xa6, 0xc7, 0x3b, 0x4f, 0x12, 0xb4, 0x8e, 0xae } \
  }

#define EFI_TLS_CA_CERTIFICATE_VARIABLE          L"TlsCaCertificate"
```
**TlsAuthConfigDxe** is a temporary driver to provide an UI to support the required certificate configuration.

## Timeline
| Time | Event | Related Module |
|:----:|:-----:|:--------------:|
| 2015.12 | Initial implementation for HTTPS boot feature by leveraging OpenSSL. | HttpDxe, TlsLib, TlsDxe |
| 2016.3 | Provided an UI configured driver to support TLS server authentication. | TlsAuthConfigDxe |
| 2016.4-7 | Hotfix and code refine according community feedback. | Tls1.h, TlsLib, HttpDxe |
| 2016.8 | Prevent the CA certificates from the runtime phase attack. | TlsAuthConfigDxe |
| 2016.9 | Support TLS Version negotiation. | TlsLib, HttpDxe |
|...|...|...|

## Feature Support Scope
*	Feature usage: Load the specified file from the remote HTTPS server successfully and steadily.
* UEFI Arch: IA32 and X64 platform.
*	TLS version: TLS1.0/1.1/1.2, version negotiation.
*	HTTPS authentication mode: One-way authentication.
*	CA certificates management: Private variable, prevent runtime phase attack.

## Feature Verification
Tomcat, IIS 8 and Apache2 are selected as the HTTPS server to verify the result of loading the UEFI shell boot file (Shell.efi), detailed see below table.

| HTTPS Server | TLS 1.0 | TLS 1.1 | TLS1.2 |
|:------------:|:-------:|:-------:|:------:|
|Tomcat | Pass |Pass | Pass |
|IIS 8 | Pass | Pass | Failure |
|Apache2 | Pass | Pass | Pass |

### NOTES
TLS version 1.2 in windows server 2012 R2 IIS8 (As HTTPS server) CAN NOT collaborate with UEFI HTTPS client while version 1.1/1.0 works well. To make the UEFI HTTPS client in staging works properly, we have to disable TLS version 1.2 in windows server 2012 R2 by the below PowerShell script:
```
New-Item 'HKLM:\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.2\Server' -Force | Out-Null
New-ItemProperty -path 'HKLM:\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.2\Server' -name 'Enabled' -value 0 -PropertyType 'DWord' -Force | Out-Null
New-ItemProperty -path 'HKLM:\SYSTEM\CurrentControlSet\Control\SecurityProviders\SCHANNEL\Protocols\TLS 1.2\Server' -name 'DisabledByDefault' -value 1 -PropertyType 'DWord' -Force | Out-Null
Write-Host 'TLS 1.2 server has been disabled.'
Write-Host 'NOTE: Need to restart the OS to take effect!'
```
