#ifndef TIMING_INCLUDE_TIMING_TOOLBOX_HXX_
#define TIMING_INCLUDE_TIMING_TOOLBOX_HXX_

#include <boost/lexical_cast.hpp>

namespace dunedaq::timing {

//-----------------------------------------------------------------------------
template<typename T>
struct stoul
{
  BOOST_STATIC_ASSERT((boost::is_unsigned<T>::value));
  T value;

  operator T() const { return value; }

  friend std::istream& operator>>(std::istream& in, stoul& out)
  {
    std::string buf;
    in >> buf;
    out.value = strtoul(buf.c_str(), NULL, 0);
    return in;
  }
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
struct stol
{
  BOOST_STATIC_ASSERT((boost::is_signed<T>::value));
  T value;

  operator T() const { return value; }

  friend std::istream& operator>>(std::istream& in, stol& out)
  {
    std::string buf;
    in >> buf;
    out.value = strtol(buf.c_str(), NULL, 0);
    return in;
  }
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
to_string(const T& v)
{
  return boost::lexical_cast<std::string>(v);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename M>
bool
map_value_comparator(typename M::value_type& p1, typename M::value_type& p2)
{
  return p1.second < p2.second;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::vector<T>
sanitize(const std::vector<T>& vec)
{
  // Sanitise the inputs, by copying
  std::vector<uint32_t> sorted(vec);

  // ...sorting...
  std::sort(sorted.begin(), sorted.end());

  // and delete the duplicates (erase+unique require a sorted vector to delete duplicates)
  sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());

  return sorted;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T, typename U>
T
safe_enum_cast(const U& value, const std::vector<T>& valid, const T& def)
{
  typename std::vector<T>::const_iterator it = std::find(valid.begin(), valid.end(), static_cast<T>(value));
  return (it != valid.end() ? *it : def);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename C>
std::string
join(const C& strings, const std::string& delimiter)
{

  if (strings.empty())
    return "";

  std::ostringstream string_stream;

  string_stream << *strings.begin();

  for (auto str_iter = std::next(strings.begin()); str_iter != strings.end(); ++str_iter) {
    string_stream << delimiter;
    string_stream << *str_iter;
  }

  return string_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class T>
std::string
format_reg_value(T reg_value, uint32_t base)
{
  std::stringstream value_stream;
  if (base == 16) {
    value_stream << std::showbase << std::hex;
  } else if (base == 10) {
    value_stream << std::dec;
  } else {
    // TODO warning?
    TLOG() << "Unsupported number base: " << base;
    value_stream << std::dec;
  }
  value_stream << reg_value;
  return value_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<>
inline std::string
format_reg_value(std::string reg_value, uint32_t /*base*/)
{
  return reg_value;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<>
inline std::string
format_reg_value(uhal::ValWord<uint32_t> reg_value, uint32_t base)
{
  std::stringstream value_stream;
  if (base == 16) {
    value_stream << std::showbase << std::hex;
  } else if (base == 10) {
    value_stream << std::dec;
  } else {
    // TODO warning?
    TLOG() << "format_reg_value: unsupported number base: " << base;
    value_stream << std::dec;
  }
  value_stream << reg_value.value();
  return value_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class T>
std::string
format_reg_table(T data, std::string title, std::vector<std::string> headers)
{

  uint32_t table_width = 7;
  uint32_t reg_column_width = 0;
  uint32_t val_column_width = 3;
  std::stringstream table_stream;

  for (auto it = data.begin(); it != data.end(); ++it) {
    reg_column_width = reg_column_width > it->first.size() ? reg_column_width : it->first.size();
    val_column_width =
      val_column_width > format_reg_value(it->second).size() ? val_column_width : format_reg_value(it->second).size();
  }

  // header vector length check
  reg_column_width = reg_column_width > headers.at(0).size() ? reg_column_width : headers.at(0).size();
  val_column_width = val_column_width > headers.at(1).size() ? val_column_width : headers.at(1).size();

  table_width = table_width + reg_column_width + val_column_width;

  if (title.size())
    table_stream << boost::format("%=s\n") % boost::io::group(std::setw(table_width), std::setfill('-'), title);

  if (headers.at(0).size() || headers.at(1).size()) {
    table_stream << boost::format("+-%=s-+-%=s-+\n") %
                      boost::io::group(std::setw(reg_column_width), std::setfill('-'), "") %
                      boost::io::group(std::setw(val_column_width), std::setfill('-'), "");
    table_stream << boost::format("| %=s | %=s |\n") % boost::io::group(std::setw(reg_column_width), headers.at(0)) %
                      boost::io::group(std::setw(val_column_width), headers.at(1));
  }

  table_stream << boost::format("+-%=s-+-%=s-+\n") %
                    boost::io::group(std::setw(reg_column_width), std::setfill('-'), "") %
                    boost::io::group(std::setw(val_column_width), std::setfill('-'), "");

  for (auto it = data.begin(); it != data.end(); ++it) {
    table_stream << boost::format("| %=s | %=s |\n") % boost::io::group(std::setw(reg_column_width), it->first) %
                      boost::io::group(std::setw(val_column_width), format_reg_value(it->second));
  }
  table_stream << boost::format("+-%=s-+-%=s-+\n") %
                    boost::io::group(std::setw(reg_column_width), std::setfill('-'), "") %
                    boost::io::group(std::setw(val_column_width), std::setfill('-'), "");

  return table_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<class T>
std::string
format_counters_table(std::vector<T> counter_nodes,
                      std::vector<std::string> counter_node_titles,
                      std::string table_title,
                      std::vector<std::string> counter_labels,
                      std::string counter_labels_header)
{

  uint32_t counter_nodes_number = counter_nodes.size();
  uint32_t table_width = 4 + (counter_nodes_number * 3);

  std::vector<std::string> counter_node_titles_to_use;

  if (!counter_node_titles.size()) {
    for (uint32_t i = 0; i < counter_nodes.size(); ++i) {
      counter_node_titles_to_use.push_back("Counters");
    }
  } else if (counter_nodes.size() != counter_node_titles.size()) {
    throw FormatCountersTableNodesTitlesMismatch(ERS_HERE);
  } else {
    counter_node_titles_to_use = counter_node_titles;
  }

  uint32_t counter_number;
  uint32_t counter_label_column_width = 0;

  std::stringstream table_stream;

  std::vector<std::string> counter_labels_to_use;
  if (counter_labels.size()) {
    counter_labels_to_use = counter_labels;
  } else {
    for (uint i = 0; i < g_command_number; ++i)
      counter_labels_to_use.push_back(str(static_cast<definitions::FixedLengthCommandType>(i)));
  }
  counter_number = counter_labels_to_use.size();

  for (auto it = counter_labels_to_use.begin(); it != counter_labels_to_use.end(); ++it) {
    counter_label_column_width = counter_label_column_width > it->size() ? counter_label_column_width : it->size();
  }
  counter_label_column_width =
    counter_label_column_width > counter_labels_header.size() ? counter_label_column_width : counter_labels_header.size();

  typedef std::vector<std::pair<std::string, std::string>> CounterValuesContainer;

  std::vector<CounterValuesContainer> counter_value_containers;
  std::vector<std::pair<uint32_t, uint32_t>> counter_value_column_widths;

  for (auto node_iter = counter_nodes.begin(); node_iter != counter_nodes.end(); ++node_iter) {

    CounterValuesContainer counter_values;

    uint32_t counter_value_dec_column_width = 5;
    uint32_t counter_value_hex_column_width = 5;

    for (auto counter_iter = node_iter->begin(); counter_iter != node_iter->end(); ++counter_iter) {

      std::string counter_value_dec = format_reg_value(*counter_iter, 10);
      std::string counter_value_hex = format_reg_value(*counter_iter, 16);

      counter_value_dec_column_width =
        counter_value_dec_column_width > counter_value_dec.size() ? counter_value_dec_column_width : counter_value_dec.size();
      counter_value_hex_column_width =
        counter_value_hex_column_width > counter_value_hex.size() ? counter_value_hex_column_width : counter_value_hex.size();

      counter_values.push_back(std::make_pair(counter_value_dec, counter_value_hex));
    }

    counter_value_containers.push_back(counter_values);
    counter_value_column_widths.push_back(std::make_pair(counter_value_dec_column_width, counter_value_hex_column_width));
  }

  std::vector<uint32_t> counter_node_title_sizes;
  // titles and border
  std::stringstream counter_titles_row;
  counter_titles_row << boost::format("| %=s |") % boost::io::group(std::setw(counter_label_column_width), "");
  table_width = table_width + counter_label_column_width;
  for (uint32_t i = 0; i < counter_nodes_number; ++i) {
    uint32_t dec_width = counter_value_column_widths.at(i).first;
    uint32_t hex_width = counter_value_column_widths.at(i).second;

    uint32_t counter_title_size = counter_node_titles_to_use.at(i).size();

    if (counter_title_size > (dec_width + hex_width + 3)) {

      if ((counter_title_size - 3) % 2)
        ++counter_title_size;

      counter_value_column_widths.at(i).first = (counter_title_size - 3) / 2;
      counter_value_column_widths.at(i).second = (counter_title_size - 3) / 2;

    } else {
      counter_title_size = (dec_width + hex_width + 3);
    }
    counter_titles_row << boost::format(" %=s |") %
                           boost::io::group(std::setw(counter_title_size), counter_node_titles_to_use.at(i));
    counter_node_title_sizes.push_back(counter_title_size);
    table_width = table_width + counter_title_size;
  }
  counter_titles_row << std::endl;

  std::stringstream title_row_border;
  title_row_border << boost::format("+-%=s-+") %
                       boost::io::group(std::setw(counter_label_column_width), std::setfill('-'), "");
  for (uint32_t i = 0; i < counter_nodes_number; ++i) {
    title_row_border << boost::format("-%=s-+") %
                         boost::io::group(std::setw(counter_node_title_sizes.at(i)), std::setfill('-'), "");
  }
  title_row_border << std::endl;

  if (table_title.size())
    table_stream << boost::format("%=s\n") % boost::io::group(std::setw(table_width), std::setfill('-'), table_title);

  table_stream << title_row_border.str();
  table_stream << counter_titles_row.str();
  table_stream << title_row_border.str();
  //

  // headers
  std::stringstream counter_headers;
  counter_headers << boost::format("| %=s |") %
                       boost::io::group(std::setw(counter_label_column_width), counter_labels_header);
  for (uint32_t j = 0; j < counter_nodes_number; ++j) {
    uint32_t dec_width = counter_value_column_widths.at(j).first;
    uint32_t hex_width = counter_value_column_widths.at(j).second;
    counter_headers << boost::format(" %=s | %=s |") % boost::io::group(std::setw(dec_width), "cnts") %
                         boost::io::group(std::setw(hex_width), "hex");
  }
  table_stream << counter_headers.str() << std::endl;
  //

  // top counter row border
  std::stringstream row_border;
  row_border << boost::format("+-%=s-+") % boost::io::group(std::setw(counter_label_column_width), std::setfill('-'), "");
  for (uint32_t j = 0; j < counter_nodes_number; ++j) {
    uint32_t dec_width = counter_value_column_widths.at(j).first;
    uint32_t hex_width = counter_value_column_widths.at(j).second;
    row_border << boost::format("-%=s-+-%=s-+") % boost::io::group(std::setw(dec_width), std::setfill('-'), "") %
                    boost::io::group(std::setw(hex_width), std::setfill('-'), "");
  }
  row_border << std::endl;
  table_stream << row_border.str();
  //

  // counter rows
  for (uint32_t i = 0; i < counter_number; ++i) {
    std::stringstream table_row_stream;

    table_row_stream << boost::format("| %=s |") %
                         boost::io::group(std::setw(counter_label_column_width), counter_labels_to_use.at(i));

    for (uint32_t j = 0; j < counter_nodes_number; ++j) {
      uint32_t dec_width = counter_value_column_widths.at(j).first;
      uint32_t hex_width = counter_value_column_widths.at(j).second;

      std::string dec_value = counter_value_containers.at(j).at(i).first;
      std::string hex_value = counter_value_containers.at(j).at(i).second;

      table_row_stream << boost::format(" %=s | %=s |") % boost::io::group(std::setw(dec_width), dec_value) %
                           boost::io::group(std::setw(hex_width), hex_value);
    }
    table_stream << table_row_stream.str() << std::endl;
  }
  //

  // bottom counter row border
  table_stream << row_border.str();

  return table_stream.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
vec_fmt(const std::vector<T>& vec)
{
  std::ostringstream oss;
  oss << "[";

  for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); it++)
    oss << *it << ",";
  oss.seekp(oss.tellp() - 1l);
  oss << "]";

  return oss.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template<typename T>
std::string
short_vec_fmt(const std::vector<T>& vec)
{
  if (vec.size() == 0)
    return "[]";
  else if (vec.size() == 1)
    return "[" + boost::lexical_cast<std::string>(vec.at(0)) + "]";

  std::ostringstream oss;
  oss << "[";

  // Initial search range
  T first = vec.at(0);
  T last = first;
  for (typename std::vector<T>::const_iterator it = vec.begin() + 1; it != vec.end(); it++) {
    // if *it is contiguous to last, carry on searching
    if ((*it) == (last + 1)) {
      last = *it;
      continue;
    }

    if (first == last)
      // if first and last are the same, this is an isolated number
      oss << first << ",";
    else
      // otherwise it's a range
      oss << first << "-" << last << ",";

    // *it is the first value of the new range
    first = *it;
    last = *it;
  }

  // Process the last range
  if (first == last)
    oss << first << ",";
  else
    oss << first << "-" << last << ",";

  // Is this actually necessary?
  // Replace final "," with a "]"
  oss.seekp(oss.tellp() - 1l);
  oss << "]";

  return oss.str();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
//-----------------------------------------------------------------------------
} // namespace dunedaq::timing

#endif // TIMING_INCLUDE_TIMING_TOOLBOX_HXX_