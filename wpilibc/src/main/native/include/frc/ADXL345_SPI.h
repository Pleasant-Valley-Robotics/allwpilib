// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>
#include <networktables/NTSendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/SPI.h"

namespace frc {

/**
 * ADXL345 Accelerometer on SPI.
 *
 * This class allows access to an Analog Devices ADXL345 3-axis accelerometer
 * via SPI. This class assumes the sensor is wired in 4-wire SPI mode.
 */
class ADXL345_SPI : public nt::NTSendable,
                    public wpi::SendableHelper<ADXL345_SPI> {
 public:
  enum Range { kRange_2G = 0, kRange_4G = 1, kRange_8G = 2, kRange_16G = 3 };

  enum Axes { kAxis_X = 0x00, kAxis_Y = 0x02, kAxis_Z = 0x04 };

  struct AllAxes {
    double XAxis;
    double YAxis;
    double ZAxis;
  };

  /**
   * Constructor.
   *
   * @param port  The SPI port the accelerometer is attached to
   * @param range The range (+ or -) that the accelerometer will measure
   */
  explicit ADXL345_SPI(SPI::Port port, Range range = kRange_2G);

  ~ADXL345_SPI() override = default;

  ADXL345_SPI(ADXL345_SPI&&) = default;
  ADXL345_SPI& operator=(ADXL345_SPI&&) = default;

  SPI::Port GetSpiPort() const;

  /**
   * Set the measuring range of the accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *     accelerometer will measure.
   */
  void SetRange(Range range);

  /**
   * Returns the acceleration along the X axis in g-forces.
   *
   * @return The acceleration along the X axis in g-forces.
   */
  double GetX();

  /**
   * Returns the acceleration along the Y axis in g-forces.
   *
   * @return The acceleration along the Y axis in g-forces.
   */
  double GetY();

  /**
   * Returns the acceleration along the Z axis in g-forces.
   *
   * @return The acceleration along the Z axis in g-forces.
   */
  double GetZ();

  /**
   * Get the acceleration of one axis in Gs.
   *
   * @param axis The axis to read from.
   * @return Acceleration of the ADXL345 in Gs.
   */
  virtual double GetAcceleration(Axes axis);

  /**
   * Get the acceleration of all axes in Gs.
   *
   * @return An object containing the acceleration measured on each axis of the
   *         ADXL345 in Gs.
   */
  virtual AllAxes GetAccelerations();

  void InitSendable(nt::NTSendableBuilder& builder) override;

 protected:
  SPI m_spi;

  hal::SimDevice m_simDevice;
  hal::SimEnum m_simRange;
  hal::SimDouble m_simX;
  hal::SimDouble m_simY;
  hal::SimDouble m_simZ;

  static constexpr int kPowerCtlRegister = 0x2D;
  static constexpr int kDataFormatRegister = 0x31;
  static constexpr int kDataRegister = 0x32;
  static constexpr double kGsPerLSB = 0.00390625;

  enum SPIAddressFields { kAddress_Read = 0x80, kAddress_MultiByte = 0x40 };

  enum PowerCtlFields {
    kPowerCtl_Link = 0x20,
    kPowerCtl_AutoSleep = 0x10,
    kPowerCtl_Measure = 0x08,
    kPowerCtl_Sleep = 0x04
  };

  enum DataFormatFields {
    kDataFormat_SelfTest = 0x80,
    kDataFormat_SPI = 0x40,
    kDataFormat_IntInvert = 0x20,
    kDataFormat_FullRes = 0x08,
    kDataFormat_Justify = 0x04
  };
};

}  // namespace frc
