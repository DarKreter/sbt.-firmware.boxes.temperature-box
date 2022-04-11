#ifndef SENDER_HPP
#define SENDER_HPP

#include "TaskManager.hpp"

/**
 * @brief This task is designed to send four messages alternately to CAN bus.
 * Each message has different values and different arbitration ID.
 */
struct Sender : public SBT::System::PeriodicTask {
  // The constructor of the task
  Sender();
  // initialize() will be called once when task is created
  void initialize() override;
  // run() will be called repeatedly
  void run() override;

  // Sending data buffer
  uint8_t data[8];
  // iterator
  uint8_t i;
};

#endif // SENDER_HPP
