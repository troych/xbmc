/*
 *      Copyright (C) 2014-2015 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "JoystickTypes.h"

#include <stdint.h>

/*!
 * \ingroup joysticks
 *
 * \brief Basic driver element associated with input events
 *
 * A driver primitive can be a button, one of the four direction arrows on a
 * dpad, or the positive or negative half of an axis.
 */
class CDriverPrimitive
{
public:
  /*!
   * \brief Type of driver primitive
   */
  enum PrimitiveType
  {
    Unknown = 0,
    Button,
    Hat,       // one of the four direction arrows on a dpad
    SemiAxis,  // the positive or negative half of an axis
  };

  /*!
   * \brief Construct an invalid driver primitive
   */
  CDriverPrimitive(void);

  /*!
   * \brief Construct a driver primitive representing a button
   */
  CDriverPrimitive(unsigned int buttonIndex);

  /*!
   * \brief Construct a driver primitive representing one of the four direction
   *        arrows on a dpad
   */
  CDriverPrimitive(unsigned int hatIndex, HAT_DIRECTION direction);

  /*!
   * \brief Construct a driver primitive representing the positive or negative
   *        half of an axis
   */
  CDriverPrimitive(unsigned int axisIndex, SEMIAXIS_DIRECTION direction);

  bool operator==(const CDriverPrimitive& rhs) const;
  bool operator<(const CDriverPrimitive& rhs) const;

  bool operator!=(const CDriverPrimitive& rhs) const { return !operator==(rhs); }
  bool operator>(const CDriverPrimitive& rhs) const  { return !(operator<(rhs) || operator==(rhs)); }
  bool operator<=(const CDriverPrimitive& rhs) const { return   operator<(rhs) || operator==(rhs); }
  bool operator>=(const CDriverPrimitive& rhs) const { return  !operator<(rhs); }

  /*!
   * \brief Type of element represented by the driver primitive
   *
   * The type determines the fields in use:
   *
   *    Button:
   *       - driver index
   *
   *    Hat direction:
   *       - driver index
   *       - hat direction
   *
   *    Semiaxis:
   *       - driver index
   *       - semiaxis direction
   */
  PrimitiveType      Type(void) const              { return m_type; }
  unsigned int       Index(void) const             { return m_driverIndex; }
  HAT_DIRECTION      HatDirection(void) const      { return m_hatDirection; }
  SEMIAXIS_DIRECTION SemiAxisDirection(void) const { return m_semiAxisDirection; }

  /*!
   * \brief Test if an driver primitive is valid
   *
   * A driver primitive is valid if has a known type and:
   *
   *   1) for hats, is a cardinal direction
   *   2) for semi-axes, is a positive or negative direction
   */
  bool IsValid(void) const;

private:
  PrimitiveType      m_type;
  unsigned int       m_driverIndex;
  HAT_DIRECTION      m_hatDirection;
  SEMIAXIS_DIRECTION m_semiAxisDirection;
};
