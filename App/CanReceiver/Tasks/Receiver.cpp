#include "Receiver.hpp"
#include "CommCAN.hpp"
#include "UART.hpp"

using namespace SBT::Hardware;
using namespace SBT::System::Comm;
using namespace SBT::System::Comm::CAN_ID;

// Setting periodicity to 5s(this could be any value, because everything here
// happens inside callbacks)
Receiver::Receiver() : PeriodicTask("Receiver", 2, 5000, 256) {}

// First callback being normal function with proper argument
void Callback1(SBT::System::Comm::CAN::RxMessage mess) {
  // Filter to this callback was set to SourceID, so now we must identify which
  // ParamID is this.
  if (mess.GetMessageID().paramID == CAN_ID::Param::LIFEPO4_CELLS_1) {
    // To unpack data from raw payload we use Unpack_{ParamID} functions. They
    // get payload by argument and returns proper structure
    LIFEPO4_CELLS_1_t poo = Unpack_LIFEPO4_CELLS_1(mess.GetPayload());

    // After unpacking we can do whatever we want. In this example values are
    // send via UART.
    uart1.printf("[F1] P: %d, Dev: %d, Param: %d, G: %d, cellVoltage1: %d, "
                 "cellVoltage2: %d, cellVoltage3: %d, "
                 "cellVoltage4: %d, cellVoltage5: %d\n",
                 mess.GetMessageID().priority, mess.GetSourceID(),
                 mess.GetMessageID().paramID, mess.GetMessageID().group,
                 poo.cellVoltage1, poo.cellVoltage2, poo.cellVoltage3,
                 poo.cellVoltage4, poo.cellVoltage5);
    // Similarly for second ParamID
  } else if (mess.GetMessageID().paramID == CAN_ID::Param::LIFEPO4_GENERAL) {
    LIFEPO4_GENERAL_t poo = Unpack_LIFEPO4_GENERAL(mess.GetPayload());

    uart1.printf("[F1] P: %d, Dev: %d, Param: %d, G: %d, percentage: %d, "
                 "voltage: %d, dischargingCurrent: %d, "
                 "chargeCurrent: %d, state: %d\n",
                 mess.GetMessageID().priority, mess.GetSourceID(),
                 mess.GetMessageID().paramID, mess.GetMessageID().group,
                 poo.percentage, poo.voltage, poo.dischargingCurrent,
                 poo.chargeCurrent, poo.state);
  } else { // Some undefined frame
    uart1.printf("[F1][Undefined] P: %d, Dev: %d, Param: %d, G: %d\n",
                 mess.GetMessageID().priority, mess.GetSourceID(),
                 mess.GetMessageID().paramID, mess.GetMessageID().group);
  }
}
// Second callback is non-static class method. It also need relevant argument
void Receiver::Callback2(SBT::System::Comm::CAN::RxMessage mess) {
  // Here happens exactly this same what inside first callback
  if (mess.GetMessageID().paramID == CAN_ID::Param::PV_DATA) {
    PV_DATA_t poo = Unpack_PV_DATA(mess.GetPayload());

    uart1.printf("[F2] P: %d, Dev: %d, Param: %d, G: %d, panelCurrent: %d, "
                 "panelPower: %d, panelVoltage: %d, \n",
                 mess.GetMessageID().priority, mess.GetSourceID(),
                 mess.GetMessageID().paramID, mess.GetMessageID().group,
                 poo.panelCurrent, poo.panelPower, poo.panelVoltage);
  } else {
    uart1.printf("[F2][Undefined] P: %d, Dev: %d, Param: %d, G: %d\n",
                 mess.GetMessageID().priority, mess.GetSourceID(),
                 mess.GetMessageID().paramID, mess.GetMessageID().group);
  }
}

void Receiver::initialize() {
  // Initialize UART in blocking mode with timeout set to 500ms
  uart1.ChangeModeToBlocking(500);
  // Initialize uart1
  uart1.Initialize();

  // We can filter by SourceID and provide standard function as callback
  CAN::AddFilter(SBT::System::Comm::CAN::Filter(CAN_ID::Source::LIFEPO4_1),
                 Callback1);

  // Filtering can also be configured by group ID
  //     CAN::AddFilter(SBT::System::Comm::CAN::Filter(CAN_ID::Group::LIFEPO4_DATA),
  //                    Callback1);

  // We can also provide static class method as callback
  CAN::AddFilter(SBT::System::Comm::CAN::Filter(CAN_ID::Source::MPPT_1), this,
                 &Receiver::Callback2);

  // Or even lambda expression, but this time we are filtering by ParamID
  auto Callback3 = [&](SBT::System::Comm::CAN::RxMessage mess) {
    if (mess.GetMessageID().paramID == CAN_ID::Param::LIFEPO4_CELLS_1) {
      LIFEPO4_CELLS_1_t poo = Unpack_LIFEPO4_CELLS_1(mess.GetPayload());

      uart1.printf("[F3] P: %d, Dev: %d, Param: %d, G: %d, cellVoltage1: %d, "
                   "cellVoltage2: %d, cellVoltage3: %d, "
                   "cellVoltage4: %d, cellVoltage5: %d\n",
                   mess.GetMessageID().priority, mess.GetSourceID(),
                   mess.GetMessageID().paramID, mess.GetMessageID().group,
                   poo.cellVoltage1, poo.cellVoltage2, poo.cellVoltage3,
                   poo.cellVoltage4, poo.cellVoltage5);
    } else {
      uart1.printf("[F3][Undefined] P: %d, Dev: %d, Param: %d, G: %d\n",
                   mess.GetMessageID().priority, mess.GetSourceID(),
                   mess.GetMessageID().paramID, mess.GetMessageID().group);
    }
  };
  CAN::AddFilter(SBT::System::Comm::CAN::Filter(CAN_ID::Param::LIFEPO4_CELLS_1),
                 Callback3);
}

// As all receiving stuff takes place in callback run can be empty
void Receiver::run() {}
