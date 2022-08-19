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

  if (argc <= 2) {
    SPDLOG_ERROR("error: must specify device description in the format of USB-4704,BID#0 as the second argument");
    return 1;
  }

  auto     n = strlen(argv[1]);
  wchar_t* device_desc = new wchar_t[n + 1];
  std::mbstowcs(device_desc, argv[1], n);

  bool differential = std::string(argv[2]) == "differential";

  SPDLOG_DEBUG("Using device {} (differential: {})", argv[1], differential);

  // Create an AI control object in "instant mode"
  InstantAiCtrl*    instant_ai_ctrl = InstantAiCtrl::Create();
  DeviceInformation dev_info{device_desc};

  // Set the selected device
  ErrorCode ret = instant_ai_ctrl->setSelectedDevice(dev_info);
  if (BioFailed(ret)) {
    PrintDaqError(ret);
    return 1;
  }

  auto data_size = instant_ai_ctrl->getFeatures()->getDataSize();
  SPDLOG_INFO("The analog input of this device has a data size of {}.", data_size);

  Array<AiChannel>* channels = instant_ai_ctrl->getChannels();
  SPDLOG_INFO("This device has {} analog channels:", channels->getCount());
  for (int i = 0; i < channels->getCount(); ++i) {
    auto& channel = channels->getItem(i);
    SPDLOG_INFO(
      " - Channel {} (logical {}) -> value range: {}; signal type: {}; ",
      channel.getChannel(),
      channel.getLogicalNumber(),
      channel.getValueRange(),   // Value of 1 is -10 to 10V
      channel.getSignalType());  // Value of 0 is Single ended.
  }

  auto signal_type = differential ? Differential : SingleEnded;
  auto value_range = V_Neg10To10;

  // Setting the signal type and value range
  for (int i = 0; i < channels->getCount(); ++i) {
    ret = channels->getItem(i).setSignalType(signal_type);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      exit(1);
    }

    ret = channels->getItem(i).setValueRange(value_range);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      exit(1);
    }
  }

  // Verify the signal types are set
  SPDLOG_INFO("After setting signal type, this device has {} analog channels:", channels->getCount());

  channels = instant_ai_ctrl->getChannels();  // reget the channel, but there's memory leak problem as the previous channels is not cleaned up..
  for (int i = 0; i < channels->getCount(); ++i) {
    auto& channel = channels->getItem(i);
    printf("- Channel %d -> value range: %d; signal type: %d\n", channel.getChannel(), channel.getValueRange(), channel.getSignalType());
  }

  int16_t data_raw[8] = {0};
  double  data_scaled[8] = {0};

  // Read data for 10 seconds
  for (int i = 0; i < 50; ++i) {
    ret = instant_ai_ctrl->Read(0, 8, data_raw, data_scaled);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      return 1;
    }

    SPDLOG_INFO(
      "Read data {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} {:.2f} (scaled)",
      data_scaled[0],
      data_scaled[1],
      data_scaled[2],
      data_scaled[3],
      data_scaled[4],
      data_scaled[5],
      data_scaled[6],
      data_scaled[7]);

    std::this_thread::sleep_for(200ms);
  }

  instant_ai_ctrl->Dispose();
  delete[] device_desc;
  return 0;
}
