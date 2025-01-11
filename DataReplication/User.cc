#include "User.h"
#include "json.hpp"
#include "DataRequest_m.h"
#include "SendingUserData_m.h"
#include <fstream>
#include <omnetpp.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace omnetpp;

Define_Module(User);

User::User() {}

User::~User() {}

void User::initialize() {
    // initialize the signals for getting metrics
    userRequestedData = registerSignal("userRequestedData");
    userDataReceived = registerSignal("userDataReceived");
    userRequestDelay = registerSignal("userRequestDelay");

    // Assign a unique userId from parameters
    userId = par("userId").stdstringValue();

    // Initialize available data items (you can later load these from a JSON file)
    std::ifstream fJson("synthetic_data.json");
    std::stringstream buffer;
    buffer << fJson.rdbuf();
    auto json = nlohmann::json::parse(buffer.str());

    for (auto item: json) {
        std::string dataId = item["id"];
        dataId = dataId.substr(0, 1);
        if (std::strcmp("d", dataId.c_str()) == 0) {
            EV << "user loaded with data of type " << dataId << endl;
            dataItems.push_back(item["id"]);
        }
    }

    // Schedule first data request
    scheduleAt(simTime() + 0.1, new cMessage("sendRequest"));
}

void User::handleMessage(cMessage* msg) {
    if (std::strcmp(msg->getName(), "sendingData") == 0) {
        SendingUserData *received = check_and_cast<SendingUserData *>(msg);
        std::string receivedData = received->getDataId();
        std::string receivedBubble = "Received " + receivedData;
        bubble(receivedBubble.c_str());
        EV << "USER " << userId << " RECEIVED DATA " << receivedData;

        // latency metric
        simtime_t receiveTime = simTime();
        simtime_t sendTime = requests[receivedData];
        simtime_t delay = receiveTime - sendTime;
        emit(userRequestDelay, delay);

        EV << "The Send Time directly stored is: " << requests[receivedData] << endl;
        EV << ", Send Time recorded: " << sendTime << ", Receive Time: " << receiveTime << ", Delay: " << delay << endl;

        // delete from our requests map
        requests.erase(receivedData);

        ++dataReceived;
        emit(userDataReceived, dataReceived);
    } else {
        sendDataRequest();

        // Schedule the next request
        float random_skip = float(intuniform(0, 1000)) / 500;
        EV << "Random skip of : " << random_skip << "s";

        scheduleAt(simTime() + random_skip, new cMessage("sendRequest"));

        ++signalRequested;
        emit(userRequestedData, signalRequested);
    }

    delete msg;
}

void User::sendDataRequest() {
    if (dataItems.empty()) {
        EV << "No data items available to request.\n";
        return;
    }

    // Randomly select a data item to request
    std::string requestedData = dataItems[intuniform(0, dataItems.size() - 1)];
    std::string dest = "u_req_" + requestedData;
    requests[requestedData] = simTime();
    simtime_t test = simTime();
    EV << "USER " << userId << " SENDING ANOTHER REQUEST! SIMTIME IS: " << simTime() << ", and the number stored is: " << test << endl;
    EV << "stored send time in the user records is: " << requests[requestedData] << endl;

    // Create and send custom defined message
    char msgname[20];
    snprintf(msgname, sizeof(msgname), dest.c_str());

    DataRequest *request = new DataRequest(msgname);
    request->setDataId(requestedData.c_str());
    request->setUserId(userId.c_str());
    send(request, "gate$o", 0);

    std::string askingFor = "Requesting " + requestedData;
    bubble(askingFor.c_str());
    EV << "User " << userId << " sent request for data: " << requestedData << "\n";
}
