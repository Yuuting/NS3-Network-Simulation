 /* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
 /*
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

 //
 // Example of the sending of a datagram to a broadcast address
 //
 // Network topology
 //     =======================
 //       |          |
 //       n0    n1   n2   n3
 //       |     |          |
 //     =======================
 //
 //   n0 originates UDP broadcast to 255.255.255.255/discard port, which
 //   is replicated and received on both n1 and n2
 #include "ns3/netanim-module.h"
 #include <iostream>
 #include <fstream>
 #include <string>
 #include <cassert>

 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
 #include "ns3/csma-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/internet-module.h"

 using namespace ns3;

 NS_LOG_COMPONENT_DEFINE ("CsmaBroadcastExample");

 int
 main (int argc, char *argv[])
 {
   // Users may find it convenient to turn on explicit debugging
   // for selected modules; the below lines suggest how to do this
 #if 0
   LogComponentEnable ("CsmaBroadcastExample", LOG_LEVEL_INFO);
 #endif
   LogComponentEnable ("CsmaBroadcastExample", LOG_PREFIX_TIME);

   // Allow the user to override any of the defaults and the above
   // Bind()s at run-time, via command-line arguments
   CommandLine cmd (__FILE__);
   cmd.Parse (argc, argv);

   NS_LOG_INFO ("Create nodes.");
   NodeContainer c;
   c.Create (4);
   NodeContainer c0 = NodeContainer (c.Get (0), c.Get (1));
   NodeContainer c1 = NodeContainer (c.Get (0), c.Get (2));
   NodeContainer c2 = NodeContainer (c.Get (0), c.Get (3));

   NS_LOG_INFO ("Build Topology.");
   CsmaHelper csma;
   csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
   csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

   NetDeviceContainer n0 = csma.Install (c0);
   NetDeviceContainer n1 = csma.Install (c1);
   NetDeviceContainer n2 = csma.Install (c2);

   InternetStackHelper internet;
   internet.Install (c);

   NS_LOG_INFO ("Assign IP Addresses.");
   Ipv4AddressHelper ipv4;
   ipv4.SetBase ("10.1.0.0", "255.255.255.0");
   ipv4.Assign (n0);
   ipv4.SetBase ("0.0.0.0", "255.255.255.0");
   ipv4.Assign (n1);
   ipv4.SetBase ("10.1.2.0", "255.255.255.0");
   ipv4.Assign (n2);


   // RFC 863 discard port ("9") indicates packet should be thrown away
   // by the system.  We allow this silent discard to be overridden
   // by the PacketSink application.
   uint16_t port = 9;

   // Create the OnOff application to send UDP datagrams of size
   // 512 bytes (default) at a rate of 500 Kb/s (default) from n0
   NS_LOG_INFO ("Create Applications.");
   OnOffHelper onoff ("ns3::UdpSocketFactory",
                      Address (InetSocketAddress (Ipv4Address ("255.255.255.255"), port)));
   onoff.SetConstantRate (DataRate ("500kb/s"));

   ApplicationContainer app = onoff.Install (c0.Get (0));
   // Start the application
   app.Start (Seconds (1.0));
   app.Stop (Seconds (1.02));

   OnOffHelper onoff1 ("ns3::UdpSocketFactory",
                      Address (InetSocketAddress (Ipv4Address ("10.1.0.1"), port)));
   onoff1.SetConstantRate (DataRate ("500kb/s"));

   OnOffHelper onoff2 ("ns3::UdpSocketFactory",
                      Address (InetSocketAddress (Ipv4Address ("10.1.2.1"), port)));
   onoff2.SetConstantRate (DataRate ("500kb/s"));

   ApplicationContainer app1 = onoff1.Install (c0.Get (1));
   app1.Add(onoff2.Install (c2.Get (1)));
   // Start the application
   app1.Start (Seconds (1.02));
   app1.Stop (Seconds (1.03));

   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
   ipv4.Assign (n1);

   ApplicationContainer app2 = onoff.Install (c0.Get (0));
   //Start the application
   app2.Start (Seconds (1.06));
   app2.Stop (Seconds (1.10));

   OnOffHelper onoff3 ("ns3::UdpSocketFactory",
                       Address (InetSocketAddress (Ipv4Address ("10.1.1.1"), port)));
   onoff3.SetConstantRate (DataRate ("500kb/s"));

   ApplicationContainer app3 = onoff1.Install (c0.Get (1));
   app3.Add(onoff2.Install (c2.Get (1)));
   app3.Add(onoff3.Install (c1.Get (1)));
   // Start the application
   app3.Start (Seconds (1.11));
   app3.Stop (Seconds (1.12));

   // Create an optional packet sink to receive these packets
   PacketSinkHelper sink ("ns3::UdpSocketFactory",
                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
   app = sink.Install (c0.Get (1));
   app.Add (sink.Install (c1.Get (1)));
   app.Add (sink.Install (c2.Get (1)));
   app.Start (Seconds (1.0));
   app.Stop (Seconds (1.02));

   //PacketSinkHelper sink1 ("ns3::UdpSocketFactory",
                             //Address (InetSocketAddress (Ipv4Address("0.0.0.2"), port)));
   //app1 = sink1.Install (c0.Get (1));
   //app1.Add (sink1.Install (c1.Get (1)));
   //app1.Add (sink1.Install (c2.Get (1)));
   //app1.Start (Seconds (1.05));
   //app1.Stop (Seconds (1.1));

   // Configure ascii tracing of all enqueue, dequeue, and NetDevice receive
   // events on all devices.  Trace output will be sent to the file
   // "csma-one-subnet.tr"
   AsciiTraceHelper ascii;
   csma.EnableAsciiAll (ascii.CreateFileStream ("csma-broadcast.tr"));

   // Also configure some tcpdump traces; each interface will be traced
   // The output files will be named
   // csma-broadcast-<nodeId>-<interfaceId>.pcap
   // and can be read by the "tcpdump -tt -r" command
   csma.EnablePcapAll ("csma-broadcast", false);

   NS_LOG_INFO ("Run Simulation.");
   AnimationInterface anim("broadcast.xml");
   anim.SetConstantPosition (c.Get (0), 0, 50); //位置
   anim.SetConstantPosition (c.Get (1), 100, 0); //位置
   anim.SetConstantPosition (c.Get (2), 100, 50); //位置
   anim.SetConstantPosition (c.Get (3), 100, 100); //位置
   anim.UpdateNodeDescription (0, "控制中心");
   anim.UpdateNodeDescription (1, "终端1");
   anim.UpdateNodeDescription (2, "终端2");
   anim.UpdateNodeDescription (3, "终端3");
   anim.UpdateNodeSize(0, 5, 5);
   anim.UpdateNodeSize(1, 5, 5);
   anim.UpdateNodeSize(2, 5, 5);
   anim.UpdateNodeSize(3, 5, 5);

   Simulator::Run ();
   Simulator::Destroy ();
   NS_LOG_INFO ("Done.");
 }
