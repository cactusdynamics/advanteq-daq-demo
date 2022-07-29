#include <bdaqctrl.h>
#include <spdlog/spdlog.h>

#include <bitset>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

using namespace Automation::BDaq;
using namespace std::chrono_literals;

// This very simple test program toggles the 4th channel of the 0th DO port every 2 seconds.

void PrintDaqError(ErrorCode ret) {
  wchar_t enum_str[256];
  AdxEnumToString(L"ErrorCode", static_cast<int32_t>(ret), 256, enum_str);
  // TODO: fix with SPDLOG_ERROR
  printf("Error 0x%X: %ls\n", ret, enum_str);
}

int main(int argc, const char* argv[]) {
#if SPDLOG_ACTIVE_LEVEL == 1
  spdlog::set_level(spdlog::level::debug);
#endif

  if (argc <= 1) {
    SPDLOG_ERROR("error: must specify device description in the format of USB-4704,BID#0 as the second argument");
    return 1;
  }

  auto     n = strlen(argv[1]);
  wchar_t* device_desc = new wchar_t[n + 1];
  std::mbstowcs(device_desc, argv[1], n);
  SPDLOG_DEBUG("Using device {}", argv[1]);

  // Create an DO control object in "instant mode"
  InstantDiCtrl*    instant_di_ctrl = InstantDiCtrl::Create();
  DeviceInformation dev_info{device_desc};

  // Set the selected device
  ErrorCode ret = instant_di_ctrl->setSelectedDevice(dev_info);
  if (BioFailed(ret)) {
    PrintDaqError(ret);
    return 1;
  }

  int   port_count = instant_di_ctrl->getPortCount();
  auto* feature = instant_di_ctrl->getFeatures();
  SPDLOG_DEBUG("This device has {} ports -> DO supported: {}, DI supported: {}", port_count, feature->getDoSupported(), feature->getDiSupported());

  // Initialize the DO port direction
  // Remember that a port is 8 channels, so this will set all 8 channels into output mode.
  // My USB-4704 doesn't allow output mode to be selected.
  // Array<DioPort>* ports = instant_di_ctrl->getPorts();
  // ret = ports->getItem(0).setDirectionMask(Input);
  // if (BioFailed(ret)) {
  //   PrintDaqError(ret);
  //   return 1;
  // }

  uint8_t state = 0;

  while (true) {
    state = (state + 1) % 2;
    uint8_t port_data = 0;

    ret = instant_di_ctrl->Read(0, port_data);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      return 1;
    }

    std::bitset<8> port_bits(port_data);
    SPDLOG_INFO("Read data: {} - {}", port_bits.to_string(), port_bits[0]);

    std::this_thread::sleep_for(300ms);
  }

  // Close and cleanup the device.
  // TODO: wrap this with RAII.
  instant_di_ctrl->Dispose();
  delete device_desc;
  return 0;
}
