// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FI

#include "widget_types.hxx"

uint32_t wxColourToRGBA(const wxColour &color) {
  uint32_t r = static_cast<uint8_t>(color.Red());
  uint32_t g = static_cast<uint8_t>(color.Green());
  uint32_t b = static_cast<uint8_t>(color.Blue());
  uint32_t a = static_cast<uint8_t>(color.Alpha());
  return (a << 24) | (r << 16) | (g << 8) | b;
}
static wxColor wxColourFromRGBA(uint32_t rgba) {
  uint8_t a = (rgba >> 24) & 0xFF;
  uint8_t b = (rgba >> 8) & 0xFF;
  uint8_t g = (rgba >> 16) & 0xFF;
  uint8_t r = rgba & 0xFF;
  return wxColour(r, g, b, a);
}

void read_json(iguana::json::reader_t &rd, wxString &val, bool unorder) {
  std::string str;
  read_json(rd, str);
  val = wxString::FromUTF8(str.c_str());
}
void read_json(iguana::json::reader_t &rd, wxColour &val, bool unorder) {
  uint32_t rgba;
  read_json(rd, rgba);
  val = wxColourFromRGBA(rgba);
}
bool LANInfoObj::isQualInfo(LANInfoObj &obj) {
  if (auto_leveling_support != obj.auto_leveling_support)
    return false;
  if (vibration_compensation_support != obj.vibration_compensation_support)
    return false;
  if (flow_calibration_support != obj.flow_calibration_support)
    return false;
  if (drying_first_support != obj.drying_first_support)
    return false;
  if (camera_timelapse != obj.camera_timelapse)
    return false;
  if (gcode_3mf_support != obj.gcode_3mf_support)
    return false;

  return true;
}

bool PrinterObj::isQualInfo(PrinterObj &obj) {
  if (printer_Name != obj.printer_Name)
    return false;
  if (printer_Type != obj.printer_Type)
    return false;
  if (printer_id != obj.printer_id)
    return false;
  if (printer_State != obj.printer_State)
    return false;
  if (printer_IncludeAmsBox != obj.printer_IncludeAmsBox)
    return false;
  if (machine_type != obj.machine_type)
    return false;

  if (is_lan != obj.is_lan)
    return false;
  if (printer_type_str != obj.printer_type_str)
    return false;
  if (description != obj.description)
    return false;
  if (deviceID != obj.deviceID)
    return false;
  if (printer_Type_str != obj.printer_Type_str)
    return false;
  if (ip != obj.ip)
    return false;

  if (uuid != obj.uuid)
    return false;
  if (url != obj.url)
    return false;
  if (username != obj.username)
    return false;
  if (password != obj.password)
    return false;
  if (clientid != obj.clientid)
    return false;
  if (ca != obj.ca)
    return false;

  if (cert != obj.cert)
    return false;
  if (key != obj.key)
    return false;
  if (label_name != obj.label_name)
    return false;
  if (!version.isQualInfo(obj.version))
    return false;
  if (!peripherie.isQualInfo(obj.peripherie))
    return false;
  if (!options.isQualInfo(obj.options))
    return false;

  return true;
}

bool PrinterOptions::isQualInfo(PrinterOptions &obj) {

  if (auto_leveling_support != obj.auto_leveling_support)
    return false;
  if (vibration_compensation_support != obj.vibration_compensation_support)
    return false;
  if (flow_calibration_support != obj.flow_calibration_support)
    return false;
  if (drying_first_support != obj.drying_first_support)
    return false;
  if (camera_timelapse_support != obj.camera_timelapse_support)
    return false;
  if (gcode_3mf_support != obj.gcode_3mf_support)
    return false;

  return true;
}

bool COTAVersion::isQualInfo(COTAVersion &obj) {
  if (need_update != obj.need_update)
    return false;
  if (time_cost != obj.time_cost)
    return false;
  if (panding != obj.panding)
    return false;
  if (firmware_version != obj.firmware_version)
    return false;
  if (update_desc != obj.update_desc)
    return false;
  if (target_version != obj.target_version)
    return false;

  return true;
}

bool Peripherie::isQualInfo(Peripherie &obj) {
  if (camera != obj.camera)
    return false;
  if (multiColorBox != obj.multiColorBox)
    return false;
  if (udisk != obj.udisk)
    return false;

  return true;
}

bool AmsSlotObjInfo::isQualInfo(AmsSlotObjInfo &obj) {
  if (slotNum != obj.slotNum)
    return false;
  if (filament_type != obj.filament_type)
    return false;
  if (filamentColorInfo != obj.filamentColorInfo)
    return false;
  if (filament_type_sub != obj.filament_type_sub)
    return false;
  if (iconType != obj.iconType)
    return false;
  if (filament_type_sub != obj.filament_type_sub)
    return false;
  if (skuColors.size() != obj.skuColors.size())
    return false;
  int nowSize = static_cast<int>(skuColors.size());
  for (int i = 0; i < nowSize; i++) {
    if (skuColors[i] != obj.skuColors[i]) {
      return false;
    }
  }

  return true;
}

bool AmsBoxObj::isQualInfo(AmsBoxObj &obj) {
  if (isEnable != obj.isEnable) {
    return false;
  }
  if (slotInfo.size() != obj.slotInfo.size())
    return false;

  int nowSize = static_cast<int>(slotInfo.size());
  for (int i = 0; i < nowSize; i++) {
    if (!slotInfo[i].isQualInfo(obj.slotInfo[i])) {
      return false;
    }
  }

  return true;
}
