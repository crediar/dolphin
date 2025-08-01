// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Common/CommonTypes.h"

class PointerWrap;

namespace Core
{
class System;
}

namespace File
{
class IOFile;
}

enum GameType
{
  FZeroAX = 1,
  FZeroAXMonster,
  MarioKartGP,
  MarioKartGP2,
  VirtuaStriker3,
  VirtuaStriker4,
  GekitouProYakyuu,
  KeyOfAvalon,
  FirmwareUpdate
};
enum MediaType
{
  GDROM = 1,
  NAND,
};
enum MediaBoardType
{
  NANDMaskBoardHDD = 0,
  NANDMaskBoardMask = 1,
  NANDMaskBoardNAND = 2,
  DIMMBoardType3 = 4,
};
enum MediaBoardStatus
{
  Initializing = 0,
  CheckingNetwork = 1,
  SystemDisc = 2,
  TestingGameProgram = 3,
  LoadingGameProgram = 4,
  LoadedGameProgram = 5,
  Error = 6
};
enum InquiryType
{
  Version1 = 0x21484100,
  Version2 = 0x29484100
};

#define SocketCheck(x) (x <= 0x3F ? x : 0)

namespace AMMediaboard
{
enum class AMMBCommand : u16
{
  Unknown_000 = 0x000,
  GetDIMMSize = 0x001,

  Inquiry = 0x12,
  Read = 0xa8,
  Write = 0xaa,
  Execute = 0xab,

  GetMediaBoardStatus = 0x100,
  GetSegaBootVersion = 0x101,
  GetSystemFlags = 0x102,
  GetMediaBoardSerial = 0x103,
  Unknown_104 = 0x104,

  NetworkReInit = 0x204,

  TestHardware = 0x301,

  // Network used by Mario Kart GPs
  Accept = 0x401,
  Bind = 0x402,
  Closesocket = 0x403,
  Connect = 0x404,
  GetIPbyDNS = 0x405,
  InetAddr = 0x406,
  Ioctl = 0x407,  // Unused
  Listen = 0x408,
  Recv = 0x409,
  Send = 0x40A,
  Socket = 0x40B,
  Select = 0x40C,
  Shutdown = 0x40D,  // Unused
  SetSockOpt = 0x40E,
  GetSockOpt = 0x40F,  // Unused
  SetTimeOuts = 0x410,
  GetLastError = 0x411,
  RouteAdd = 0x412,     // Unused
  RouteDelete = 0x413,  // Unused
  GetParambyDHCPExec = 0x414,
  ModifyMyIPaddr = 0x415,
  Recvfrom = 0x416,       // Unused
  Sendto = 0x417,         // Unused
  RecvDimmImage = 0x418,  // Unused
  SendDimmImage = 0x419,  // Unused

  // Network used by F-Zero AX
  InitLink = 0x601,
  Unknown_605 = 0x605,
  SetupLink = 0x606,
  SearchDevices = 0x607,
  Unknown_608 = 0x608,
  Unknown_614 = 0x614,

  // NETDIMM Commands
  Unknown_001 = 0x001,
  GetNetworkFirmVersion = 0x101,
  Unknown_103 = 0x103,
};

enum MediaBoardAddress : u32
{
  MediaBoardStatus1 = 0x80000000,
  MediaBoardStatus2 = 0x80000020,
  MediaBoardStatus3 = 0x80000040,

  FirmwareStatus1 = 0x80000120,
  FirmwareStatus2 = 0x80000140,

  BackupMemory = 0x000006A0,

  DIMMMemory = 0x1F000000,
  DIMMMemory2 = 0xFF000000,

  DIMMExtraSettings = 0x1FFEFFE0,

  NetworkControl = 0xFFFF0000,

  DIMMCommandVersion1 = 0x1F900000,
  DIMMCommandVersion2 = 0x84000000,
  DIMMCommandVersion2_2 = 0x89000000,
  DIMMCommandExecute2 = 0x88000000,

  NetworkCommandAddress = 0x1F800200,
  NetworkCommandAddress2 = 0x89040200,

  NetworkBufferAddress1 = 0x1FA00000,
  NetworkBufferAddress2 = 0x1FD00000,
  NetworkBufferAddress3 = 0x89100000,
  NetworkBufferAddress4 = 0x89180000,
  NetworkBufferAddress5 = 0x1FB00000,

  FirmwareAddress = 0x84800000,

  FirmwareMagicWrite1 = 0x00600000,
  FirmwareMagicWrite2 = 0x00700000,
};

// Mario Kart GP2 has a complete list of them
// but in japanese
// They somewhat match WSA errors codes
enum SocketStatusCodes
{
  SSC_E_4 = -4,  // Failure (abnormal argument)
  SSC_E_3 = -3,  // Success (unsupported command)
  SSC_E_2 =
      -2,  // Failure (failed to send, abnormal argument, or communication condition violation)
  SSC_E_1 = -1,  // Failure (error termination)

  SSC_EINTR = 4,    // An interrupt occurred before data reception was completed
  SSC_EBADF = 9,    // Invalid descriptor
  SSC_E_11 = 11,    // Send operation was blocked on a non-blocking mode socket
  SSC_EACCES = 13,  // The socket does not support broadcast addresses, but the destination address
                    // is a broadcast address
  SSC_EFAULT =
      14,  // The name argument specifies a location other than an address used by the process.
  SSC_E_23 = 23,     // System file table is full.
  SSC_AEMFILE = 24,  // Process descriptor table is full.
  SSC_EMSGSIZE =
      36,  // Socket tried to send message without splitting, but message size is too large.
  SSC_EAFNOSUPPORT = 47,   // Address prohibited for use on this socket.
  SSC_EADDRINUSE = 48,     // Address already in use.
  SSC_EADDRNOTAVAIL = 49,  // Prohibited address.
  SSC_E_50 = 50,           // Non-socket descriptor.
  SSC_ENETUNREACH = 51,    // Cannot access specified network.
  SSC_ENOBUFS = 55,        // Insufficient buffer
  SSC_EISCONN = 56,        // Already connected socket
  SSC_ENOTCONN = 57,       // No connection for connection-type socket
  SSC_ETIMEDOUT = 60,      // Timeout
  SSC_ECONNREFUSED = 61,   // Connection request forcibly rejected
  SSC_EHOSTUNREACH = 65,   // Remote host cannot be reached
  SSC_EHOSTDOWN = 67,      // Remote host is down
  SSC_EWOULDBLOCK = 70,    // Socket is in non-blocking mode and connection has not been completed
  SSC_E_69 = 69,  // Socket is in non-blocking mode and a previously issued Connect command has not
                  // been completed
  SSC_SUCCESS = 70,
};

void Init(void);
void FirmwareMap(bool on);
u8* InitDIMM(u32 size);
void InitKeys(u32 KeyA, u32 KeyB, u32 KeyC);
u32 ExecuteCommand(std::array<u32, 3>& DICMDBUF, u32 Address, u32 Length);
u32 GetGameType(void);
u32 GetMediaType(void);
bool GetTestMenu(void);
void Shutdown(void);
};  // namespace AMMediaboard
