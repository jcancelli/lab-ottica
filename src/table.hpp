#pragma once

#include <array>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

// utility functions
static inline std::string padRight(std::string str, int spaces) {
  return str + std::string(spaces, ' ');
}

static inline std::string padLeft(std::string str, int spaces) {
  return std::string(spaces, ' ') + str;
}

static inline std::string padCenter(std::string str, int spaces) {
  return std::string(std::floor(spaces / 2.), ' ') + str +
         std::string(std::ceil(spaces / 2.), ' ');
}

enum class Align { LEFT, CENTER, RIGHT };

template <class... ColTypes>
class Table {
 public:
  static const int columns = sizeof...(ColTypes);

 private:
  std::array<std::string, columns> m_Headers;
  std::array<int, columns> m_ColumnsWidths;
  std::vector<std::array<std::string, columns>> m_Rows;
  int m_ColumnSpacing = 1;

 public:
  void print(Align alignment = Align::LEFT) const {
    std::function<std::string(std::string, int)> align;
    std::string spacing(m_ColumnSpacing, ' ');
    switch (alignment) {
      case Align::LEFT:
        align = padRight;
        break;
      case Align::RIGHT:
        align = padLeft;
        break;
      case Align::CENTER:
        align = padCenter;
        break;
    }
    // print headers
    for (int i = 0; i < columns; i++) {
      std::cout << align(m_Headers[i], m_ColumnsWidths[i] - m_Headers[i].size())
                << spacing;
    }
    std::cout << "\n";
    // print table body
    for (auto const& row : m_Rows) {
      for (int i = 0; i < columns; i++) {
        std::cout << align(row[i], m_ColumnsWidths[i] - row[i].size())
                  << spacing;
      }
      std::cout << "\n";
    }
  }

  Table& headers(std::array<std::string, columns> headers) {
    if (headers.size() != columns) {
      throw std::invalid_argument(
          "Number of headers and number of columns must match");
    }
    int i = 0;
    for (auto& header : headers) {
      m_Headers[i] = header;
      // update column width
      if ((int)header.size() > m_ColumnsWidths[i]) {
        m_ColumnsWidths[i] = header.size();
      }
      ++i;
    }
    return *this;
  }

  Table& row(ColTypes... row) {
    std::array<std::string, columns> stringValues;
    int i = 0;
    (
        [&] {
          // convert to string
          if constexpr (std::is_same_v<std::string, ColTypes> ||
                        std::is_same_v<const char*, ColTypes>) {
            stringValues[i] = row;
          } else {
            stringValues[i] = std::to_string(row);
          }
          // update column width
          if ((int)stringValues[i].size() > this->m_ColumnsWidths[i]) {
            this->m_ColumnsWidths[i] = stringValues[i].size();
          }
          i++;
        }(),
        ...);
    m_Rows.push_back(stringValues);
    return *this;
  }

  Table& spacing(int n) {
    if (n < 1) {
      throw std::invalid_argument("Spacing cannot be less than 1");
    }
    m_ColumnSpacing = n;
    return *this;
  }

  int getSpacing() const {
    return m_ColumnSpacing;
  }
};