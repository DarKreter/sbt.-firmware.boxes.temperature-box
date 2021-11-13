//
// Created by jazon on 10/27/21.
//

#include "canTest.hpp"
#include "CommCAN.h"
#include "Hardware.hpp"

CanTest::CanTest() : PeriodicTask({}, 2, 1000) {}

void CanTest::initialize() {
  using namespace SBT::System;
  Communication::CAN::Init(SBT::System::Communication::CANBoardID::PiBOX);
}

void CanTest::run() {
  using namespace SBT::System;
  Communication::CAN::Send(Communication::CANParameterID::HeartBeat,
                           HAL_GetTick());
}
