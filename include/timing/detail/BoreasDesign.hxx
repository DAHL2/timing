#include <sstream>
#include <string>

namespace dunedaq::timing {

// In leiu of UHAL_REGISTER_DERIVED_NODE
//-----------------------------------------------------------------------------
template<class IO>
uhal::Node*
BoreasDesign<IO>::clone() const
{
  return new BoreasDesign<IO>(static_cast<const BoreasDesign<IO>&>(*this));
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
BoreasDesign<IO>::BoreasDesign(const uhal::Node& node)
  : TopDesign<IO>(node)
  , MasterDesign<IO, PDIMasterNode>(node)
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
BoreasDesign<IO>::~BoreasDesign()
{}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
std::string
BoreasDesign<IO>::get_status(bool print_out) const
{
  std::stringstream status;
  status << this->get_io_node().get_pll_status();
  status << this->get_master_node().get_status();
  status << this->get_hsi_node().get_status();
  if (print_out)
    TLOG() << status.str();
  return status.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
BoreasDesign<IO>::configure() const
{

  // Hard resets
  this->reset();

  // Set timestamp to current time
  this->sync_timestamp();

  // configure hsi
  // this->get_his_node().
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
BoreasDesign<IO>::configure_hsi(uint32_t src,      // NOLINT(build/unsigned)
                                uint32_t re_mask,  // NOLINT(build/unsigned)
                                uint32_t fe_mask,  // NOLINT(build/unsigned)
                                uint32_t inv_mask, // NOLINT(build/unsigned)
                                double rate,
                                bool dispatch) const
{
  uint32_t firmware_frequency = this->get_io_node().read_firmware_frequency();
  this->get_hsi_node().configure_hsi(src, re_mask, fe_mask, inv_mask, rate, firmware_frequency, dispatch);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class IO>
void
BoreasDesign<IO>::get_info(opmonlib::InfoCollector& ci, int level) const
{ 
  opmonlib::InfoCollector master_collector;
  this->get_master_node().get_info(master_collector, level);
  ci.add("master", master_collector);

  opmonlib::InfoCollector hardware_collector;
  this->get_io_node().get_info(hardware_collector, level);
  ci.add("io", hardware_collector);

  opmonlib::InfoCollector hsi_collector;
  this->get_hsi_node().get_info(hsi_collector, level);
  ci.add("hsi", hsi_collector);
}
//-----------------------------------------------------------------------------
}