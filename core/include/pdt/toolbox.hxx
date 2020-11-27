#ifndef __PDT_TOOLBOX_HXX__
#define __PDT_TOOLBOX_HXX__

#include <boost/lexical_cast.hpp>

namespace pdt {


//-----------------------------------------------------------------------------
template < typename T >
struct stoul {
    BOOST_STATIC_ASSERT((boost::is_unsigned<T>::value));
    T value;

    operator T() const {
        return value;
    }

    friend std::istream& operator>>(std::istream& in, stoul& out) {
        std::string buf;
        in>>buf;
        out.value = strtoul(buf.c_str(), NULL, 0);
        return in;
    }
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template < typename T >
struct stol {
    BOOST_STATIC_ASSERT((boost::is_signed<T>::value));
    T value;

    operator T() const {
        return value;
    }

    friend std::istream& operator>>(std::istream& in, stol& out) {
        std::string buf;
        in>>buf;
        out.value = strtol(buf.c_str(), NULL, 0);
        return in;
    }
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template < typename T >
std::string
to_string(const T& v) {
    return boost::lexical_cast<std::string>(v);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template < typename M >
bool map_value_comparator( typename M::value_type &p1, typename M::value_type &p2){
    return p1.second < p2.second;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< typename T>
std::vector<T> sanitize( const std::vector<T>& vec ) {
  // Sanitise the inputs, by copying
  std::vector<uint32_t> sorted(vec);
 
  // ...sorting...
  std::sort(sorted.begin(), sorted.end());
  
  // and delete the duplicates (erase+unique require a sorted vector to delete duplicates)
  sorted.erase( std::unique(sorted.begin(), sorted.end()), sorted.end());

  return sorted;
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template< typename T, typename U>
T safe_enum_cast(const U& value, const std::vector<T>& valid, const T& def) {
    typename std::vector<T>::const_iterator it = std::find(valid.begin(), valid.end(), static_cast<T>(value));
    return ( it != valid.end() ? *it : def );
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template<typename C>
std::string join(const C& aStrings, const std::string& aDelimiter)
{

  if ( aStrings.empty() ) return "";

  std::ostringstream lString;

  lString << *aStrings.begin();

  for(auto iStr = std::next(aStrings.begin()); iStr != aStrings.end(); ++iStr) {
    lString << aDelimiter; 
    lString << *iStr;
  }

  return lString.str();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
template <class T>
std::string
formatRegValue(T regValue, uint32_t base) {
  std::stringstream lValueStream;
  if (base == 16) {
    lValueStream << "0x" << std::hex;
  } else if (base == 10) {
    lValueStream << std::dec;
  } else {
    PDT_LOG(kWarning) << "Unsupported number base: " << base << std::endl;
    lValueStream << std::dec;
  }
  lValueStream << regValue;
  return lValueStream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class T>
std::string
formatRegTable(T data, std::string title, std::vector<std::string> headers) {
  
  uint32_t lTableWidth = 7;
  uint32_t lRegColumnWidth = 0;
  uint32_t lValColumnWidth = 3;
  std::stringstream lTableStream;

  for (auto it=data.begin(); it!=data.end(); ++it) {
    lRegColumnWidth = lRegColumnWidth > it->first.size() ? lRegColumnWidth : it->first.size();
    lValColumnWidth = lValColumnWidth > formatRegValue(it->second).size() ? lValColumnWidth : formatRegValue(it->second).size();
  }

  // header vector length check
  lRegColumnWidth = lRegColumnWidth > headers.at(0).size() ? lRegColumnWidth : headers.at(0).size();
  lValColumnWidth = lValColumnWidth > headers.at(1).size() ? lValColumnWidth : headers.at(1).size();
  
  lTableWidth = lTableWidth + lRegColumnWidth + lValColumnWidth;

  if (title.size()) lTableStream << boost::format("%=s\n") % boost::io::group(std::setw(lTableWidth), std::setfill('-'), title);
  
  if (headers.at(0).size() || headers.at(1).size()) {
    lTableStream << boost::format("+-%=s-+-%=s-+\n") % boost::io::group(std::setw(lRegColumnWidth), std::setfill('-'), "")  % boost::io::group(std::setw(lValColumnWidth), std::setfill('-'), "");
    lTableStream << boost::format("| %=s | %=s |\n") % boost::io::group(std::setw(lRegColumnWidth), headers.at(0)) % boost::io::group(std::setw(lValColumnWidth), headers.at(1));
  }
  
  lTableStream << boost::format("+-%=s-+-%=s-+\n") % boost::io::group(std::setw(lRegColumnWidth), std::setfill('-'), "")  % boost::io::group(std::setw(lValColumnWidth), std::setfill('-'), "");

  for (auto it=data.begin(); it!=data.end(); ++it) {
    lTableStream << boost::format("| %=s | %=s |\n") % boost::io::group(std::setw(lRegColumnWidth), it->first) % boost::io::group(std::setw(lValColumnWidth), formatRegValue(it->second));
  }
  lTableStream << boost::format("+-%=s-+-%=s-+\n") % boost::io::group(std::setw(lRegColumnWidth), std::setfill('-'), "")  % boost::io::group(std::setw(lValColumnWidth), std::setfill('-'), "");

  return lTableStream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class T>
std::string formatCountersTable(std::vector<T> aCounterNodes, std::vector<std::string> aCounterNodeTitles, std::string aTableTitle, std::vector<std::string> aCounterLabels, std::string aCounterLabelsHeader) {

  uint32_t lCounterNodesNumber = aCounterNodes.size();
  uint32_t lTableWidth = 4+(lCounterNodesNumber*3);

  std::vector<std::string> lCounterNodeTitles;

  if (!aCounterNodeTitles.size()) {
    for (uint32_t i=0; i < aCounterNodes.size(); ++i) {
      lCounterNodeTitles.push_back("Counters");
    }
  } else if (aCounterNodes.size() != aCounterNodeTitles.size()) {
    std::ostringstream lMsg;
    lMsg << "Format counters table, counter nodes vs. title number mismatch";
    throw FormatCountersTableNodesTitlesMismatch(lMsg.str());
  } else {
    lCounterNodeTitles = aCounterNodeTitles;
  }

  uint32_t lCounterNumber;
  uint32_t lCounterLabelColumnWidth = 0;

  std::stringstream lTableStream;

  std::vector<std::string> lCounterLabels;
  if (aCounterLabels.size()) {
    lCounterLabels = aCounterLabels;
  } else {
    for (auto it=kCommandMap.begin(); it != kCommandMap.end(); ++it) lCounterLabels.push_back(it->second);
  }
  lCounterNumber = lCounterLabels.size();

  for (auto it=lCounterLabels.begin(); it != lCounterLabels.end(); ++it) {
    lCounterLabelColumnWidth = lCounterLabelColumnWidth > it->size() ? lCounterLabelColumnWidth : it->size();
  }
  lCounterLabelColumnWidth = lCounterLabelColumnWidth > aCounterLabelsHeader.size() ? lCounterLabelColumnWidth : aCounterLabelsHeader.size();

  typedef std::vector<std::pair<std::string, std::string>> CounterValuesContainer;

  std::vector<CounterValuesContainer> lCounterValueContainers;
  std::vector<std::pair<uint32_t, uint32_t>> lCounterValueColumnWidths;

  for (auto nodeIt=aCounterNodes.begin(); nodeIt!=aCounterNodes.end(); ++nodeIt) {

    CounterValuesContainer lCounterValues;

    uint32_t lCounterValueDecColumnWidth = 5;
    uint32_t lCounterValueHexColumnWidth = 5;

    for (auto counterIt=nodeIt->begin(); counterIt != nodeIt->end(); ++counterIt) {

      std::string lCounterValueDec = formatRegValue(*counterIt,10);
      std::string lCounterValueHex = formatRegValue(*counterIt,16);

      lCounterValueDecColumnWidth = lCounterValueDecColumnWidth > lCounterValueDec.size() ? lCounterValueDecColumnWidth : lCounterValueDec.size();
      lCounterValueHexColumnWidth = lCounterValueHexColumnWidth > lCounterValueHex.size() ? lCounterValueHexColumnWidth : lCounterValueHex.size();

      lCounterValues.push_back(std::make_pair(lCounterValueDec, lCounterValueHex));
    }

    lCounterValueContainers.push_back(lCounterValues);
    lCounterValueColumnWidths.push_back(std::make_pair(lCounterValueDecColumnWidth, lCounterValueHexColumnWidth));
  }

  std::vector<uint32_t> lCounterNodeTitleSizes;
  // titles and border
  std::stringstream lCounterTitlesRow;
  lCounterTitlesRow << boost::format("| %=s |") % boost::io::group(std::setw(lCounterLabelColumnWidth), "");
  lTableWidth = lTableWidth + lCounterLabelColumnWidth;
  for (uint32_t i=0; i < lCounterNodesNumber; ++i) {
    uint32_t lDecWidth = lCounterValueColumnWidths.at(i).first;
    uint32_t lHexWidth = lCounterValueColumnWidths.at(i).second;

    uint32_t lCounterTitleSize = lCounterNodeTitles.at(i).size();

    if (lCounterTitleSize > (lDecWidth+lHexWidth+3)) {

      if ((lCounterTitleSize-3)%2) ++lCounterTitleSize;
      
      lCounterValueColumnWidths.at(i).first  = (lCounterTitleSize-3)/2;
      lCounterValueColumnWidths.at(i).second = (lCounterTitleSize-3)/2;
  
    } else {
      lCounterTitleSize = (lDecWidth+lHexWidth+3);
    }
    lCounterTitlesRow << boost::format(" %=s |") % boost::io::group(std::setw(lCounterTitleSize), lCounterNodeTitles.at(i));
    lCounterNodeTitleSizes.push_back(lCounterTitleSize);
    lTableWidth = lTableWidth + lCounterTitleSize;
  }
  lCounterTitlesRow << std::endl;
  
  std::stringstream lTitleRowBorder;
  lTitleRowBorder << boost::format("+-%=s-+") % boost::io::group(std::setw(lCounterLabelColumnWidth), std::setfill('-'), "");
  for (uint32_t i=0; i < lCounterNodesNumber; ++i) {
    lTitleRowBorder << boost::format("-%=s-+") % boost::io::group(std::setw(lCounterNodeTitleSizes.at(i)), std::setfill('-'), "");
  }
  lTitleRowBorder << std::endl;

  if (aTableTitle.size()) lTableStream << boost::format("%=s\n") % boost::io::group(std::setw(lTableWidth), std::setfill('-'), aTableTitle);

  lTableStream << lTitleRowBorder.str();
  lTableStream << lCounterTitlesRow.str();
  lTableStream << lTitleRowBorder.str();
  //

  // headers
  std::stringstream lCounterHeaders;
  lCounterHeaders << boost::format("| %=s |") % boost::io::group(std::setw(lCounterLabelColumnWidth), aCounterLabelsHeader);
  for (uint32_t j=0; j < lCounterNodesNumber; ++j) {
      uint32_t lDecWidth = lCounterValueColumnWidths.at(j).first;
      uint32_t lHexWidth = lCounterValueColumnWidths.at(j).second;
      lCounterHeaders << boost::format(" %=s | %=s |") % boost::io::group(std::setw(lDecWidth), "cnts") % boost::io::group(std::setw(lHexWidth), "hex");
  }
  lTableStream << lCounterHeaders.str() << std::endl;
  //

  // top counter row border
  std::stringstream lRowBorder;
  lRowBorder << boost::format("+-%=s-+") % boost::io::group(std::setw(lCounterLabelColumnWidth), std::setfill('-'), "");
  for (uint32_t j=0; j < lCounterNodesNumber; ++j) {
      uint32_t lDecWidth = lCounterValueColumnWidths.at(j).first;
      uint32_t lHexWidth = lCounterValueColumnWidths.at(j).second;
      lRowBorder << boost::format("-%=s-+-%=s-+") % boost::io::group(std::setw(lDecWidth), std::setfill('-'), "")  % boost::io::group(std::setw(lHexWidth), std::setfill('-'), "");
    }
  lRowBorder << std::endl;
  lTableStream << lRowBorder.str();
  //

  // counter rows
  for (uint32_t i=0; i < lCounterNumber; ++i) {
    std::stringstream lTableRowStream;

    lTableRowStream << boost::format("| %=s |") % boost::io::group(std::setw(lCounterLabelColumnWidth), lCounterLabels.at(i));

    for (uint32_t j=0; j < lCounterNodesNumber; ++j) {
      uint32_t lDecWidth = lCounterValueColumnWidths.at(j).first;
      uint32_t lHexWidth = lCounterValueColumnWidths.at(j).second;

      std::string lDecValue = lCounterValueContainers.at(j).at(i).first;
      std::string lHexValue = lCounterValueContainers.at(j).at(i).second;

      lTableRowStream << boost::format(" %=s | %=s |") % boost::io::group(std::setw(lDecWidth), lDecValue) % boost::io::group(std::setw(lHexWidth), lHexValue);

    }
    lTableStream << lTableRowStream.str() << std::endl;
  }
  //

  // bottom counter row border
  lTableStream << lRowBorder.str();
  
  return lTableStream.str();
}
//-----------------------------------------------------------------------------

} // namespace pdt


#endif /* _mp7_helpers_hpp_ */
