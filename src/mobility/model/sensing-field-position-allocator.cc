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
#include "sensing-field-position-allocator.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/csv-reader.h"

#include <cmath>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (RectangleFieldSensingPositionAllocator);

TypeId
RectangleFieldSensingPositionAllocator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RectangleFieldSensingPositionAllocator")
    .SetParent<PositionAllocator> ()
    .SetGroupName ("Mobility")
    .AddConstructor<RectangleFieldSensingPositionAllocator> ()
    .AddAttribute ("SizeX", "Size of the field in x",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldSensingPositionAllocator::size_x),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SizeY", "Size of the field in y",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldSensingPositionAllocator::size_y),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SensingRadius", "Sensing radius of the node",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldSensingPositionAllocator::sensing_radius),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Z",
                   "The z coordinate of all the positions allocated.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&RectangleFieldSensingPositionAllocator::m_z),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

RectangleFieldSensingPositionAllocator::RectangleFieldSensingPositionAllocator ()
  : current_x (-1), current_y(-1), n(0), rowType(0), hasNext(true), size_x(100), size_y(100), sensing_radius(100), m_z(5)
{
  ComputePositions();
}

RectangleFieldSensingPositionAllocator::RectangleFieldSensingPositionAllocator (double x, double y, double xSensing_radius,double z)
  : current_x (-1), current_y(-1), n(0), rowType(0), hasNext(true), size_x(x), size_y(y), sensing_radius(xSensing_radius), m_z(z)
{
  ComputePositions();
}

void
RectangleFieldSensingPositionAllocator::ComputePositions (void)
{
  node_spacing_x = sensing_radius*std::sqrt(3);
  node_spacing_y = sensing_radius*3/2;
  current_x = 0;
  current_y = 0;
  hasNext   =  true;
  while(hasNext){  
    //Checking if the point is inside the coverage circle
    if(current_x <= size_x){
      n ++;
      m_positions.push_back (Vector(current_x,current_y,m_z));
      current_x = current_x + node_spacing_x;
    }
    else{
      rowType = (rowType+1)%2 ; 
      current_y = current_y + node_spacing_y;
      current_x = node_spacing_x*rowType/2;
    } 

    //Checking if the point is still inside the possible square
    if  (current_y > size_y){
      hasNext = false;
    }  
  } 

  m_next = m_positions.begin();
}

void
RectangleFieldSensingPositionAllocator::SetParams (double x, double y, double xSensing_radius,double z)
{
  size_x = x;
  size_y = y;
  sensing_radius = xSensing_radius;
  m_z = z;
  m_positions.clear();
  n=0;
  ComputePositions();
}

Vector
RectangleFieldSensingPositionAllocator::GetNext (void) const
{
  Vector position = *m_next;
  m_next++;
  return position;
}

int 
RectangleFieldSensingPositionAllocator::GetN (void) const
{
  return n;
}

int64_t
RectangleFieldSensingPositionAllocator::AssignStreams (int64_t stream)
{
  return 0;
}









NS_OBJECT_ENSURE_REGISTERED (RectangleFieldGatewayPositionAllocator);

TypeId
RectangleFieldGatewayPositionAllocator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RectangleFieldGatewayPositionAllocator")
    .SetParent<PositionAllocator> ()
    .SetGroupName ("Mobility")
    .AddConstructor<RectangleFieldGatewayPositionAllocator> ()
    .AddAttribute ("SizeX", "Size of the field in x",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldGatewayPositionAllocator::size_x),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SizeY", "Size of the field in y",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldGatewayPositionAllocator::size_y),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Range", "Range of the gateway",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&RectangleFieldGatewayPositionAllocator::m_range),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Z",
                   "The z coordinate of all the positions allocated.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&RectangleFieldGatewayPositionAllocator::m_z),
                   MakeDoubleChecker<double> ())
    ;
  return tid;
}

RectangleFieldGatewayPositionAllocator::RectangleFieldGatewayPositionAllocator ()
  : current_x (-1), current_y(-1), n(0), rowType(0), hasNext(true), size_x(100), size_y(100),  m_range(1000), m_z(15)
{
  ComputePositions();
}

RectangleFieldGatewayPositionAllocator::RectangleFieldGatewayPositionAllocator (double x, double y, double range,double z)
  : current_x (-1), current_y(-1), n(0), rowType(0), hasNext(true), size_x(x), size_y(y), m_range(range), m_z(z)
{
  ComputePositions();
}

void
RectangleFieldGatewayPositionAllocator::ComputePositions (void)
{
  double rangeSqrt2      = m_range/std::sqrt(2);
  double rangeMinusSqrt2 = m_range - rangeSqrt2;

  node_spacing_x = m_range*std::sqrt(3);
  node_spacing_y = m_range*3/2;
  current_x = rangeSqrt2;
  current_y = m_range/2;//rangeSqrt2;
  hasNext   =  true;
  while(hasNext){  
    //Checking if the point is inside the coverage circle
    if(current_x < size_x + rangeMinusSqrt2){
      n ++;
      m_positions.push_back (Vector(current_x,current_y,m_z));
      current_x = current_x + node_spacing_x;
    }
    else{
      rowType = (rowType+1)%2 ; 
      current_y = current_y + node_spacing_y;
      if(rowType == 1)
        current_x = -rangeMinusSqrt2;
      else
        current_x = rangeSqrt2;
    }

    //Checking if the point is still inside the possible square
    if  (current_y >= size_y + rangeMinusSqrt2){
      hasNext = false;
    }  
  } 
  m_next = m_positions.begin();
}

void
RectangleFieldGatewayPositionAllocator::SetParams (double x, double y, double range,double z)
{
  size_x = x;
  size_y = y;
  m_range = range;
  m_z = z;
  m_positions.clear();
  n=0;
  ComputePositions();
}

Vector
RectangleFieldGatewayPositionAllocator::GetNext (void) const
{
  Vector position = *m_next;
  m_next++;
  return position;
}

int 
RectangleFieldGatewayPositionAllocator::GetN (void) const
{
  return n;
}

int64_t
RectangleFieldGatewayPositionAllocator::AssignStreams (int64_t stream)
{
  return 0;
}

} // namespace ns3
