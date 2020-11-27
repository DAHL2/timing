#ifndef __PDT_MASTERNODE_HPP__
#define __PDT_MASTERNODE_HPP__

// C++ Headers
#include <chrono>

// uHal Headers
#include "uhal/DerivedNode.hpp"

// PDT Headers
#include "pdt/TimingNode.hpp"
#include "pdt/exception.hpp"
#include "pdt/TimestampGeneratorNode.hpp"
#include "pdt/VLCmdGeneratorNode.hpp"
#include "pdt/GlobalNode.hpp"
#include "pdt/EchoMonitorNode.hpp"
#include "pdt/FLCmdGeneratorNode.hpp"
#include "pdt/PartitionNode.hpp"

namespace pdt {

/**
 * @brief      Base class for timing IO nodes.
 */
class MasterNode : public TimingNode {
public:
    MasterNode(const uhal::Node& aNode);
    virtual ~MasterNode();
    
    /**
     * @brief      Read the current timestamp word.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint64_t readTimestamp() const;

    /**
     * @brief      Set the timestamp to current time.
     */
    virtual void setTimestamp(uint64_t aTimestamp) const;

    /**
     * @brief     Control the tx line of endpoint sfp
     */
    virtual void switchEndpointSFP(uint32_t aAddr, bool aOn) const = 0;

    /**
     * @brief     Enable RTT endpoint
     */
    virtual bool enableUpstreamEndpoint() const = 0;

    /**
     * @brief      Measure the endpoint round trip time.
     *
     * @return     { description_of_the_return_value }
     */
    virtual uint32_t measureEndpointRTT(uint32_t aAddr, bool aControlSFP=true) const = 0;
    
    /**
     * @brief     Apply delay to endpoint
     */
    virtual void applyEndpointDelay(uint32_t aAddr, uint32_t aCDel, uint32_t aFDel, uint32_t aPDel, bool aMeasureRTT=false, bool aControlSFP=true) const = 0;

    /**
     * @brief     Apply delay to endpoint
     */
    virtual void applyEndpointDelay(const ActiveEndpointConfig& aEptConfig, bool aMeasureRTT=false) const;

    /**
     * @brief     Send a fixed length command
     */
    virtual void sendFLCmd(uint32_t aCmd, uint32_t aChan, uint32_t aNumber=1) const = 0;

    /**
     * @brief      Get partition node
     *
     * @return     { description_of_the_return_value }
     */
    virtual const PartitionNode& getPartitionNode(uint32_t aPartID) const;
};


} // namespace pdt

#endif // __PDT_MASTERNODE_HPP__