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
  // There's a hardware jumper on the USB-4704 that controls if the channels are
  // in single-ended or differential mode. You cannot set a single pair of
  // channels to be in differential mode.
  // From the docs about AI Channel Types:
  // > In general, the ChannelType property value of the selected device will be
  // > one of the AiChannelType enumeration, but there are some special cases, for
  // > example ChannelType property value of device USB-4702 and USB-4704 is
  // > decided by the hardware jumper, it could be AllSingleEnded or
  // > AllDifferential, if it is AllSingleEnded, all the channel can only be set
  // > to single-ended and can't change to be differential by programming.  Please
  // > note that the case of USB-4702 and USB-4704 is different from the
  // > conditions that the ChannelType property value of the selected device
  // > equals to AllSeDiffAdj, the latter means all the AI channels can be set to
  // > single-ended at the same time and can change all the AI channels to be
  // > differential by programming.
  //
  // However, according to the 4704 manual, there are no jumpers?
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

  for (int i = 0; i < channels->getCount(); ++i) {
    ret = channels->getItem(i).setSignalType(Differential);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      exit(1);
    }
  }
  // // Let's set channel 2 and 3 to differential:
  // // This is not allowed on my USB-4704
  // ret = channels->getItem(2).setSignalType(Differential);
  // if (BioFailed(ret)) {
  //   PrintDaqError(ret);
  //   return 1;
  // }

  int16_t data_raw[2] = {0};
  double  data_scaled[2] = {0};

  while (true) {
    ret = instant_ai_ctrl->Read(2, 2, data_raw, data_scaled);
    if (BioFailed(ret)) {
      PrintDaqError(ret);
      return 1;
    }

    SPDLOG_INFO("Read data from channel 2 and 3: {} {} (raw), {} {} (scaled)", data_raw[0], data_raw[1], data_scaled[0], data_scaled[1]);

    std::this_thread::sleep_for(300ms);
  }

  // uint8_t state = 0;

  // while (true) {
  //   state = (state + 1) % 2;
  //   uint8_t port_data = 0;

  //   ret = instant_ai_ctrl->Read(0, port_data);
  //   if (BioFailed(ret)) {
  //     PrintDaqError(ret);
  //     return 1;
  //   }

  //   std::bitset<8> port_bits(port_data);
  //   SPDLOG_INFO("Read data: {} - {}", port_bits.to_string(), port_bits[0]);

  //   std::this_thread::sleep_for(300ms);
  // }

  // Close and cleanup the device.
  // TODO: wrap this with RAII.
  instant_ai_ctrl->Dispose();
  delete device_desc;
  return 0;
}
