#include "DataRequest_m.h"
#include "CoreRequest_m.h"
#include "ReplicateRequest_m.h"
#include "ManagerMessage_m.h"
#include "SendingUserData_m.h"
#include "DataItem.h"
#include "json.hpp"
#include <fstream>
#include <map>
#include <omnetpp.h>
#include <string>
#include <climits>

using namespace omnetpp;

class EdgeNode : public cSimpleModule {
  private:
    std::map<std::string, DataItem> cachedData; // Data items stored locally
    int edgeId;
    int cacheLimit = 20;
    long signalRequestCore = 0, signalHaveData = 0;
    double signalHitMissRatio = 0.0;
    simsignal_t edgeHitMissRatio;
    simsignal_t edgeRequestCore;
    simsignal_t edgeHaveData;

  protected:
    virtual void initialize() override {
        // initialize signals for metrics
        edgeRequestCore = registerSignal("edgeRequestCore");
        edgeHaveData = registerSignal("edgeHaveData");
        edgeHitMissRatio = registerSignal("edgeHitMissRatio");

        // get the edge id
        edgeId = par("nodeId");

        // Caching some data in this edge node
        std::ifstream fJson("synthetic_data.json");
        std::stringstream buffer;
        buffer << fJson.rdbuf();
        auto json = nlohmann::json::parse(buffer.str());

        // a poor attempt to randomly store some data into each edge node
        int num_items = 0;
        int random_skip = intuniform(0, 70);
        int i = 0;
        for (auto item: json) {
            while (i++ < random_skip)
                continue;
            if (num_items >= cacheLimit || i >= 100)
                break;

            DataItem data;
            data.id = item["id"];
            data.type = item["type"];
            data.size = item["size"];
            data.access_frequency = 0;

            cachedData[data.id] = data;
            ++num_items;
        }

        return;
    }

    virtual void handleMessage(cMessage *msg) override {
        if (strcmp(msg->getName(), "replicate") == 0)
        {
            // deletes the item with the lowest frequency to make room
            // for the new item to be replicated
            std::string test = "Sending replicated data";
            bubble(test.c_str());

            ReplicateRequest *rep_req = check_and_cast<ReplicateRequest *>(msg);
            std::string dataId = rep_req->getDataId();
//            // for 3:10 configuration
            std::string userIdStr = rep_req->getUserId();
            userIdStr = userIdStr.substr(1, userIdStr.length());
//            int userId = 0;

            // for 5-20 and 3-12
//            int userint = std::stoi(userIdStr);
//            int subt = 4 * (edgeId - 1);
//            int userId = (userint - subt) % 5;

//            if (edgeId == 1) { // because 4 users are connected
//                userId = (std::stoi(userIdStr) % 5);
//            } else if (edgeId == 2) { // 3 users connected
//                userId = ((std::stoi(userIdStr) - 4) % 4);
//            } else {
//                userId = ((std::stoi(userIdStr) - 7) % 4);
//            }
//            int userId = (std::stoi(userIdStr) % 2); // 2 users connected each node
//            userId = (userId == 1) ? 1 : 2;

//            EV << "The userIdStr is: " << userIdStr << ", so the gate is: " << userId << endl;

            int min = INT_MAX;
            std::string min_data = "";

            for(auto i: cachedData) {
                if (i.second.access_frequency < min) {
                    min = i.second.access_frequency;
                    min_data = i.first;
                }

                if (min == 0)
                    break;
            }

            // this is the auto delete
            cachedData.erase(min_data);
            std::string deletingBubble = "Deleting " + min_data;
            bubble(deletingBubble.c_str());

            // send a message to the replication manager telling it that you're deleting some data off of your cache
            DataItem data;
            data.id = dataId;
            data.type = rep_req->getType();
            data.size = rep_req->getSize();
            data.access_frequency = 1;

            cachedData[dataId] = data;

            SendingUserData *user_msg = new SendingUserData("sendingData");
            user_msg->setDataId(rep_req->getDataId());
            user_msg->setTimestamp(simTime());
            int data_size = data.size;
            float time = float(data_size) / 2000;

            // gate 1, because only one user is connected
            sendDelayed(user_msg, time, "gate$o", 1);

            // for multiple users
//            sendDelayed(user_msg, time, "gate$o", userId);

            std::string sendingDataString = "Sending " + dataId + " in " + std::to_string(time) + "s";
            EV << "Sending " << dataId << " from edge " << edgeId << " to user " << userIdStr << " in " << time << "s." << endl;
            bubble(sendingDataString.c_str());
        }
        else
        {
            DataRequest *ttmsg = check_and_cast<DataRequest *>(msg);
            std::string requestedData = ttmsg->getDataId();
            std::string userIdStr = ttmsg->getUserId();
            userIdStr = userIdStr.substr(1, userIdStr.length());
//            int userId = (std::stoi(userIdStr) % 2);
//            userId = (userId == 1) ? 1 : 2;

            // for 5-20 config
//            int userint = std::stoi(userIdStr);
//            int subt = 4 * (edgeId - 1);
//            int userId = (userint - subt) % 5;

//            // for 3:10 configuration
//            std::string userIdStr = ttmsg->getUserId();
//            userIdStr = userIdStr.substr(1, userIdStr.length());
//            int userId = 0;
//            if (edgeId == 1) { // because 4 users are connected
//                userId = (std::stoi(userIdStr) % 5);
//            } else if (edgeId == 2) { // 3 users connected
//                userId = ((std::stoi(userIdStr) - 4) % 4);
//            } else {
//                userId = ((std::stoi(userIdStr) - 7) % 4);
//            }

//            EV << "MY EDGE ID IS: " << edgeId << endl;
//            EV << "USER ID STRING: " << userIdStr << ", USER ID GATE: " << userId << endl;

            // if the edge node has the data
            if (cachedData.find(requestedData) != cachedData.end()) {
                // the edge node has the data
                cachedData[requestedData].access_frequency += 1;
                int data_size = cachedData[requestedData].size;
                float time = float(data_size) / 2000; // based on distance
                EV << "size is " << data_size << ", so should take " << time << "s.\n";
//                EV << "Sending from edge " << edgeId << " to user " << userIdStr << " through gate " << userId << endl;

                // this is the data we need to send back
                SendingUserData *msg = new SendingUserData("sendingData");
                msg->setDataId(requestedData.c_str());
                msg->setTimestamp(simTime());

//                sendDelayed(msg, time, "gate$o", userId);
                // this will get more complicated once multiple users connect to one edge
                sendDelayed(msg, time, "gate$o", 1); // gate 1, because only one user is connected
                EV << "Edge node has data " << requestedData << ", serving directly.\n";
                std::string sendingUserBubble = requestedData + " found. Sending in " \
                        + std::to_string(time) + "s";
                bubble(sendingUserBubble.c_str());

                std::string dest = "inform";
                char msgname[20];
                snprintf(msgname, sizeof(msgname), dest.c_str());

                // send a message to the replication manager for informing it
                ManagerMessage *inform = new ManagerMessage(msgname);
                inform->setEdgeId(edgeId);
                inform->setDataId(requestedData.c_str());
                inform->setUserId(ttmsg->getUserId());
//                if (edgeId == 1) {
//                    send(inform, "gate$o", 5);
//                } else {
//                    send(inform, "gate$o", 4);
//                }
//                send(inform, "gate$o", 3); // gate 3 is the replication manager for two users

//                send(inform, "gate$o", 5); // for 4 to each edge
                send(inform, "gate$o", 2); // for single user connected
                ++signalHaveData;
                emit(edgeHaveData, signalHaveData);

            } else { // the edge node doesn't have the data
                EV << "Data " << requestedData << " not found at edge node, forwarding to core node.\n";
                std::string askingCoreBubble = requestedData + " not found. Requesting Core...";
                bubble(askingCoreBubble.c_str());

                std::string dest = "edge_request";
                char msgname[20];
                snprintf(msgname, sizeof(msgname), dest.c_str());

                CoreRequest *c_req = new CoreRequest(msgname);
                c_req->setEdgeId(edgeId);
                c_req->setUserId(ttmsg->getUserId());
                c_req->setDataId(requestedData.c_str());

                send(c_req, "gate$o", 0); // gate 0 is core node

                ++signalRequestCore;
                emit(edgeRequestCore, signalRequestCore);
            }
        }

        if (signalHaveData + signalRequestCore > 0) {
            signalHitMissRatio = static_cast<double>(signalHaveData) / (signalHaveData + signalRequestCore);
            emit(edgeHitMissRatio, signalHitMissRatio);
        }

    }
};

Define_Module(EdgeNode);
