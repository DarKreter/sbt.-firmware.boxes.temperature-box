//
// Created by jazon on 10/27/21.
//

#ifndef F1XX_PROJECT_TEMPLATE_CANTEST_HPP
#define F1XX_PROJECT_TEMPLATE_CANTEST_HPP

#include "Task.hpp"
#include "CommCAN.h"

class CanTest : public SBT::System::PeriodicTask {
public:
  CanTest();
  void initialize() override;
  void run() override;
};

#endif //F1XX_PROJECT_TEMPLATE_CANTEST_HPP
