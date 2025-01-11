#include <omnetpp.h>
#include <unordered_map>
#include <string>
#include <vector>

using namespace omnetpp;

class User : public cSimpleModule {
private:
    std::string userId;            // Unique ID for the user
    std::vector<std::string> dataItems; // List of available data items to request
    long signalRequested = 0, dataReceived = 0;
    simsignal_t userRequestedData;
    simsignal_t userDataReceived;
    simsignal_t userRequestDelay;

    std::unordered_map<std::string, simtime_t> requests;


protected:
    virtual void initialize() override;   // Initialization logic
    virtual void handleMessage(cMessage* msg) override; // Handle incoming messages
    void sendDataRequest();       // Send a data request to the connected EdgeNode

public:
    User();  // Constructor
    virtual ~User(); // Destructor
};
