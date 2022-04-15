/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#ifndef SENSING_FIELD_POSITION_ALLOCATOR_H
#define SENSING_FIELD_POSITION_ALLOCATOR_H

#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include "ns3/vector.h"
#include "ns3/position-allocator.h"

namespace ns3 {


class RectangleFieldSensingPositionAllocator : public PositionAllocator
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RectangleFieldSensingPositionAllocator ();

  RectangleFieldSensingPositionAllocator (double x, double y, double xSensing_radius,double z);

  void SetParams (double x, double y, double xSensing_radius,double z);
  
  void ComputePositions (void);
  
  int  GetN (void) const;

  virtual Vector GetNext (void) const;
  virtual int64_t AssignStreams (int64_t stream);

private:
  mutable double current_x;
  mutable double current_y;
  mutable int    n;
  mutable int    rowType;
  mutable bool   hasNext; 
  double size_x;
  double size_y;
  double sensing_radius;
  double node_spacing_x;
  double node_spacing_y;
  double m_z; //!< z coordinate of all the positions generated

    /**
   * The list of current positions
   */
  std::vector<Vector> m_positions;

  /**
   * The iterator pointing to the next position to return
   */
  mutable std::vector<Vector>::const_iterator m_next;
};




class RectangleFieldGatewayPositionAllocator : public PositionAllocator
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RectangleFieldGatewayPositionAllocator ();

  RectangleFieldGatewayPositionAllocator (double x, double y, double range,double z);

  void SetParams (double x, double y, double range,double z);
  
  void ComputePositions (void);
  
  int  GetN (void) const;

  virtual Vector GetNext (void) const;
  virtual int64_t AssignStreams (int64_t stream);

private:
  mutable double current_x;
  mutable double current_y;
  mutable int    n;
  mutable int    rowType;
  mutable bool   hasNext; 
  double size_x;
  double size_y;
  double m_range;
  double node_spacing_x;
  double node_spacing_y;
  double m_z; //!< z coordinate of all the positions generated

    /**
   * The list of current positions
   */
  std::vector<Vector> m_positions;

  /**
   * The iterator pointing to the next position to return
   */
  mutable std::vector<Vector>::const_iterator m_next;
};

} // namespace ns3

#endif /* SENSING_FIELD_POSITION_ALLOCATOR_H */
