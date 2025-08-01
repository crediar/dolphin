// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <SFML/Network.hpp>
#include <deque>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include "Common/CommonTypes.h"
#include "Common/Flag.h"
#include "Core/HW/SI/SI_Device.h"

namespace SerialInterface
{

// "JAMMA Video Standard" I/O
class JVSIOMessage
{
public:
  u32 m_ptr, m_last_start, m_csum;
  u8 m_msg[0x80];

  JVSIOMessage();
  void start(int node);
  void addData(const u8* dst, size_t len, int sync);
  void addData(const void* data, size_t len);
  void addData(const char* data);
  void addData(u32 n);
  void end();
};  // end class JVSIOMessage

// triforce (GC-AM) baseboard
class CSIDevice_AMBaseboard : public ISIDevice
{
private:
  enum BaseBoardCommand
  {
    GCAM_Reset = 0x00,
    GCAM_Command = 0x70,
  };

  enum GCAMCommand
  {
    StatusSwitches = 0x10,
    SerialNumber = 0x11,
    Unknown_12 = 0x12,
    Unknown_14 = 0x14,
    FirmVersion= 0x15,
    FPGAVersion= 0x16,
    RegionSettings = 0x1F,

    Unknown_21 = 0x21,
    Unknown_22 = 0x22,
    Unknown_23 = 0x23,
    Unknown_24 = 0x24,

    SerialA = 0x31,
    SerialB = 0x32,

    JVSIOA = 0x40,
    JVSIOB = 0x41,

    Unknown_60 = 0x60,
  };

  enum JVSIOCommand
  {
    IOID = 0x10,
    CommandRevision = 0x11,
    JVRevision = 0x12,
    CommunicationVersion = 0x13,
    CheckFunctionality = 0x14,
    MainID = 0x15,

    SwitchesInput = 0x20,
    CoinInput = 0x21,
    AnalogInput = 0x22,
    RotaryInput = 0x23,
    KeyCodeInput = 0x24,
    PositionInput = 0x25,
    GeneralSwitchInput = 0x26,

    PayoutRemain = 0x2E,
    Retrans = 0x2F,
    CoinSubOutput = 0x30,
    PayoutAddOutput = 0x31,
    GeneralDriverOutput = 0x32,
    AnalogOutput = 0x33,
    CharacterOutput = 0x34,
    CoinAddOutput = 0x35,
    PayoutSubOutput = 0x36,
    GeneralDriverOutput2 = 0x37,
    GeneralDriverOutput3 = 0x38,

    NAMCOCommand = 0x70,

    Reset = 0xF0,
    SetAddress = 0xF1,
    ChangeComm = 0xF2,
  };

  enum JVSIOStatusCode
  {
    StatusOkay = 1,
    UnsupportedCommand = 2,
    ChecksumError = 3,
    AcknowledgeOverflow = 4,
  };

  enum CARDCommand
  {
    Init = 0x10,
    GetState = 0x20,
    Read = 0x33,
    IsPresent = 0x40,
    Write = 0x53,
    SetPrintParam = 0x78,
    RegisterFont = 0x7A,
    WriteInfo = 0x7C,
    Erase = 0x7D,
    Eject = 0x80,
    Clean = 0xA0,
    Load = 0xB0,
    SetShutter = 0xD0,
  };

  enum ICCARDCommand
  {
    GetStatus = 0x10,
    SetBaudrate = 0x11,
    FieldOn = 0x14,
    FieldOff = 0x15,
    InsertCheck = 0x20,
    AntiCollision = 0x21,
    SelectCard = 0x22,
    ReadPage = 0x24,
    WritePage = 0x25,
    DecreaseUseCount = 0x26,
    ReadUseCount = 0x33,
    ReadPages = 0x34,
    WritePages = 0x35,
  };

  enum CDReaderCommand
  {
    ShutterAuto = 0x61,
    BootVersion = 0x62,
    SensLock = 0x63,
    SensCard = 0x65,
    FirmwareUpdate =0x66,
    ShutterGet = 0x67,
    CameraCheck = 0x68,
    ShutterCard = 0x69,
    ProgramChecksum = 0x6B,
    BootChecksum = 0x6D,
    ShutterLoad = 0x6F,
    ReadCard = 0x72,
    ShutterSave = 0x73,
    SelfTest = 0x74,
    ProgramVersion = 0x76,
  };

  union ICCommand
  {
    u8 data[81 + 4 + 4 + 4];

    struct
    {
      u32 pktcmd : 8;
      u32 pktlen : 8;
      u32 fixed : 8;
      u32 command : 8;
      u32 flag : 8;
      u32 length : 8;
      u32 status : 16;

      u8 extdata[81];
      u32 extlen;
    };
  };

  u16 m_coin[2];
  u32 m_coin_pressed[2];

  u8 m_ic_card_data[2048];
  u16 m_ic_card_state;
  /*
       0 - OK
    8000 - no card
    800E - ???
    other- bad card
  */
  u16 m_ic_card_status;
  u16 m_ic_card_session;
  u8 m_ic_write_buffer[512];
  u32 m_ic_write_offset;
  u32 m_ic_write_size;

  u8 m_card_memory[0xD0];
  u8 m_card_read_packet[0xDB];
  u8 m_card_buffer[0x100];
  u32 m_card_memory_size;
  u32 m_card_is_inserted;
  u32 m_card_command;
  u32 m_card_clean;
  u32 m_card_write_length;
  u32 m_card_wrote;
  u32 m_card_read_length;
  u32 m_card_read;
  u32 m_card_bit;
  u32 m_card_shutter;
  u32 m_card_state_call_count;
  u8 m_card_offset;

  u32 m_wheelinit;

  u32 m_motorinit;
  u8 m_motorreply[64];
  s16 m_motorforce_x;

  // F-Zero AX (DX)
  u32 m_fzdx_seatbelt;
  u32 m_fzdx_motion_stop;
  u32 m_fzdx_sensor_right;
  u32 m_fzdx_sensor_left;
  u8 m_rx_reply;

  // F-Zero AX (CryCraft)
  u32 m_fzcc_seatbelt;
  u32 m_fzcc_sensor;
  u32 m_fzcc_emergency;
  u32 m_fzcc_service;

  void ICCardSendReply(ICCommand* iccommand, u8* buffer, u32* length);

public:
  // constructor
  CSIDevice_AMBaseboard(Core::System& system, SIDevices device, int device_number);

  // run the SI Buffer
  int RunBuffer(u8* buffer, int request_length) override;

  // return true on new data
  DataResponse GetData(u32& hi, u32& low) override;

  // send a command directly
  void SendCommand(u32 command, u8 poll) override;
};

}  // namespace SerialInterface
