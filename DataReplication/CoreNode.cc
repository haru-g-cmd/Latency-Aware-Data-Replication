#include <fstream>
#include <omnetpp/cpacket.h>
#include "json.hpp"
#include "CoreNode.h"
#include "DataRequest_m.h"
#include "CoreRequest_m.h"
#include "ManagerMessage_m.h"
#include "ReplicateRequest_m.h"
#include "SendingUserData_m.h"

using namespace omnetpp;

Define_Module(CoreNode);

void CoreNode::initialize() {
    // Load data from the file
    loadDataFromFile("synthetic_data.json");
    EV << "Core Node initialized with " << dataStore.size() << " data items.\n";

    coreTimeTaken = registerSignal("coreTimeTaken");
}

void CoreNode::handleMessage(cMessage *msg) {
    // convert the received message to the right data type
    if (strcmp(msg->getName(), "edge_request") == 0)
    {
        CoreRequest *edge_msg = check_and_cast<CoreRequest *>(msg);
        std::string requestedData = edge_msg->getDataId();
        int edgeId = edge_msg->getEdgeId();
        int size = dataStore[requestedData].size;
        std::string tempUserId = edge_msg->getUserId();
        int userId = std::stoi(tempUserId.substr(1, tempUserId.length())) + 5;

        std::string dest = "replicate";
        double time = float(size) / 4000;
        char msgname[20];
        snprintf(msgname, sizeof(msgname), dest.c_str());

        // metrics
        signalTimeTaken += time;
        emit(coreTimeTaken, signalTimeTaken);

        ReplicateRequest *replicate = new ReplicateRequest(msgname);
        replicate->setEdgeId(edgeId);
        replicate->setUserId(tempUserId.c_str());
        replicate->setDataId(edge_msg->getDataId());
        replicate->setType(dataStore[requestedData].type.c_str());
        replicate->setSize(dataStore[requestedData].size);
        sendDelayed(replicate, time, "gate$o", edgeId - 1);

        EV << "SENDING " << requestedData << " from CORE to EDGE " << edgeId << " in " << time << "s." << endl;

        std::string sendingEdgeBubble = "Sending " + requestedData + " to edge " \
                + std::to_string(edgeId) + " in " + std::to_string(time) + "s";
        bubble(sendingEdgeBubble.c_str());

        // inform the replication manager
        dest = "inform";
        snprintf(msgname, sizeof(msgname), dest.c_str());

        ManagerMessage *inform = new ManagerMessage(msgname);
        inform->setEdgeId(edge_msg->getEdgeId());
        inform->setDataId(requestedData.c_str());
        inform->setUserId(tempUserId.c_str());
        send(inform, "gate$o", 5); // replication manager at gate 5
//        send(inform, "gate$o", 3); // for 3 edge nodes
    } else if (strcmp(msg->getName(), "replicate") == 0) {
        ManagerMessage *rmMessage = check_and_cast<ManagerMessage *>(msg);
        std::string requestedData = rmMessage->getDataId();
        int edgeId = rmMessage->getEdgeId();
        std::string userId = rmMessage->getUserId();

        int size = dataStore[requestedData].size;
        std::string dest = "replicate";
        double time = float(size) / 4000;
        char msgname[20];
        snprintf(msgname, sizeof(msgname), dest.c_str());

        ReplicateRequest *replicate = new ReplicateRequest(msgname);
        replicate->setEdgeId(edgeId);
        replicate->setUserId(userId.c_str());
        replicate->setDataId(rmMessage->getDataId());
        replicate->setType(dataStore[requestedData].type.c_str());
        replicate->setSize(dataStore[requestedData].size);
        sendDelayed(replicate, time, "gate$o", edgeId - 1);

        signalTimeTaken += time;
        emit(coreTimeTaken, signalTimeTaken);

        std::string sendingEdgeBubble = "Replicating " + requestedData + " to edge " \
                + std::to_string(edgeId) + " in " + std::to_string(time) + "s";
        bubble(sendingEdgeBubble.c_str());

    } else {
        // request coming from user directly
        DataRequest *user_msg = check_and_cast<DataRequest *>(msg);
        std::string requestedData = user_msg->getDataId();
        int size = dataStore[requestedData].size;

        std::string tempUserId = user_msg->getUserId();
//        int userId = std::stoi(tempUserId.substr(1, tempUserId.length())) + 3;
        int userId = std::stoi(tempUserId.substr(1, tempUserId.length())) - 1;
        float time = float(size) / 1000;

        SendingUserData *send_user_msg = new SendingUserData("sendingData");
        send_user_msg->setDataId(requestedData.c_str());
        send_user_msg->setUserId(tempUserId.c_str());
        sendDelayed(send_user_msg, time, "gate$o", userId);
        std::string sendingUserDirectString = "Sending " + requestedData + " to user " + tempUserId;
        sendingUserDirectString += " in " + std::to_string(time) + "s.";
        bubble(sendingUserDirectString.c_str());

        signalTimeTaken += time;
        emit(coreTimeTaken, signalTimeTaken);
    }

    delete msg;
}

void CoreNode::loadDataFromFile(const std::string &filename) {
    std::ifstream fJson(filename);
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());

    for (auto item: json) {
        DataItem data;
        data.id = item["id"];
        data.type = item["type"];
        data.size = item["size"];
        data.access_frequency = item["access_frequency"];

        dataStore[item["id"]] = data;
    }

    EV << "Loaded " << dataStore.size() << " data items.\n";
}
