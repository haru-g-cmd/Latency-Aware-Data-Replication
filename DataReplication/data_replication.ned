simple CoreNode
{
    parameters:
        string type = "core";
        int nodeId = 0;
        @signal[coreTimeTaken](type=double);
        @statistic[coreTimeTaken](title="Time taken to send data"; record=vector?,count?);
    gates:
        inout gate[];
}

simple EdgeNode
{
    parameters:
        string type = "edge";
        int nodeId; // 0 is for Core Node
        @signal[edgeRequestCore](type=long);
        @statistic[edgeRequestCore](title="Requested Core for data"; record=vector?,count?);
        @signal[edgeHaveData](type=long);
        @statistic[edgeHaveData](title="Already had data"; record=vector?,count?);
        @signal[edgeHitMissRatio](type=double);
        @statistic[edgeHitMissRatio](title="Hit/Miss Ratio"; record=vector?,count?);
    gates:
        inout gate[];
}

simple User
{
    parameters:
        string userId;
        int requestInterval = 1;
        @signal[userRequestedData](type=long);
        @statistic[userRequestedData](title="The number of requests sent out"; record=vector?,count?);
        @signal[userDataReceived](type=long);
        @statistic[userDataReceived](title="The number of data packets received"; record=vector?,count?);
        @signal[userRequestDelay](type=simtime_t);
        @statistic[userRequestDelay](title="Time between sending req and getting data"; record=vector?,histogram?);
    gates:
        inout gate[];
}

simple ReplicationManager
{
    gates:
        inout gate[];
}

network DataReplicationNetwork
{
    parameters:
        int edgeNodeCount = default(5);
        int userCount = default(5);
        @display("bgb=801,602");
    submodules:
        replicationManager: ReplicationManager {
            @display("p=389,39"); 
        }

        edgeNode1: EdgeNode {
            @display("p=90,355"); 
            nodeId = 1;
        }
        edgeNode2: EdgeNode {
            @display("p=230,357"); 
            nodeId = 2;
        }
        edgeNode3: EdgeNode {
            @display("p=388,356"); 
            nodeId = 3;
        }
        edgeNode4: EdgeNode {
            @display("p=529,357"); 
            nodeId = 4;
        }
        edgeNode5: EdgeNode {
            @display("p=664,357"); 
            nodeId = 5;
        }

        coreNode1: CoreNode {
            @display("p=389,153"); 
        }

        user1: User {
            userId = "u1";
            @display("p=90,474"); 
        }

        user2: User {
            userId = "u2";
            @display("p=229,474"); 
        }

        user3: User {
            userId = "u3";
            @display("p=388,474");  
        }

        user4: User {
            userId = "u4";
            @display("p=528,474");  
        }

        user5: User {
            userId = "u5";
            @display("p=663,474");  
        }

//        user6: User {
//            userId = "u6";
//            @display("p=147,463"); 
//        }
//
//        user7: User {
//            userId = "u7";
//            @display("p=207,463");  
//        }
//
//        user8: User {
//            userId = "u8";
//            @display("p=264,463");  
//        }
//
//        user9: User {
//            userId = "u9";
//            @display("p=320,463");  
//        }
//
//        user10: User {
//            userId = "u10";
//            @display("p=379,463"); 
//        }
//
//        user11: User {
//            userId = "u10";
//            @display("p=435,463");  
//        }
//
//        user12: User {
//            userId = "u10";
//            @display("p=487,463");  
//        }
//
//        user13: User {
//            userId = "u10";
//            @display("p=544,463");  
//        }
//
//        user14: User {
//            userId = "u10";
//            @display("p=599,463"); 
//        }
//
//        user15: User {
//            userId = "u10";
//            @display("p=654,463");  
//        }
//
//        user16: User {
//            userId = "u10";
//            @display("p=710,463");  
//        }
//
//        user17: User {
//            userId = "u10";
//            @display("p=766,463"); 
//        }
//
//        user18: User {
//            userId = "u10";
//            @display("p=766,360");  
//        }
//
//        user19: User {
//            userId = "u10";
//            @display("p=766,251");  
//        }
//
//        user20: User {
//            userId = "u10";
//            @display("p=767,153");  
//        }


    connections allowunconnected:
        // Connect each edge node to the core node
        edgeNode1.gate++ <--> coreNode1.gate++;
        edgeNode2.gate++ <--> coreNode1.gate++;
        edgeNode3.gate++ <--> coreNode1.gate++;
        edgeNode4.gate++ <--> coreNode1.gate++;
        edgeNode5.gate++ <--> coreNode1.gate++;

        // Connect the replication manager to the core node
        replicationManager.gate++ <--> coreNode1.gate++;

        // Connect each user to an edge node
        user1.gate++ <--> edgeNode1.gate++;
        user2.gate++ <--> edgeNode2.gate++;
        user3.gate++ <--> edgeNode3.gate++;
        user4.gate++ <--> edgeNode4.gate++;
        user5.gate++ <--> edgeNode5.gate++;
//        user6.gate++ <--> edgeNode2.gate++;
//        user7.gate++ <--> edgeNode2.gate++;
//        user8.gate++ <--> edgeNode2.gate++;
//        user9.gate++ <--> edgeNode3.gate++;
//        user10.gate++ <--> edgeNode3.gate++;
//        user11.gate++ <--> edgeNode3.gate++;
//        user12.gate++ <--> edgeNode3.gate++;
//        user13.gate++ <--> edgeNode4.gate++;
//        user14.gate++ <--> edgeNode4.gate++;
//        user15.gate++ <--> edgeNode4.gate++;
//        user16.gate++ <--> edgeNode4.gate++;
//        user17.gate++ <--> edgeNode5.gate++;
//        user18.gate++ <--> edgeNode5.gate++;
//        user19.gate++ <--> edgeNode5.gate++;
//        user20.gate++ <--> edgeNode5.gate++;

        // Connect each edge node to the replication manager
        edgeNode1.gate++ <--> replicationManager.gate++;
        edgeNode2.gate++ <--> replicationManager.gate++;
        edgeNode3.gate++ <--> replicationManager.gate++;
        edgeNode4.gate++ <--> replicationManager.gate++;
        edgeNode5.gate++ <--> replicationManager.gate++;

        // Connect the users to the core node(s)
//        user1.gate++ <--> coreNode1.gate++;
//        user2.gate++ <--> coreNode1.gate++;
//        user3.gate++ <--> coreNode1.gate++;
//        user4.gate++ <--> coreNode1.gate++;
//        user5.gate++ <--> coreNode1.gate++;
//        user6.gate++ <--> coreNode1.gate++;
//        user7.gate++ <--> coreNode1.gate++;
//        user8.gate++ <--> coreNode1.gate++;
//        user9.gate++ <--> coreNode1.gate++;
//        user10.gate++ <--> coreNode1.gate++;
//        user11.gate++ <--> coreNode1.gate++;
//        user12.gate++ <--> coreNode1.gate++;
//        user13.gate++ <--> coreNode1.gate++;
//        user14.gate++ <--> coreNode1.gate++;
//        user15.gate++ <--> coreNode1.gate++;
//        user16.gate++ <--> coreNode1.gate++;
//        user17.gate++ <--> coreNode1.gate++;
//        user18.gate++ <--> coreNode1.gate++;
//        user19.gate++ <--> coreNode1.gate++;
//        user20.gate++ <--> coreNode1.gate++;

}


// for 5-5 configuration
//simple CoreNode
//{
//    parameters:
//        string type = "core";
//        int nodeId = 0;
//        @signal[coreTimeTaken](type=double);
//        @statistic[coreTimeTaken](title="Time taken to send data"; record=vector?,count?);
//    gates:
//        inout gate[];
//}
//
//simple EdgeNode
//{
//    parameters:
//        string type = "edge";
//        int nodeId; // 0 is for Core Node
//        @signal[edgeRequestCore](type=long);
//        @statistic[edgeRequestCore](title="Requested Core for data"; record=vector?,count?);
//        @signal[edgeHaveData](type=long);
//        @statistic[edgeHaveData](title="Already had data"; record=vector?,count?);
//        @signal[edgeHitMissRatio](type=double);
//        @statistic[edgeHitMissRatio](title="Hit/Miss Ratio"; record=vector?,count?);
//    gates:
//        inout gate[];
//}
//
//simple User
//{
//    parameters:
//        string userId;
//        int requestInterval = 1; // Time between requests
//        @signal[userRequestedData](type=long);
//        @statistic[userRequestedData](title="The number of requests sent out"; record=vector?,count?);
//        @signal[userDataReceived](type=long);
//        @statistic[userDataReceived](title="The number of data packets received"; record=vector?,count?);
//        @signal[userRequestDelay](type=simtime_t);
//        @statistic[userRequestDelay](title="Time between sending req and getting data"; record=vector?, histogram?);
//    gates:
//        inout gate[];
//}
//
//simple ReplicationManager
//{
//    gates:
//        inout gate[];
//}
//
//network DataReplicationNetwork
//{
//    parameters:
//        int edgeNodeCount = default(5);
//        int userCount = default(5);
//        @display("bgb=600,600");
//    submodules:
//        replicationManager: ReplicationManager {
//            @display("p=300,50");  // Positioning the replication manager
//        }
//
//        // Define Core Node (center of the star configuration)
//        coreNode1: CoreNode {
//            @display("p=300,300");  // Core node at the center
//        }
//
//        // Define Edge Nodes around the Core Node in a star layout
//        edgeNode1: EdgeNode {
//            @display("p=100,300");
//            nodeId = 1;
//        }
//        edgeNode2: EdgeNode {
//            @display("p=200,450");
//            nodeId = 2;
//        }
//        edgeNode3: EdgeNode {
//            @display("p=400,450");
//            nodeId = 3;
//        }
//        edgeNode4: EdgeNode {
//            @display("p=500,300");
//            nodeId = 4;
//        }
//        edgeNode5: EdgeNode {
//            @display("p=300,150");
//            nodeId = 5;
//        }
//
//        // Define Users placed closer to their respective Edge Nodes
//        user1: User {
//            userId = "u1";
//            @display("p=50,300");
//        }
//
//        user2: User {
//            userId = "u2";
//            @display("p=200,500");
//        }
//
//        user3: User {
//            userId = "u3";
//            @display("p=400,500");
//        }
//
//        user4: User {
//            userId = "u4";
//            @display("p=550,300");
//        }
//
//        user5: User {
//            userId = "u5";
//            @display("p=400,70");
//        }
//    connections allowunconnected:
//        // Connect each edge node to the core node
//        edgeNode1.gate++ <--> coreNode1.gate++;
//        edgeNode2.gate++ <--> coreNode1.gate++;
//        edgeNode3.gate++ <--> coreNode1.gate++;
//        edgeNode4.gate++ <--> coreNode1.gate++;
//        edgeNode5.gate++ <--> coreNode1.gate++;
//
//        // Connect the replication manager to the core node
//        replicationManager.gate++ <--> coreNode1.gate++;
//
//        // Connect each user to their respective edge node
//        user1.gate++ <--> edgeNode1.gate++;
////		user1.gate++ <--> edgeNode2.gate++;
//        user2.gate++ <--> edgeNode2.gate++;
//        user3.gate++ <--> edgeNode3.gate++;
//        user4.gate++ <--> edgeNode4.gate++;
//        user5.gate++ <--> edgeNode5.gate++;
//}
//


