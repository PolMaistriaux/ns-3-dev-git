/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
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
 */ 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"


using namespace ns3;

int main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);

  double radius = 6500 ;
  double sensing_radius = 800;
  double node_spacing_x = sensing_radius*std::sqrt(3);
  double node_spacing_y = sensing_radius*3/2;

  int n = std::ceil(3*(radius/node_spacing_x) *(radius/node_spacing_y));
  std::cout << "Number of nodes: "<<n<<std::endl;

  NodeContainer nodes;
  
  // create an array of empty nodes for testing purposes 
  //nodes.Create (n);
  // setup the grid itself: objects are laid out
  // started from (-100,-100) with 20 objects per row, 
  // the x interval between each object is 5 meters
  // and the y interval between each object is 20 meters
  SensingGridPositionAllocator positionAllocator = SensingGridPositionAllocator();
  positionAllocator.SetRadius(radius);
  positionAllocator.SetSensingRadius(sensing_radius);
  positionAllocator.SetZ(5.0);
  positionAllocator.Initialize();

  // each object will be attached a static position.
  // i.e., once set by the "position allocator", the
  // position will never change.
  ObjectFactory m_mobility;
  m_mobility.SetTypeId ("ns3::ConstantPositionMobilityModel");

  while(positionAllocator.HasNext()){
    Ptr<Object> object = CreateObject<Node> ();
    Ptr<MobilityModel> model = m_mobility.Create ()->GetObject<MobilityModel> ();
    object->AggregateObject (model);

    Vector position = positionAllocator.GetNext ();
    model->SetPosition (position);
    nodes.Add(object->GetObject<Node> ());
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////
/*
  nodes.Create (n);
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::SensingGridPositionAllocator",
                                 "Radius", DoubleValue (radius),
                                 "SensingRadius", DoubleValue (sensing_radius),
                                 "Z", DoubleValue (5.0));
  // each object will be attached a static position.
  // i.e., once set by the "position allocator", the
  // position will never change.
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // finalize the setup by attaching to each object
  // in the input array a position and initializing
  // this position with the calculated coordinates.
  mobility.Install (nodes);*/

  // iterate our nodes and print their position.
  std::ofstream outputFile;
  // Delete contents of the file as it is opened
  outputFile.open ("Results_Simu/positions.txt", std::ofstream::out | std::ofstream::trunc);
  for (NodeContainer::Iterator j = nodes.Begin (); j != nodes.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Vector pos = position->GetPosition ();
      outputFile << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }
    outputFile.close ();
  Simulator::Destroy ();
  return 0;
}
