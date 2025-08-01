// Copyright 2009 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <string>
#include <string_view>

#include "Common/CommonTypes.h"
#include "Common/SymbolDB.h"

namespace Core
{
class CPUThreadGuard;
}  // namespace Core

// This has functionality overlapping Debugger_Symbolmap. Should merge that stuff in here later.
class PPCSymbolDB : public Common::SymbolDB
{
public:
  PPCSymbolDB();
  ~PPCSymbolDB() override;

  const Common::Symbol* AddFunction(const Core::CPUThreadGuard& guard, u32 start_addr) override;
  void AddKnownSymbol(const Core::CPUThreadGuard& guard, u32 startAddr, u32 size,
                      const std::string& name, const std::string& object_name,
                      Common::Symbol::Type type = Common::Symbol::Type::Function);
  void AddKnownNote(u32 start_addr, u32 size, const std::string& name);

  const Common::Symbol* GetSymbolFromAddr(u32 addr) const override;
  bool NoteExists() const { return !m_notes.empty(); }
  const Common::Note* GetNoteFromAddr(u32 addr) const;
  void DetermineNoteLayers();
  void DeleteFunction(u32 start_address);
  void DeleteNote(u32 start_address);

  std::string GetDescription(u32 addr) const;

  void FillInCallers();

  bool LoadMapOnBoot(const Core::CPUThreadGuard& guard);
  bool LoadMap(const Core::CPUThreadGuard& guard, std::string filename, bool bad = false);
  bool SaveSymbolMap(const std::string& filename) const;
  bool SaveCodeMap(const Core::CPUThreadGuard& guard, const std::string& filename) const;

  void PrintCalls(u32 funcAddr) const;
  void PrintCallers(u32 funcAddr) const;
  void LogFunctionCall(u32 addr);

  static bool FindMapFile(std::string* existing_map_file, std::string* writable_map_file);

private:
  static void AddKnownSymbol(const Core::CPUThreadGuard& guard, u32 startAddr, u32 size,
                             const std::string& name, const std::string& object_name,
                             Common::Symbol::Type type, XFuncMap* functions,
                             XFuncPtrMap* checksum_to_function);
  static void AddKnownNote(u32 start_addr, u32 size, const std::string& name, XNoteMap* notes);

  static void DetermineNoteLayers(XNoteMap* notes);
};
