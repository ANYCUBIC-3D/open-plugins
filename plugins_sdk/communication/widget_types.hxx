// Copyright (c) 2026 深圳市纵维立方科技有限公司
// Open-Plugin is licensed under Mulan PSL v2.
// You can use this software according to the terms and conditions of the Mulan
// PSL v2.
// You may obtain a copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FI

#pragma once

#include <utility/json/reader.hxx>

#include <wx/wx.h>

uint32_t wxColourToRGBA(const wxColour &color);

template <typename stream>
void render_json_value(stream &ss, const wxColour &color) {
  uint32_t rgba = wxColourToRGBA(color);
  render_json_value(ss, rgba);
}
template <typename stream>
void render_json_value(stream &ss, const wxString &val) {
  render_json_value(ss, val.utf8_string());
}

template <typename stream, typename value>
void render_json_value(stream &ss, const std::map<int, value> &o) {
  using namespace iguana::json;
  ss.put('{');
  join(ss, o.cbegin(), o.cend(), ',', [&ss](const auto &jsv) {
    render_key(ss, "key");
    ss.put(':');
    render_json_value(ss, jsv.first);
    render_key(ss, "value");
    ss.put(':');
    render_json_value(ss, jsv.second);
  });
  ss.put('}');
}

void read_json(iguana::json::reader_t &rd, wxString &val, bool unorder = false);
void read_json(iguana::json::reader_t &rd, wxColour &val, bool unorder = false);

namespace Anycubic::communication {
template <typename desType, typename srcType>
inline void assign_sequence_container(desType &des, const srcType &src) {
  des.resize(src.size());
  std::copy(src.begin(), src.end(), des.begin());
}
template <typename srcType> auto transform(const srcType &src) {
  if constexpr (iguana::is_template_instant_of<wxVector, srcType>::value) {
    std::vector<typename std::decay_t<decltype(src)>::value_type> ret;
    assign_sequence_container(ret, src);
    return ret;
  } else {
    wxVector<typename std::decay_t<decltype(src)>::value_type> ret;
    assign_sequence_container(ret, src);
    return ret;
  }
};
} // namespace Anycubic::communication

enum RunCallTestModel {
  r_upload = 0,
  r_print = 1,
  r_calculatePrinter = 2,
};
struct FileUploadObj {
  bool isCloud{true};
  std::vector<int> processList;
  std::vector<wxString> errList;
};

struct RemotePrintObj {
  bool succeed = false;
  bool isLANPrint = false;
  wxString printer_id;
  wxString result_string;
  wxString device_id;
  wxString task_id;
};

struct OpAmsChange {
  wxString deviceID;
  int countIndex;
  int slotNum;
  wxString slotFilament;
};

struct AmsSlotObj {
  int count;
  int slotName;
  wxString filament;
  wxColour filamentColor;
  int sourceBoxId{-2};
};

struct FramGroupSendTaskObj {

  bool is_start_print{false};
  int print_number{1};
  int work_project_group_id{0};
  int bulk_id{0};
  int empty_group_id{0};
  std::vector<int> printer_ids;
};

struct intDoubleMap {
  int slotInt;
  double slotDouble;
};

struct AmsLinkMappingObj {
  int num;
  wxString filamentName;
  wxColour filamentColor;
  std::vector<intDoubleMap> slotInfoMap;
};

struct GcodeFilamentColor {
  int num;
  int sloop;
  wxString filament;
  wxColour filamentColor;
  wxString weight;
};

struct ValueComparator {
  bool operator()(const int &a, const int &b) const {
    return mapRef->at(a).slotInt < mapRef->at(b).slotInt;
  }
  std::vector<intDoubleMap> *mapRef;
};

class ACScrolledNoFocusWindow : public wxScrolledWindow {
public:
  ACScrolledNoFocusWindow(wxWindow *parent, wxWindowID id, const wxPoint &pos,
                          const wxSize &size, long style)
      : wxScrolledWindow(parent, id, pos, size, style) {}
  ~ACScrolledNoFocusWindow() = default;

protected:
  virtual bool ShouldScrollToChildOnFocus(wxWindow *child) override {
    return false;
  }
};

class ACZipThread : public wxThread {
public:
  ACZipThread(wxEvtHandler *handler, wxString fileIndex, bool isLan,
              wxString infoStr = "");

protected:
  virtual ExitCode Entry() override;

private:
  wxEvtHandler *m_handler;
  wxString m_fileIndex;
  bool m_isLan;
  wxString m_infoStr;
};

struct CloudSlicerInfoObj_FilamentColor {
  wxString material_type;
  std::vector<int> color;
};

struct ModelSlicerInfo {
  bool isGcode{false};
  bool isCloudStart{false};
  float print_time_f;
  int machine_type;
  int fileID{-1};
  double filamentWeight_d;
  wxString gcodeName;
  wxString imgBase64;
  wxString print_time;
  wxString used_filament;
  wxString modleLayers;
  wxString filament_length;
  wxString filament_type;
  wxString printerName;
  std::vector<int> filament_color;
  std::vector<unsigned int> extruder_idsList;
  std::vector<wxString> filamentColors_slicer;
  std::vector<wxString> filamentTypess_slicer;
  std::vector<wxString> filament_types_gcode;
  std::vector<wxString> extruder_colors_gcode;
  std::vector<GcodeFilamentColor> gcodeFilamentColorList;
  std::vector<CloudSlicerInfoObj_FilamentColor> filamentSlicerList;
};

struct CloudSlicerInfoObj {
  int openType{0}; // 0: print 1:framtask
  int machine_type;
  int estimate;
  int total_layers;
  double used_filament;
  double length_filament;
  wxString machine_name;
  wxString thumbnail;
  wxString name;
  wxString ams_info;
  wxString print_time;
  std::vector<CloudSlicerInfoObj_FilamentColor> filament_color;
};

struct R_BatchResult {
  int32_t estimate;
  int32_t total_layers;
  double supplies_usage;
  wxString print_time;
};

struct ZipFinishObj {
  bool isLan{false};
  wxString gcodeName;
  wxString filePath;
  wxString baseUrl;
};

struct FramGroupCloudObj {
  int id;
  int user_id;
  int print_number;
  int status;
  wxString name;
};

struct FramGroupCloudList {
  std::vector<FramGroupCloudObj> g_framGroupCloudList;
};

struct WebWakeUpObj {
  bool regionCn{false};
  bool prod{false};
  int fileId{-1};
  int fileType{-1};
  wxString accessToken;
  wxString hash;
  wxString fileName;
  wxString userId;
};

struct LANSendFileResultObj {
  bool succeed{false};
  wxString resultStr;
  wxString deviceID;
};

struct LANInfoObj {
  bool auto_leveling_support{false};
  bool vibration_compensation_support{false};
  bool flow_calibration_support{false};
  bool drying_first_support{false};
  bool camera_timelapse{false};
  bool gcode_3mf_support{false};
  wxString gcodeName;
  wxString filePath;
  wxString baseUrl;
  wxString deviceID;

public:
  bool isQualInfo(LANInfoObj &obj);
};

struct LANInfoObjList {
  std::vector<LANInfoObj> lanInfoObjList;
};

struct Peripherie {
  int camera{-1};
  int multiColorBox{-1};
  int udisk{-1};

public:
  bool isQualInfo(Peripherie &obj);
};

struct CPrintOptions {
  wxString function_name;
  wxString function_des;
  wxString name;
};

struct DryingStateObj {
  bool isEnable{false};
  int selectIndex{-1};
  wxString deviceID;
  wxString dryTemp;
  wxString drtTime;

public:
  void Clear() {
    deviceID.clear();
    selectIndex = -1;
    dryTemp.clear();
    drtTime.clear();
    isEnable = false;
  }
};

struct CPrinterFunctions {
  bool ai{false};
  bool photography{false};
  bool leveling{true};
  bool resonance{false};
  bool dryEnable{false};
  bool flowCalibration{false};
};

struct CPrintOptionsResponse {
  int32_t machine_type;
  int32_t padding;
  wxString name;
  std::vector<CPrintOptions> optionList;
};

struct CPrintTaskResponse {
  int id;
  int taskid;
  int user_id;
  int printer_id;
  int gcode_id;
  int model;
  int estimate;
  int pause;
  int progress;
  int print_status;
  int slice_status;
  int project_type;
  int create_time;
  int end_time;
  int machine_type;
  int padding;
  wxString img;
  wxString reason;
  wxString material;
  wxString localtask;
  wxString source;
  wxString key;
  wxString type;
  wxString printer_name;
  wxString machine_name;
  wxString gcode_name;
};

struct FilamentInfoObj {
  wxColour numColor;
  wxColour numTextColor;
  wxString filament_type;
};

struct ColorBoxAndName {
  int slotNum;
  int sourceBoxId{-2};
  wxString filament_name;
  wxColour filamentColorInfo = wxColour(255, 255, 255, 255);
  wxColour textColorInfo = wxColour(38, 38, 38, 255);
};

struct PrinterSelectObj {
  int filamentNum;
  wxString filament;
  wxColour filamentColor;
  int slotNum;
  wxColour slotColor;
  wxString slotFilament;
  int sloopIndex;
  int sourceBoxId{-2};

public:
  static bool CompareByIntNum(const PrinterSelectObj &obj1,
                              const PrinterSelectObj &obj2) {
    return obj1.filamentNum < obj2.filamentNum;
  }
};

struct AmsSlotObjInfo {
  int slotNum;
  int iconType{0};
  int sourceBoxId{-2};
  float capacityGap{0.0f};
  wxString filament_type;
  wxColour filamentColorInfo;
  wxColour textColorInfo;
  wxString sku;
  wxString filament_type_sub;
  std::vector<wxColour> skuColors;

public:
  bool isQualInfo(AmsSlotObjInfo &obj);
};

struct AmsBoxObj {
  bool isEnable{true};
  int id{0};
  std::vector<AmsSlotObjInfo> slotInfo;
  std::vector<int> m_ids;

public:
  bool isQualInfo(AmsBoxObj &obj);
  static bool CompareByIntNum(const AmsBoxObj &obj1, const AmsBoxObj &obj2) {
    return obj1.id < obj2.id;
  }
};

struct COTAVersion {
  bool isSupport{false};
  int need_update;
  int time_cost;
  int force_update;
  int panding;
  wxString firmware_version;
  wxString update_desc;
  wxString target_version;

public:
  bool isQualInfo(COTAVersion &obj);
};

struct PrinterOptions {
  bool auto_leveling_support{false};
  bool vibration_compensation_support{false};
  bool flow_calibration_support{false};
  bool drying_first_support{false};
  bool camera_timelapse_support{false};
  bool gcode_3mf_support{false};

public:
  bool isQualInfo(PrinterOptions &obj);
};

struct PrinterObj {
  bool is_lan{false};
  bool printer_IncludeAmsBox{false};
  int printer_Type; // 2:k2 3:k3
  int printer_id;
  int printer_State; // 0:offline 1.free 2.buy
  int machine_type;
  wxString printer_type_str;
  wxString description;
  wxString deviceID;
  wxString printer_Type_str;
  wxString ip;
  wxString uuid;
  wxString url;
  wxString username;
  wxString password;
  wxString clientid;
  wxString ca;
  wxString cert;
  wxString key;
  wxString label_name;
  wxString printer_Name;
  COTAVersion version;
  Peripherie peripherie;
  PrinterOptions options;

public:
  bool isQualInfo(PrinterObj &obj);
};

struct SendAmsInfoObj {
  std::vector<AmsBoxObj> m_backAmsBoxList;
  std::vector<int> m_ids;
};

struct CloudClientOpObj {
  int type;
  int state;
  int slotNum;
  int boxId;
  wxString deviceID;
  Peripherie peripherie;
  AmsSlotObjInfo newObj;
  std::vector<PrinterObj> objList;
  std::vector<AmsBoxObj> info;
};

struct LANSendRemoteEventDataObjMap {
  wxString dryingStateObjMapStr;
  DryingStateObj dryingStateObjMapObj;
};

struct LANSendRemoteEventDataObj {
  wxString deviceID;
  std::vector<PrinterSelectObj> printerAmsInfoList;
  std::vector<LANSendRemoteEventDataObjMap> dryingStateObjMap;
  std::vector<int> functionsList;
};

struct FramGroupInfo {

  bool g_isSelect{false};
  int printer_id{-1};
  wxString g_deviceID;
  wxString g_printerName;
  wxString g_state;
  wxString g_groupNum;
  wxString g_modelName;
  wxString g_linkeType;
  std::vector<AmsSlotObjInfo> g_slotInfoList;
};
struct FramGroupListInfo {
  std::vector<FramGroupInfo> g_framGroupList;
};

struct SendLanPrinterObj {
  wxString gcodeName;
  wxString filePath;
  wxString baseUrl;
  wxString deviceID;
};

struct SendFileObj {
  wxString time;
  wxString weight;
  wxString layer;
  wxString imgStr;
  wxString printerName;
  std::vector<GcodeFilamentColor> colorInfoList;
};
struct SendFileSumObjMap {
  int parteInfoMapInt;
  SendFileObj parteInfoMapObj;
};

struct SendFileSumObj {
  wxString timeSum;
  wxString weightSum;
  wxString parteSum;
  wxString printerName;
  wxString imgStr;
  std::vector<GcodeFilamentColor> colorInfoListSum;
  std::vector<SendFileSumObjMap> parteInfoMap;
};

struct NewVersionParObj {
  int id;
  int versionCode;
  int draft;
  int del_flag;
  wxString param_version;
  wxString data_url;
  wxString update_desc_cn;
  wxString update_desc_en;
  wxString create_time;
  wxString update_time;
};

struct UpAndDownResultObj {
  int type{0}; // 0:up 1:down
  bool result{true};
  wxString contentStr;
};

struct ZipFolderObj {
  bool isShowDialog{false};
  wxString folderPath;
  wxString zipFilePath;
};

struct UnZipFolderObj {
  bool isShowDialog{false};
  wxString zipFilePath;
  wxString outputDir;
};

struct PostDownLoadObj {
  int id;
  wxString url;
  wxString filePath;
  wxString fileName;
  wxString md5;
};

struct PostUpLoadObj {
  int id;
  wxString filePath;
  wxString fileName;
  wxString md5;
};

REFLECTION(FramGroupInfo, g_isSelect, printer_id, g_deviceID, g_printerName,
           g_state, g_groupNum, g_modelName, g_linkeType, g_slotInfoList)
REFLECTION(FramGroupListInfo, g_framGroupList)
REFLECTION(R_BatchResult, estimate, total_layers, supplies_usage, print_time)
REFLECTION(CloudSlicerInfoObj, openType, machine_type, machine_name, thumbnail,
           name, ams_info, filament_color, print_time, estimate, total_layers,
           used_filament, length_filament)
REFLECTION(GcodeFilamentColor, num, filament, filamentColor, sloop, weight)
REFLECTION(AmsLinkMappingObj, num, filamentName, filamentColor, slotInfoMap)
REFLECTION(intDoubleMap, slotInt, slotDouble)
REFLECTION(AmsSlotObj, count, slotName, filament, filamentColor, sourceBoxId)
REFLECTION(OpAmsChange, deviceID, countIndex, slotNum, slotFilament)
REFLECTION(RemotePrintObj, succeed, printer_id, result_string, device_id,
           task_id, isLANPrint)
REFLECTION(FileUploadObj, isCloud, processList, errList)
REFLECTION(CloudSlicerInfoObj_FilamentColor, material_type, color)
REFLECTION(ModelSlicerInfo, fileID, isCloudStart, gcodeName, imgBase64,
           print_time, used_filament, filamentWeight_d, print_time_f,
           modleLayers, filament_length, filament_color, machine_type,
           printerName, isGcode, extruder_idsList, filamentColors_slicer,
           filamentTypess_slicer, filament_types_gcode, extruder_colors_gcode,
           gcodeFilamentColorList, filamentSlicerList)
REFLECTION(FramGroupSendTaskObj, is_start_print, print_number,
           work_project_group_id, bulk_id, empty_group_id, printer_ids)
REFLECTION(AmsSlotObjInfo, slotNum, iconType, sourceBoxId, capacityGap,
           filament_type, filamentColorInfo, textColorInfo, sku,
           filament_type_sub, skuColors)
REFLECTION(AmsBoxObj, isEnable, id, slotInfo, m_ids)
REFLECTION(FramGroupCloudList, g_framGroupCloudList)
REFLECTION(FramGroupCloudObj, id, user_id, print_number, status, name)
REFLECTION(LANInfoObj, auto_leveling_support, vibration_compensation_support,
           flow_calibration_support, drying_first_support, camera_timelapse,
           gcode_3mf_support, gcodeName, filePath, baseUrl, deviceID)
REFLECTION(CPrintOptions, function_name, function_des, name)
REFLECTION(CPrintOptionsResponse, machine_type, padding, name, optionList)
REFLECTION(DryingStateObj, isEnable, selectIndex, deviceID, dryTemp, drtTime)
REFLECTION(PrinterSelectObj, filamentNum, slotNum, sloopIndex, sourceBoxId,
           filamentColor, slotColor, slotFilament, filament)
REFLECTION(PrinterObj, is_lan, printer_IncludeAmsBox, printer_Type, printer_id,
           printer_State, machine_type, printer_type_str, description, deviceID,
           printer_Type_str, ip, uuid, url, username, password, clientid, ca,
           cert, key, label_name, printer_Name, version, peripherie, options)
REFLECTION(PrinterOptions, auto_leveling_support,
           vibration_compensation_support, flow_calibration_support,
           drying_first_support, camera_timelapse_support, gcode_3mf_support)
REFLECTION(Peripherie, camera, multiColorBox, udisk)
REFLECTION(COTAVersion, isSupport, need_update, time_cost, force_update,
           panding, firmware_version, update_desc, target_version)
REFLECTION(SendAmsInfoObj, m_backAmsBoxList, m_ids)
REFLECTION(LANSendRemoteEventDataObjMap, dryingStateObjMapStr,
           dryingStateObjMapObj)
REFLECTION(LANSendRemoteEventDataObj, deviceID, printerAmsInfoList,
           dryingStateObjMap, functionsList)

REFLECTION(LANInfoObjList, lanInfoObjList)
