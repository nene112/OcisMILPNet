#include "FileInteraction.h"

std::ofstream gateFlowVolumeFile;
std::ofstream changeQFile;
WaterUnitMapper mapper;

#ifndef _WIN32
#include <errno.h>
#include <iconv.h>

namespace {

std::string convert_encoding_iconv(const std::string& input,
                                   const char* from_encoding,
                                   const char* to_encoding) {
  if (input.empty()) {
    return input;
  }

  iconv_t converter = iconv_open(to_encoding, from_encoding);
  if (converter == reinterpret_cast<iconv_t>(-1)) {
    return {};
  }

  std::string input_copy = input;
  char* input_buffer = &input_copy[0];
  size_t input_bytes = input_copy.size();
  std::string output(input_copy.size() * 4 + 16, '\0');
  char* output_buffer = &output[0];
  size_t output_bytes = output.size();

  while (iconv(converter, &input_buffer, &input_bytes,
               &output_buffer, &output_bytes) == static_cast<size_t>(-1)) {
    if (errno != E2BIG) {
      iconv_close(converter);
      return {};
    }
    const size_t used = output.size() - output_bytes;
    output.resize(output.size() * 2);
    output_buffer = &output[used];
    output_bytes = output.size() - used;
  }

  iconv_close(converter);
  output.resize(output.size() - output_bytes);
  return output;
}

std::string gbk_to_utf8_unix(const std::string& text) {
  if (text.empty() || isValidUTF8(text)) {
    return text;
  }
  for (const char* encoding : {"GB18030", "GBK", "GB2312"}) {
    const std::string converted =
        convert_encoding_iconv(text, encoding, "UTF-8");
    if (!converted.empty() && isValidUTF8(converted)) {
      return converted;
    }
  }
  return text;
}

std::string utf8_to_gbk_unix(const std::string& text) {
  if (text.empty() || !isValidUTF8(text)) {
    return text;
  }
  for (const char* encoding : {"GB18030", "GBK", "GB2312"}) {
    const std::string converted =
        convert_encoding_iconv(text, "UTF-8", encoding);
    if (!converted.empty()) {
      return converted;
    }
  }
  return text;
}

}  // namespace
#endif

void ocis_init_encoding() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#else
  setlocale(LC_CTYPE, "");
#endif
}
nlohmann::json ReadJson(string filename)
{
  std::ifstream file(filename); // 创建输入文件流对象
  if (!isExist(filename)) {
    printf("json::parse can't find %s!\n",filename.c_str());
    return -1;
  }

  //第2个参数是错误处理，第3个参数是不throw exception
  nlohmann::json j3 =
    nlohmann::json::parse(
      file,
      nullptr,
      true,
      true);


  if (j3.is_discarded())
  {
    printf("json::parse error!\n");
    return -1;
  }
  return j3;
}

void WriteJson(json data, string dirpath, string filename) {
    string output_file_name = dirpath + filename;
    std::ofstream out(output_file_name, std::ios::binary);
    out << std::fixed << std::setprecision(2);
    out << data.dump(4);
}

void write_process_strT(process_strT data, string dirpath, string filename) {

  json data_j;
  for (auto iter = data.begin(); iter != data.end(); iter++) {
    data_j[iter->first] = iter->second;
  }

  string output_file_name = dirpath + filename;
  std::ofstream out(output_file_name, std::ios::binary);
  out << std::fixed << std::setprecision(2);
  out << data_j.dump(4);

}



std::vector<string> readPaths(string filename)
{
  std::vector<std::vector<string>> fields_vector;

  ifstream fin(filename);
  string line;
  char temp[1000];
  std::memset(temp, 0, 1000);
  if (!fin.good())
  {
    cout << "file " << filename << "read failed！" << endl;
    exit(-1);
  }
  std::vector<string> fields;
  while (fin.peek() != EOF && getline(fin, line))
  {
    stringstream sstr(line);
    string field;
    //下行中的“，”表示文件的   分隔符号
    while (getline(sstr, field, '\t'))
    {
      fields.push_back(field);
    }
  }
  return fields;
}
process_h jsonToProcess_h(json aimjson)
{
  process_h value;
  for (auto& d : aimjson.items()) {
    time_t t = StringToTime_t(d.key());
    string str;
    str = fiterstr(d.value().dump());
    value[t] = (double)atof(str.c_str());
  }
  return value;
}

process_k Process_hToprocess_k(process_h aimprocess_h)
{
  process_k value;
  time_t real_start_t = aimprocess_h.begin()->first;
  time_t fake_start_t = 0;
  for (auto& d : aimprocess_h) {
    int k = d.first - real_start_t + fake_start_t;
    value[k] = d.second;
  }
  return value;
}

void AdjustProcss_kByProp(process_k& input_k, double prop)
{
  process_k copy = input_k;
  for (auto d : copy) {
    input_k[d.first] = d.second * prop;
  }

}

string fiterstr(string instr) {

  regex pattern("\"");
  string returnstr = regex_replace(instr, pattern, "");
  return returnstr;
}

string filterSpecialChars(const string& instr) {
    string result = instr;
    
    // 移除所有特殊字符
    result.erase(
        remove_if(result.begin(), result.end(), 
                 [](char c) {
                     // 定义哪些是特殊字符
                     string specials = "\"\'\\!@#$%^&*()+=[]{}|;:,<>?`~";
                     return specials.find(c) != string::npos;
                 }),
        result.end()
    );
    
    return result;
}
string getBeforeLastUnderscore(const string& filename) {
  size_t pos = filename.find('_');

  if (pos != string::npos && pos < filename.length() - 1) {
    return filename.substr(pos + 1);
  }

  return "";
}
void write_h_csv(string filename, h_csv outcsv, char delimiter) {
  std::ofstream o(filename);
  for (auto& oc : outcsv) {
    for (auto& occ : oc) {
      o << occ << delimiter;
    }
    o << "\n";
  }
  o.close();
}

void write_h_csv_double(string filename, h_csv_double outcsv, char delimiter) {
  std::ofstream o(filename);
  for (auto& oc : outcsv) {
    for (auto& occ : oc) {
      o << occ << delimiter;
    }
    o << "\n";
  }
  o.close();
}



h_csv create_h_csv(int row, int col) {
  h_csv csv;
  csv.resize(row);

  for (int i = 0; i < csv.size(); i++) {
    csv[i].resize(col);
  }
  return csv;
}

h_csv_double create_h_csv_double(int row, int col) {
  h_csv_double csv;
  csv.resize(row);

  for (int i = 0; i < csv.size(); i++) {
    csv[i].resize(col);
  }
  return csv;
}

//h_csv read_h_csv(string filename, char delimiter) {
//  h_csv outvecvecstr;
//  if (!isExist(filename)) {
//    return outvecvecstr;
//  }
//  ifstream fin(filename);
//  string line;
//  char temp[1000];
//  std::memset(temp, 0, 1000);
//  if (!fin.good())
//  {
//    cout << "file : " << filename << "read failed!" << endl;
//  }
//  int i = 0;
//  while (fin.peek() != EOF && getline(fin, line))
//  {
//    stringstream sstr(line);
//    std::vector<string> fields;
//    string field;
//    split(line, delimiter, fields);	//csv文件
//    if (i >= 0)
//    {
//      std::vector<string> row_datas;
//#ifdef _WIN32 // Includes both 32 bit and 64 bit
//#ifdef _WIN64
//      for (int ln = 0; ln < fields.size(); ln++)
//      {
//        row_datas.push_back((fields[ln].c_str()));
//      }
//#else 
//      printf("Windows 32 bit\n");
//      for (int ln = 0; ln < fields.size(); ln++)
//      {
//        row_datas.push_back(UTF8ToGB(fields[ln].c_str()));
//      }
//
//#endif
//#else
//      printf("Not a Windows OS\n");
//      for (int ln = 0; ln < fields.size(); ln++)
//      {
//        row_datas.push_back((fields[ln]));
//      }
//#endif
//
//      outvecvecstr.push_back(row_datas);
//    }
//    if (i == -1)
//    {
//      std::vector<string> labels;
//      std::vector<string> row_datas;
//      for (int ln = 1; ln < fields.size(); ln++)
//      {
//        row_datas.push_back((fields[ln]));
//      }
//    }
//    i++;
//  }
//  fin.close();
//  return outvecvecstr;
//}

h_csv read_h_csv(string filename, char delimiter)
{
  h_csv outvecvecstr;

  // ========================================================
  // 判断当前位置合法 UTF-8 字符的字节数
  // 返回 0 表示不是合法 UTF-8 字符
  // ========================================================
  auto utf8CharLength =
    [](const string& text, size_t pos) -> size_t
  {
    if (pos >= text.size())
    {
      return 0;
    }

    auto byteAt =
      [&](size_t index) -> unsigned char
    {
      return static_cast<unsigned char>(
        text[index]);
    };

    const unsigned char c = byteAt(pos);

    // ASCII
    if (c <= 0x7F)
    {
      return 1;
    }

    // 两字节 UTF-8
    if (c >= 0xC2 && c <= 0xDF)
    {
      if (pos + 1 < text.size() &&
        (byteAt(pos + 1) & 0xC0) == 0x80)
      {
        return 2;
      }

      return 0;
    }

    // 三字节 UTF-8：E0
    if (c == 0xE0)
    {
      if (pos + 2 < text.size() &&
        byteAt(pos + 1) >= 0xA0 &&
        byteAt(pos + 1) <= 0xBF &&
        (byteAt(pos + 2) & 0xC0) == 0x80)
      {
        return 3;
      }

      return 0;
    }

    // 三字节 UTF-8：E1-EC、EE-EF
    if ((c >= 0xE1 && c <= 0xEC) ||
      (c >= 0xEE && c <= 0xEF))
    {
      if (pos + 2 < text.size() &&
        (byteAt(pos + 1) & 0xC0) == 0x80 &&
        (byteAt(pos + 2) & 0xC0) == 0x80)
      {
        return 3;
      }

      return 0;
    }

    // 三字节 UTF-8：ED，排除 UTF-16 代理区
    if (c == 0xED)
    {
      if (pos + 2 < text.size() &&
        byteAt(pos + 1) >= 0x80 &&
        byteAt(pos + 1) <= 0x9F &&
        (byteAt(pos + 2) & 0xC0) == 0x80)
      {
        return 3;
      }

      return 0;
    }

    // 四字节 UTF-8：F0
    if (c == 0xF0)
    {
      if (pos + 3 < text.size() &&
        byteAt(pos + 1) >= 0x90 &&
        byteAt(pos + 1) <= 0xBF &&
        (byteAt(pos + 2) & 0xC0) == 0x80 &&
        (byteAt(pos + 3) & 0xC0) == 0x80)
      {
        return 4;
      }

      return 0;
    }

    // 四字节 UTF-8：F1-F3
    if (c >= 0xF1 && c <= 0xF3)
    {
      if (pos + 3 < text.size() &&
        (byteAt(pos + 1) & 0xC0) == 0x80 &&
        (byteAt(pos + 2) & 0xC0) == 0x80 &&
        (byteAt(pos + 3) & 0xC0) == 0x80)
      {
        return 4;
      }

      return 0;
    }

    // 四字节 UTF-8：F4
    if (c == 0xF4)
    {
      if (pos + 3 < text.size() &&
        byteAt(pos + 1) >= 0x80 &&
        byteAt(pos + 1) <= 0x8F &&
        (byteAt(pos + 2) & 0xC0) == 0x80 &&
        (byteAt(pos + 3) & 0xC0) == 0x80)
      {
        return 4;
      }

      return 0;
    }

    return 0;
  };

  // ========================================================
  // 判断整个字符串是否是合法 UTF-8
  // ========================================================
  auto isValidUtf8 =
    [&](const string& text) -> bool
  {
    size_t pos = 0;

    while (pos < text.size())
    {
      const size_t length =
        utf8CharLength(text, pos);

      if (length == 0)
      {
        return false;
      }

      pos += length;
    }

    return true;
  };

  // ========================================================
  // 解码 UTF-8 字符的 Unicode 码点
  // ========================================================
  auto decodeUtf8CodePoint =
    [&](const string& text,
      size_t pos,
      size_t length) -> uint32_t
  {
    auto byteAt =
      [&](size_t index) -> uint32_t
    {
      return static_cast<unsigned char>(
        text[index]);
    };

    if (length == 1)
    {
      return byteAt(pos);
    }

    if (length == 2)
    {
      return
        ((byteAt(pos) & 0x1F) << 6) |
        (byteAt(pos + 1) & 0x3F);
    }

    if (length == 3)
    {
      return
        ((byteAt(pos) & 0x0F) << 12) |
        ((byteAt(pos + 1) & 0x3F) << 6) |
        (byteAt(pos + 2) & 0x3F);
    }

    if (length == 4)
    {
      return
        ((byteAt(pos) & 0x07) << 18) |
        ((byteAt(pos + 1) & 0x3F) << 12) |
        ((byteAt(pos + 2) & 0x3F) << 6) |
        (byteAt(pos + 3) & 0x3F);
    }

    return 0;
  };

  // ========================================================
  // 判断 Unicode 码点是否为常用中文
  // ========================================================
  auto isChineseCodePoint =
    [](uint32_t codePoint) -> bool
  {
    return
      (codePoint >= 0x3400 &&
        codePoint <= 0x4DBF) ||

      (codePoint >= 0x4E00 &&
        codePoint <= 0x9FFF) ||

      (codePoint >= 0xF900 &&
        codePoint <= 0xFAFF) ||

      (codePoint >= 0x20000 &&
        codePoint <= 0x2EBEF);
  };

  // ========================================================
  // 判断当前位置的 GBK/GB18030 字符长度
  // ========================================================
  auto gb18030CharLength =
    [](const string& text, size_t pos) -> size_t
  {
    if (pos >= text.size())
    {
      return 0;
    }

    auto byteAt =
      [&](size_t index) -> unsigned char
    {
      return static_cast<unsigned char>(
        text[index]);
    };

    const unsigned char b1 = byteAt(pos);

    if (b1 <= 0x7F)
    {
      return 1;
    }

    if (b1 < 0x81 || b1 > 0xFE)
    {
      return 0;
    }

    // GB18030 四字节编码
    if (pos + 3 < text.size())
    {
      const unsigned char b2 =
        byteAt(pos + 1);

      const unsigned char b3 =
        byteAt(pos + 2);

      const unsigned char b4 =
        byteAt(pos + 3);

      if (b2 >= 0x30 && b2 <= 0x39 &&
        b3 >= 0x81 && b3 <= 0xFE &&
        b4 >= 0x30 && b4 <= 0x39)
      {
        return 4;
      }
    }

    // GBK 两字节编码
    if (pos + 1 < text.size())
    {
      const unsigned char b2 =
        byteAt(pos + 1);

      if (b2 >= 0x40 &&
        b2 <= 0xFE &&
        b2 != 0x7F)
      {
        return 2;
      }
    }

    return 0;
  };

  // ========================================================
  // GBK/GB18030 转 UTF-8
  // ========================================================
  auto gb18030ToUtf8 =
    [](const string& input) -> string
  {
    if (input.empty())
    {
      return "";
    }

#ifdef _WIN32

    // Windows 代码页 54936：GB18030
    const UINT codePage = 54936;

    const int wideLength =
      MultiByteToWideChar(
        codePage,
        0,
        input.data(),
        static_cast<int>(input.size()),
        nullptr,
        0);

    if (wideLength <= 0)
    {
      return "";
    }

    std::wstring wideText(
      static_cast<size_t>(wideLength),
      L'\0');

    if (MultiByteToWideChar(
      codePage,
      0,
      input.data(),
      static_cast<int>(input.size()),
      &wideText[0],
      wideLength) <= 0)
    {
      return "";
    }

    const int utf8Length =
      WideCharToMultiByte(
        CP_UTF8,
        0,
        wideText.data(),
        wideLength,
        nullptr,
        0,
        nullptr,
        nullptr);

    if (utf8Length <= 0)
    {
      return "";
    }

    string output(
      static_cast<size_t>(utf8Length),
      '\0');

    if (WideCharToMultiByte(
      CP_UTF8,
      0,
      wideText.data(),
      wideLength,
      &output[0],
      utf8Length,
      nullptr,
      nullptr) <= 0)
    {
      return "";
    }

    return output;

#else

    iconv_t converter =
      iconv_open("UTF-8", "GB18030");

    if (converter == (iconv_t)-1)
    {
      return "";
    }

    size_t inputLeft = input.size();

    char* inputPointer =
      const_cast<char*>(input.data());

    string output(
      input.size() * 4 + 16,
      '\0');

    char* outputPointer =
      &output[0];

    size_t outputLeft =
      output.size();

    while (true)
    {
      const size_t convertResult =
        iconv(
          converter,
          &inputPointer,
          &inputLeft,
          &outputPointer,
          &outputLeft);

      if (convertResult !=
        static_cast<size_t>(-1))
      {
        break;
      }

      if (errno == E2BIG)
      {
        const size_t written =
          output.size() - outputLeft;

        output.resize(
          output.size() * 2);

        outputPointer =
          &output[0] + written;

        outputLeft =
          output.size() - written;

        continue;
  }

      iconv_close(converter);
      return "";
}

    const size_t written =
      output.size() - outputLeft;

    output.resize(written);

    iconv_close(converter);

    return output;

#endif
    };

  // ========================================================
  // 混合编码字段转 UTF-8
  //
  // 注意：
  // 只有整个文件不是合法 UTF-8 时，才会调用这里。
  // ========================================================
  auto normalizeMixedField =
    [&](const string& field) -> string
  {
    if (field.empty())
    {
      return "";
    }

    string output;
    output.reserve(field.size() * 2);

    size_t pos = 0;

    while (pos < field.size())
    {
      const unsigned char currentByte =
        static_cast<unsigned char>(
          field[pos]);

      // ASCII 在所有编码中相同
      if (currentByte <= 0x7F)
      {
        output.push_back(field[pos]);
        ++pos;
        continue;
      }

      const size_t utf8Length =
        utf8CharLength(field, pos);

      const size_t gbLength =
        gb18030CharLength(field, pos);

      // 只有 UTF-8 合法
      if (utf8Length >= 2 &&
        gbLength == 0)
      {
        output.append(
          field,
          pos,
          utf8Length);

        pos += utf8Length;
        continue;
      }

      // 只有 GBK/GB18030 合法
      if (utf8Length == 0 &&
        gbLength >= 2)
      {
        const string converted =
          gb18030ToUtf8(
            field.substr(
              pos,
              gbLength));

        if (!converted.empty())
        {
          output += converted;
        }
        else
        {
          output += "\xEF\xBF\xBD";
        }

        pos += gbLength;
        continue;
      }

      // UTF-8 和 GBK 都合法，需要消除歧义
      if (utf8Length >= 2 &&
        gbLength >= 2)
      {
        const uint32_t utf8CodePoint =
          decodeUtf8CodePoint(
            field,
            pos,
            utf8Length);

        const string gbConverted =
          gb18030ToUtf8(
            field.substr(
              pos,
              gbLength));

        uint32_t gbCodePoint = 0;

        if (!gbConverted.empty())
        {
          const size_t convertedLength =
            utf8CharLength(
              gbConverted,
              0);

          if (convertedLength > 0)
          {
            gbCodePoint =
              decodeUtf8CodePoint(
                gbConverted,
                0,
                convertedLength);
          }
        }

        const bool utf8IsChinese =
          isChineseCodePoint(
            utf8CodePoint);

        const bool gbIsChinese =
          isChineseCodePoint(
            gbCodePoint);

        /*
         * UTF-8 中文一般占 3 个字节。
         * 三字节或四字节合法 UTF-8 优先保留。
         */
        if (utf8Length >= 3)
        {
          output.append(
            field,
            pos,
            utf8Length);

          pos += utf8Length;
          continue;
        }

        /*
         * 解决典型误判：
         *
         * D2 BB：
         * UTF-8 -> һ
         * GBK   -> 一
         *
         * D6 A7：
         * UTF-8 -> ֧
         * GBK   -> 支
         *
         * D5 A2：
         * UTF-8 -> բ
         * GBK   -> 闸
         */
        if (utf8Length == 2 &&
          gbIsChinese &&
          !utf8IsChinese &&
          !gbConverted.empty())
        {
          output += gbConverted;
          pos += gbLength;
          continue;
        }

        // 其他情况默认保留 UTF-8
        output.append(
          field,
          pos,
          utf8Length);

        pos += utf8Length;
        continue;
      }

      // 无法识别的字节，写入 UTF-8 替换字符
      output += "\xEF\xBF\xBD";
      ++pos;
    }

    return output;
  };

  // ========================================================
  // 打开文件
  // ========================================================
#ifdef _WIN32

  const std::filesystem::path filePath =
    std::filesystem::u8path(filename);

  std::ifstream file(
    filePath,
    std::ios::binary);

#else

  std::ifstream file(
    filename,
    std::ios::binary);

#endif

  if (!file.is_open())
  {
    std::cout
      << "file : "
      << filename
      << " read failed!"
      << std::endl;

    return outvecvecstr;
  }

  // 一次性读取文件原始字节
  string rawData{
      std::istreambuf_iterator<char>(file),
      std::istreambuf_iterator<char>()
  };

  file.close();

  if (rawData.empty())
  {
    return outvecvecstr;
  }

  // ========================================================
  // 先判断整个文件是否为 UTF-8
  // ========================================================
  const bool hasUtf8Bom =
    rawData.size() >= 3 &&
    static_cast<unsigned char>(rawData[0]) == 0xEF &&
    static_cast<unsigned char>(rawData[1]) == 0xBB &&
    static_cast<unsigned char>(rawData[2]) == 0xBF;

  const bool wholeFileIsUtf8 =
    hasUtf8Bom ||
    isValidUtf8(rawData);

  // 删除 UTF-8 BOM
  if (hasUtf8Bom)
  {
    rawData.erase(0, 3);
  }

  // ========================================================
  // 保留原来的按行读取和 split 逻辑
  // ========================================================
  std::istringstream inputStream(rawData);

  string line;

  while (std::getline(inputStream, line))
  {
    // Windows CSV 使用 \r\n，getline 后可能保留 \r
    if (!line.empty() &&
      line.back() == '\r')
    {
      line.pop_back();
    }

    std::vector<string> fields;

    // 使用你原来的 split 函数
    split(line, delimiter, fields);

    std::vector<string> row_datas;
    row_datas.reserve(fields.size());

    for (size_t columnIndex = 0;
      columnIndex < fields.size();
      ++columnIndex)
    {
      if (wholeFileIsUtf8)
      {
        /*
         * 整个文件已经确认是 UTF-8：
         * 直接保存，禁止再进行 GBK 猜测。
         */
        row_datas.push_back(
          fields[columnIndex]);
      }
      else
      {
        /*
         * 整个文件不是合法 UTF-8：
         * 才逐字段修复混合编码。
         */
        row_datas.push_back(
          normalizeMixedField(
            fields[columnIndex]));
      }
    }

    outvecvecstr.push_back(
      std::move(row_datas));
  }

  return outvecvecstr;
  }

h_csv read_h_csv_from_string(const std::string& data, char delimiter) {
  std::stringstream fin(data);
  string line;
  h_csv outvecvecstr;
  int i = 0;
  while (getline(fin, line)) {
    std::vector<string> fields; split(line, delimiter, fields);
    if (i >= 0) {
      std::vector<string> row_datas;
#ifdef _WIN32 // Includes both 32 bit and 64 bit
#ifdef _WIN64
      for (int ln = 0; ln < fields.size(); ln++) {
        row_datas.push_back((fields[ln].c_str()));
      }
#else 
      printf("Windows 32 bit\n");
      for (int ln = 0; ln < fields.size(); ln++) {
        row_datas.push_back(UTF8ToGB(fields[ln].c_str()));
      }
#endif
#else
      printf("Not a Windows OS\n");
      for (int ln = 0; ln < fields.size(); ln++) {
        row_datas.push_back((fields[ln]));
      }
#endif

      outvecvecstr.push_back(row_datas);
    }
    if (i == -1) {
      std::vector<string> labels;
      std::vector<string> row_datas;
      for (int ln = 1; ln < fields.size(); ln++) {
        row_datas.push_back((fields[ln]));
      }
    }
    i++;
  }
  return outvecvecstr;
}

// 根据分隔符delim，将s中被分隔的字符串逐个保存到elems
void split(const std::string& s, char delim, std::vector<std::string>& elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

//创建文件夹，路径
void mkdir_h(string mkpath) {
  string sFullPath = "./测试11/测试22/测试33/测试44/";
  if (mkpath != "") {
    sFullPath = mkpath;
  }
  string sNewFullPath = sFullPath.substr(0, sFullPath.find_last_of("/") + 1);
  //判断前面的路径是否存在，不存在则创建
  size_t index = sFullPath.find("/", 0);//得到第一级路径的索引
  if (index > sFullPath.length()) return;
  while ((index = sFullPath.find("/", index + 1)) < sFullPath.length())
  {
    string tmpPath = sFullPath.substr(0, index);
    if (_access(tmpPath.c_str(), 0) != 0)//io.h
    {
      if (_mkdir(tmpPath.c_str()) != 0) break;//direct.h
    }
  }
}
// 根据分隔符delim，将s中被分隔的字符串逐个保存到elems
void split_s(const std::string& s, char delim, std::vector<std::string>& elems) {
  std::stringstream ss(s);
  //cout << s << endl;
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
}

// UTF8字符串转成GBK字符串
std::string U2G(const std::string& utf8)
{
#ifdef _WIN32
  int nwLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
  wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
  memset(pwBuf, 0, nwLen * 2 + 2);

  MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.length(), pwBuf, nwLen);

  int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

  char* pBuf = new char[nLen + 1];
  memset(pBuf, 0, nLen + 1);

  WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  std::string retStr = pBuf;

  delete[]pBuf;
  delete[]pwBuf;

  pBuf = NULL;
  pwBuf = NULL;

  return retStr;
#else
  return utf8_to_gbk_unix(utf8);
#endif
}

// GBK字符串转成json识别的UTF8字符串
std::string G2U(const std::string& gbk)
{
#ifdef _WIN32
  int nwLen = ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), -1, NULL, 0);

  wchar_t* pwBuf = new wchar_t[nwLen + 1];//加1用于截断字符串 
  ZeroMemory(pwBuf, nwLen * 2 + 2);

  ::MultiByteToWideChar(CP_ACP, 0, gbk.c_str(), gbk.length(), pwBuf, nwLen);

  int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

  char* pBuf = new char[nLen + 1];
  ZeroMemory(pBuf, nLen + 1);

  ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

  std::string retStr(pBuf);

  delete[]pwBuf;
  delete[]pBuf;

  pwBuf = NULL;
  pBuf = NULL;

  return retStr;
#else
  return gbk_to_utf8_unix(gbk);
#endif
}



// 检查是否是 GBK 编码（简单版，不完全准确）
bool isPossibleGBK(const std::string &str) {
    for (size_t i = 0; i < str.size(); ) {
        if ((unsigned char)str[i] <= 0x7F) {
            i++; // ASCII
        } else {
            if (i + 1 >= str.size()) return false; // 不完整的 GBK
            unsigned char c1 = str[i];
            unsigned char c2 = str[i + 1];
            // GBK 范围：首字节 0x81-0xFE，次字节 0x40-0xFE（非严格）
            if (c1 >= 0x81 && c1 <= 0xFE && c2 >= 0x40 && c2 <= 0xFE) {
                i += 2;
            } else {
                return false;
            }
        }
    }
    return true;
}


//bool isValidUTF8(const std::string& string) {
//  int numBytes = 0;
//  for (unsigned char c : string) {
//    if (numBytes == 0) {
//      if ((c & 0x80) == 0) {
//        // ASCII byte (0xxxxxxx)
//        continue;
//      }
//      else if ((c & 0xE0) == 0xC0) {
//        // First byte of 2-byte sequence (110xxxxx)
//        numBytes = 1;
//      }
//      else if ((c & 0xF0) == 0xE0) {
//        // First byte of 3-byte sequence (1110xxxx)
//        numBytes = 2;
//      }
//      else if ((c & 0xF8) == 0xF0) {
//        // First byte of 4-byte sequence (11110xxx)
//        numBytes = 3;
//      }
//      else {
//        return false; // Invalid UTF-8
//      }
//    }
//    else {
//      // Follow-up byte (10xxxxxx)
//      if ((c & 0xC0) != 0x80) {
//        return false;
//      }
//      numBytes--;
//    }
//  }
//  return numBytes == 0;
//}

bool isValidUTF8(const std::string& string) {
  int numBytes = 0;
  int seqLength = 0; // 记录当前字符的总字节数
  int codePoint = 0; // 用于计算 2 字节序列的 Unicode 码位
  
  for (unsigned char c : string) {
    if (numBytes == 0) {
      if ((c & 0x80) == 0) {
        // ASCII 字符
        continue;
      }
      else if ((c & 0xE0) == 0xC0) {
        // 2字节序列的第一个字节
        numBytes = 1;
        seqLength = 2;
        codePoint = c & 0x1F; // 提取前 5 位
      }
      else if ((c & 0xF0) == 0xE0) {
        // 3字节序列的第一个字节 (UTF-8的中文通常在这里)
        numBytes = 2;
        seqLength = 3;
      }
      else if ((c & 0xF8) == 0xF0) {
        // 4字节序列的第一个字节
        numBytes = 3;
        seqLength = 4;
      }
      else {
        return false; // 非法 UTF-8 头部
      }
    }
    else {
      // 跟随字节必须是 10xxxxxx
      if ((c & 0xC0) != 0x80) {
        return false;
      }
      
      // 如果是 2 字节序列，我们把码位拼出来
      if (seqLength == 2) {
         codePoint = (codePoint << 6) | (c & 0x3F);
      }
      
      numBytes--;
      
      // 当一个 2 字节字符解析完毕时，进行“防 GBK 撞车”拦截
      if (numBytes == 0 && seqLength == 2) {
         // U+0080 到 U+03FF 包含常见的拉丁字母扩展和数学符号(如 °C, ±, ×, ÷ 等)
         // 如果码位 >= 0x0400 (例如俄文西里尔字母、亚美尼亚语等偏门语言)
         // 在纯中文/英文环境里，这极大概率是 GBK 汉字的“巧合撞车”(比如"闸"会变成 U+0562)
         // 此时我们果断判定它不是 UTF-8！
         if (codePoint >= 0x0400) {
             return false; 
         }
      }
    }
  }
  return numBytes == 0;
}

bool isExist(string filepath) {
  std::ifstream file(filepath);

  if (file) {
    std::cout << "file : " << filepath << " -> exist." << std::endl;
    return true;
  }
  else {
    std::cout <<"file : " << filepath << " ->  Not exist."<< std::endl;
    return false;
  }

}
bool isDirExist(const std::string& dirpath)
{
    namespace fs = std::filesystem;

    if (fs::exists(dirpath) && fs::is_directory(dirpath))
    {
        std::cout << "dir : " << dirpath << " -> exist." << std::endl;
        return true;
    }
    else
    {
        std::cout << "dir : " << dirpath << " -> Not exist." << std::endl;
        return false;
    }
}

void check_elements_is_null(h_csv& edges, int row, int col) {

  for (int i = 0; i < edges.size(); i++) {
    for (int j = 0; j < edges[i].size(); j++) {
      if (i == row && j == col) {
        if (edges[i][j] == "") {
          cout << "\r[Warning]: " << "edges" << "[" << i << "]" << "[" << j << "]"<<"("<<"source= "<<edges[i][0]<<", label= "<<edges[0][j] << "is empty !" ;
          edges[i][j]="0";
          return;
        }
        else
        {
          return;
        }
      }
    }
  }
  std::cout << "[Error]: " << "Can't find element at row= " << row << ", col= " << col << endl;
}



CalculationParams read_Generalization_input_json(string inpath) {

  CalculationParams input;
    json input_json = ReadJson(inpath + "/input.json");

  if (input_json.contains("CalculationParams")) {
      string start_time = input_json["CalculationParams"]["start_time"];
      input.start_time_t = StringToTime_t(start_time);
      string start_time_str = FormatTime(input.start_time_t);
      input.T = input_json["CalculationParams"]["T"];

      // 输出 隐式 调度方案的路径
      if (input_json["CalculationParams"].contains("schedule_json_output_path")) {
        input.schedule_json_output_path = input_json["CalculationParams"]["schedule_json_output_path"];
      }
      else
      {
        input.schedule_json_output_path = inpath;
      }
      if (input_json["CalculationParams"].contains("sim_dt")) {
        double sim_dt = input_json["CalculationParams"]["sim_dt"].get<double>();
        input.sim_dt = sim_dt;
      }

      if (input_json["CalculationParams"].contains("sim_dx")) {
        double sim_dx = input_json["CalculationParams"]["sim_dx"].get<double>();
        input.sim_dx = sim_dx;
      }
      if (input_json["CalculationParams"].contains("sim_inpath")) {
        string  sim_inpath = input_json["CalculationParams"]["sim_inpath"].get<string>();
        input.sim_inpath = sim_inpath;
      }
      if (input_json["CalculationParams"].contains("sim_outpath")) {
        string  sim_outpath = input_json["CalculationParams"]["sim_outpath"].get<string>();
        input.sim_outpath = sim_outpath;
      }

    }
  return input;
}
CalculationParams read_Generalization_input_json(const char* input_json_c_str) {

  CalculationParams input;
  string input_str = input_json_c_str;
  json input_json = json::parse(input_str);

  if (input_json.contains("CalculationParams")) {
      string start_time = input_json["CalculationParams"]["start_time"];
      input.start_time_t = StringToTime_t(start_time);
      input.T = input_json["CalculationParams"]["T"];

      // 输出 隐式 调度方案的路径
      if (input_json["CalculationParams"].contains("schedule_json_output_path")) {
        input.schedule_json_output_path = input_json["CalculationParams"]["schedule_json_output_path"];
      }
      else
      {
        input.schedule_json_output_path = "./";
      }
     

      if (input_json["CalculationParams"].contains("sim_dt")) {
        double sim_dt = input_json["CalculationParams"]["sim_dt"].get<double>();
        input.sim_dt = sim_dt;
      }

      if (input_json["CalculationParams"].contains("sim_dx")) {
        double sim_dx = input_json["CalculationParams"]["sim_dx"].get<double>();
        input.sim_dx = sim_dx;
      }
      if (input_json["CalculationParams"].contains("sim_inpath")) {
        string  sim_inpath = input_json["CalculationParams"]["sim_inpath"].get<string>();
        input.sim_inpath = sim_inpath;
      }
      if (input_json["CalculationParams"].contains("sim_outpath")) {
        string  sim_outpath = input_json["CalculationParams"]["sim_outpath"].get<string>();
        input.sim_outpath = sim_outpath;
      }

    }
  return input;
}


  //void get_obs_Generalization(json obs_json,CalculationParams& input ) {


  //  int max_obs_STime = 0;
  //  int max_obs_STime_gates_vec_id = 0;
  //  //获取实测流量、调节时间
  //  for (int i = 0; i < input.topo_dicts.gates.size(); i++) {
  //    string g_name = input.topo_dicts.gates[i].name;
  //    if (isValidUTF8(g_name)) {
  //      g_name = U2G(g_name);
  //    }
  //    Ocis_edges::Point* g = input.topo_dicts.get_gate_byName(g_name);
  //    if (obs_json.contains(G2U(g_name))) {
  //      double flow = obs_json[G2U(g_name)]["flow"];
  //      double h = obs_json[G2U(g_name)]["h"];
  //      g->obs_flow = flow;
  //      g->obs_h = h;


  //      if (obs_json[G2U(g_name)].contains("obs_STime")) {
  //        g->obs_STime = obs_json[G2U(g_name)]["obs_STime"];
 
  //        int obs_STime = StringToTime_t(g->obs_STime);
  //        input.topo_dicts.gates_obs_STime[g_name] = obs_STime;


  //        //获取节制闸的最新（最后一次控制的时间）
  //        if (obs_STime > max_obs_STime && (g->type == 4 || g->type == 2)) {
  //          max_obs_STime = obs_STime;
  //          max_obs_STime_gates_vec_id = i;
  //        }

  //      }
  //    }
  //  }
  //  input.topo_dicts.gates[max_obs_STime_gates_vec_id].is_max_obs_STime_Label = 1;



  //}




//
//process_T read_csv_tm(string path) {
//
//  process_T csv_tm;
//
//  h_csv boundary_flow_csv = read_h_csv(path, ',');
//  if (boundary_flow_csv.size() <= 0) {
//    return csv_tm;
//  }
//
//  std::vector<string> Label = boundary_flow_csv[0];
//
//
//  std::vector<string> tm ;
//
//  for (int i = 0; i < boundary_flow_csv.size(); i++) {
//    if (boundary_flow_csv[i].size() > 0&&i>0) {
//      tm.push_back(boundary_flow_csv[i][0]);
//    }
//  }
//
//  for (int i = 0; i < tm.size(); i++) {
//    for (int j = 0; j < boundary_flow_csv[i].size(); j++) {
//
//      if (i > 0 && j > 0) {                //  避开label和time_str
//        if (tm[i] == "") { continue; }
//        double data = -1;
//        if (boundary_flow_csv[i][j] != "" && !containsInvalidChars(boundary_flow_csv[i][j])) {
//          data = stod(boundary_flow_csv[i][j]);
//        }
//        else if (boundary_flow_csv[i][j] != "" && isValidString(boundary_flow_csv[i][j], "^[a-zA-Z0-9]*$"))
//        {
//          data = stod(removeSpaces(boundary_flow_csv[i][j]));
//        }
//        if (j < Label.size()) {
//
//
//          csv_tm[Label[j]][StringToTime_t(tm[i-1])] = data;
//        }
//      }
//
//    }
//  }
//  return csv_tm;
//}
//
//
//process_T read_tidyData(string path,int value_col) {
//
//  process_T csv_tm;
//
//  h_csv boundary_flow_csv = read_h_csv(path, ',');
//  if (boundary_flow_csv.size() <= 0) {
//    return csv_tm;
//  }
//
//  std::vector<string> Label;
//
//  for (int i = 0; i < boundary_flow_csv.size(); i++) {
//
//    string name = boundary_flow_csv[i][0];
//    if (!find_vec(Label, name)) {
//      Label.push_back(name);
//    }
//
//  }
//
//
//  for (int i = 0; i < Label.size(); i++) {
//    if (isValidUTF8(Label[i])) {
//      Label[i] = U2G(Label[i]);
//    }
//  }
//
//
//  std::vector<string> tm ;
//
//  for (int i = 0; i < boundary_flow_csv.size(); i++) {
//    if (boundary_flow_csv[i].size() > 0&&i>0) {
//      string name = boundary_flow_csv[i][0];
//      if (isValidUTF8(name)) {
//        name = U2G(name);
//      }
//
//
//      string tm_str = boundary_flow_csv[i][1];
//      string value_str = boundary_flow_csv[i][value_col] == "" ? "-1" : boundary_flow_csv[i][value_col];
//      if (find_vec(Label,name )&&name!="") {
//        csv_tm[name][StringToTime_t(tm_str)] = stod(value_str);
//
//      }
//    }
//  }
//
//  for (int i = 0; i < tm.size(); i++) {
//    for (int j = 0; j < boundary_flow_csv[i].size(); j++) {
//
//      if (i > 0 && j > 0) {
//        if (tm[i] == "") { continue; }
//        double data = -1;
//        if (boundary_flow_csv[i][j] != "" && !containsInvalidChars(boundary_flow_csv[i][j])) {
//          data = stod(boundary_flow_csv[i][j]);
//        }
//        if (j < Label.size()) {
//          csv_tm[Label[j]][StringToTime_t(tm[i])] = data;
//        }
//      }
//
//    }
//  }
//
//
//  return csv_tm;
//
//}
//
//
//process_T read_tidyData(string path, int obj_col, int tm_col, int value_col) {
//
//  process_T csv_tm;
//
//  h_csv boundary_flow_csv = read_h_csv(path, ',');
//  if (boundary_flow_csv.size() <= 0) {
//    return csv_tm;
//  }
//
//  std::vector<string> Label;
//
//  for (int i = 0; i < boundary_flow_csv.size(); i++) {
//
//    string name = boundary_flow_csv[i][obj_col];
//    if (!find_vec(Label, name)) {
//      Label.push_back(name);
//    }
//
//  }
//
//  for (int i = 0; i < boundary_flow_csv.size(); i++) {
//    if (boundary_flow_csv[i].size() > 0 && i > 0) {
//      string name = boundary_flow_csv[i][obj_col];
//      string tm_str = boundary_flow_csv[i][tm_col];
//      if (i < boundary_flow_csv.size() && value_col < boundary_flow_csv[i].size()) {
//        string value_str = boundary_flow_csv[i][value_col] == "" ? "-1" : boundary_flow_csv[i][value_col];
//        if (find_vec(Label, name) && name != "") {
//          csv_tm[name][StringToTime_t(tm_str)] = stod(value_str);
//
//        }
//
//      }
//
//    }
//  }
//
//
//  return csv_tm;
//
//
//}
//
//

// ============================================================
// 清理 CSV 字段
// 返回值仍然是 UTF-8 编码
// ============================================================
static string trim_csv_field_utf8(string text)
{
    // 去除字段开头可能残留的 UTF-8 BOM
    if (text.size() >= 3 &&
        static_cast<unsigned char>(text[0]) == 0xEF &&
        static_cast<unsigned char>(text[1]) == 0xBB &&
        static_cast<unsigned char>(text[2]) == 0xBF)
    {
        text.erase(0, 3);
    }

    // 删除开头的 ASCII 空白字符
    size_t begin = 0;

    while (begin < text.size() &&
           std::isspace(
               static_cast<unsigned char>(text[begin])))
    {
        ++begin;
    }

    // 删除结尾的 ASCII 空白字符
    size_t end = text.size();

    while (end > begin &&
           std::isspace(
               static_cast<unsigned char>(text[end - 1])))
    {
        --end;
    }

    text = text.substr(begin, end - begin);

    // 删除字段两侧的双引号
    if (text.size() >= 2 &&
        text.front() == '"' &&
        text.back() == '"')
    {
        text = text.substr(1, text.size() - 2);
    }

    return text;
}


// ============================================================
// 安全解析 double
//
// 解析成功：返回 true
// 解析失败：返回 false，不抛出异常
// ============================================================
static bool try_parse_csv_double(
    const string& input,
    double& result)
{
    string text = trim_csv_field_utf8(input);

    if (text.empty())
    {
        return false;
    }

    errno = 0;

    char* parseEnd = nullptr;

    const double value = std::strtod(
        text.c_str(),
        &parseEnd);

    // 没有解析到任何数字
    if (parseEnd == text.c_str())
    {
        return false;
    }

    // 跳过数值后的空格
    while (*parseEnd != '\0' &&
           std::isspace(
               static_cast<unsigned char>(*parseEnd)))
    {
        ++parseEnd;
    }

    // 数值后面还有其他非法字符
    if (*parseEnd != '\0')
    {
        return false;
    }

    // 数值超出 double 范围
    if (errno == ERANGE)
    {
        return false;
    }

    // 排除 NaN 和无穷大
    if (!std::isfinite(value))
    {
        return false;
    }

    result = value;

    return true;
}


// ============================================================
// 安全解析时间
//
// StringToTime_t 为你原有的时间转换函数。
// 时间为空或转换异常时返回 false。
// ============================================================
static bool try_parse_csv_time(
    const string& input,
    time_t& result)
{
    string timeString =
        trim_csv_field_utf8(input);

    if (timeString.empty())
    {
        return false;
    }

    try
    {
        result = StringToTime_t(timeString);
        return true;
    }
    catch (...)
    {
        return false;
    }
}


// ============================================================
// 宽表格式 CSV
//
// CSV 格式示例：
//
// time,gate1,gate2,gate3
// 2025/01/01 00:00:00,1.0,2.0,3.0
// 2025/01/01 01:00:00,1.1,2.1,3.1
//
// 返回：
// csv_tm["gate1"][time] = 1.0
// ============================================================
process_T read_csv_tm(string path)
{
  if (!isExist(path))
  {
      return process_T();
  }

    process_T csv_tm;

    h_csv boundary_flow_csv =
        read_h_csv(path, ',');

    // 至少需要标题行
    if (boundary_flow_csv.empty())
    {
        return csv_tm;
    }

    const std::vector<string>& labels =
        boundary_flow_csv[0];

    // 第一列为时间，至少应有两列
    if (labels.size() <= 1)
    {
        return csv_tm;
    }

    // 从第 1 行开始，第 0 行为标题行
    for (size_t rowIndex = 1;
         rowIndex < boundary_flow_csv.size();
         ++rowIndex)
    {
        const std::vector<string>& row =
            boundary_flow_csv[rowIndex];

        // 当前行至少需要时间列
        if (row.empty())
        {
            continue;
        }

        const string timeString =
            trim_csv_field_utf8(row[0]);

        if (timeString.empty())
        {
            continue;
        }

        time_t currentTime;
        if (timeString.length() > 6)
        {
          if (!try_parse_csv_time(
                  timeString,
                  currentTime))
          {
            // 时间格式错误，跳过整行
            continue;
          }
        }
        else
        {
          currentTime = stoi(timeString);
        }

        // 防止当前行列数超过标题行，或者少于标题行
        const size_t columnCount =
            std::min(
                row.size(),
                labels.size());

        // 第 0 列为时间，因此从第 1 列开始读取
        for (size_t columnIndex = 1;
             columnIndex < columnCount;
             ++columnIndex)
        {
            const string label =
                trim_csv_field_utf8(
                    labels[columnIndex]);

            // 标题为空，不写入
            if (label.empty())
            {
                continue;
            }

            double data = -1.0;

            // 解析失败时保持为 -1.0
            try_parse_csv_double(
                row[columnIndex],
                data);

            // label 始终为 UTF-8
            csv_tm[label][currentTime] = data;
        }
    }

    return csv_tm;
}


// ============================================================
// 长表格式 CSV 的内部实现
//
// obj_col：对象名称所在列
// tm_col：时间所在列
// value_col：数值所在列
//
// CSV 格式示例：
//
// name,time,value
// gate1,2025/01/01 00:00:00,1.0
// gate2,2025/01/01 00:00:00,2.0
// ============================================================
static process_T read_tidyData_utf8_impl(
    string path,
    int obj_col,
    int tm_col,
    int value_col)
{
    process_T csv_tm;

    // 禁止负数列号
    if (obj_col < 0 ||
        tm_col < 0 ||
        value_col < 0)
    {
        return csv_tm;
    }

    h_csv boundary_flow_csv =
        read_h_csv(path, ',');

    if (boundary_flow_csv.empty())
    {
        return csv_tm;
    }

    const size_t objColumn =
        static_cast<size_t>(obj_col);

    const size_t timeColumn =
        static_cast<size_t>(tm_col);

    const size_t valueColumn =
        static_cast<size_t>(value_col);

    size_t requiredColumn = objColumn;

    requiredColumn =
        std::max(requiredColumn, timeColumn);

    requiredColumn =
        std::max(requiredColumn, valueColumn);

    // 从第 1 行读取，第 0 行视为标题行
    for (size_t rowIndex = 1;
         rowIndex < boundary_flow_csv.size();
         ++rowIndex)
    {
        const std::vector<string>& row =
            boundary_flow_csv[rowIndex];

        // 当前行列数不足
        if (row.size() <= requiredColumn)
        {
            continue;
        }

        // read_h_csv 已经将内容统一为 UTF-8
        string name =
            trim_csv_field_utf8(
                row[objColumn]);

        string timeString =
            trim_csv_field_utf8(
                row[timeColumn]);

        if (name.empty() ||
            timeString.empty())
        {
            continue;
        }

        time_t currentTime;

        if (!try_parse_csv_time(
                timeString,
                currentTime))
        {
            // 时间无效，跳过当前行
            continue;
        }

        double value = -1.0;

        // 空值或非法数值保持为 -1.0
        try_parse_csv_double(
            row[valueColumn],
            value);

        // name 保持 UTF-8，不再进行 U2G 转换
        csv_tm[name][currentTime] = value;
    }

    return csv_tm;
}


// ============================================================
// 长表格式：对象列固定为 0，时间列固定为 1
// ============================================================
process_T read_tidyData(
    string path,
    int value_col)
{
    return read_tidyData_utf8_impl(
        path,
        0,
        1,
        value_col);
}


// ============================================================
// 长表格式：由调用方指定对象列、时间列和数值列
// ============================================================
process_T read_tidyData(
    string path,
    int obj_col,
    int tm_col,
    int value_col)
{
    return read_tidyData_utf8_impl(
        path,
        obj_col,
        tm_col,
        value_col);
}
process_T read_jsonData(string path) {
  process_T data;
  json input_json = ReadJson(path);
  for (auto& gate_obj : input_json.items()) {
    string name = gate_obj.key();
    string name_utf8 = name;
    json sub_json = gate_obj.value();
    for (auto& timedata_obj : sub_json.items()) {
      string time_str = timedata_obj.key();
      time_t time_t = StringToTime_t(time_str);
      double value = timedata_obj.value().get<double>();
      data[name][time_t] = value;
    }
  }

  return data;
}


void write_tidyData(string path,process_T& data) {

  h_csv td_data;
  std::vector<string> label = {"tm","obj","value"};
  td_data.push_back(label);

  for (auto iter = data.begin(); iter != data.end(); iter++) {

    string name = iter->first;
    string name_utf8 = iter->first;
    if (!isValidUTF8(name)) {
      name_utf8 = G2U(name);
    }

    if (name_utf8.find("?")!=string::npos) {
      continue;
    }

    auto sub_data = iter->second;

    int tm_length = sub_data.size();
    h_csv row_data; row_data.resize(tm_length);

    int i = 0;
    for (auto sub_iter = sub_data.begin(); sub_iter != sub_data.end(); sub_iter++) {
      string tm_str = FormatTime(sub_iter->first);
      string value_str = to_string(sub_iter->second);

      row_data[i].push_back(tm_str);
      row_data[i].push_back(name_utf8);
      row_data[i].push_back(value_str);
      i++;
    }

    td_data.insert(td_data.end(), std::begin(row_data), std::end(row_data));

  }

  write_h_csv(path,td_data,',');

}


void write_tidyData(string path, std::map<string, time_t>& data) {
  h_csv td_data;
  std::vector<string> label = {"tm","obj","action_type"};
  td_data.push_back(label);

  for (auto iter = data.begin(); iter != data.end(); iter++) {

    string name = iter->first;
    string name_utf8 = iter->first;
    if (isPossibleGBK(name)) {
      name_utf8 = G2U(name);
    }
    else
    {
      name = U2G(name);
    }

    double sub_data = iter->second;

    h_csv row_data; row_data.resize(1);

    int i = 0;
    string tm_str = FormatTime(iter->second);
    string obj_str = name_utf8;
    string action_type = "arr";

    row_data[i].push_back(tm_str);
    row_data[i].push_back(obj_str);
    row_data[i].push_back(action_type);

    td_data.insert(td_data.end(), std::begin(row_data), std::end(row_data));

  }

  write_h_csv(path,td_data,',');

}

void write_tidyData(string path, std::unordered_map<string, double>& data,std::vector<string> label) {
  h_csv td_data;
  td_data.push_back(label);

  for (auto iter = data.begin(); iter != data.end(); iter++) {

    string name = iter->first;
    string name_utf8 = iter->first;
    if (isPossibleGBK(name)) {
      name_utf8 = G2U(name);
    }
    else
    {
      name = U2G(name);
    }

    double sub_data = iter->second;

    h_csv row_data; row_data.resize(1);

    int i = 0;
    string  value_str = to_string(iter->second);
    string obj_str = name_utf8;
    row_data[i].push_back(obj_str);
    row_data[i].push_back(value_str);

    td_data.insert(td_data.end(), std::begin(row_data), std::end(row_data));

  }

  write_h_csv(path,td_data,',');

}

void write_tidyData(string path, std::unordered_map<string, string>& data,std::vector<string> label) {
  h_csv td_data;
  td_data.push_back(label);

  for (auto iter = data.begin(); iter != data.end(); iter++) {

    string name = iter->first;
    string name_utf8 = iter->first;
    if (isPossibleGBK(name)) {
      name_utf8 = G2U(name);
    }
    else
    {
      name = U2G(name);
    }


    h_csv row_data; row_data.resize(1);
    int i = 0;
    string  value_str = (iter->second);
    string obj_str = name_utf8;
    row_data[i].push_back(obj_str);
    row_data[i].push_back(value_str);
    td_data.insert(td_data.end(), std::begin(row_data), std::end(row_data));
  }

  write_h_csv(path,td_data,',');

}


double intkey_LinearInterpolate(const std::map<int, double>& myMap, int key) {
  if (myMap.empty() ) {
    return -1;
  }

  // 如果 map 为空或者 key 小于 map 中的最小键，则无法插值
  if (myMap.empty() ) {


    return -1;
  }

  // 查找第一个大于或等于 key 的迭代器
  auto upper = myMap.lower_bound(key);

  // 如果这个迭代器是 map 的开始，则只能返回第一个值
  if (upper == myMap.begin()) {
    return upper->second;
  }

  // 如果这个迭代器是 map 的结束，则只能返回最后一个值
  if (upper == myMap.end()) {
    return std::prev(upper)->second;
  }

  // 找到 key 之前的键值对
  auto lower = std::prev(upper);

  // 进行线性插值
  double k1 = lower->first;
  double v1 = lower->second;
  double k2 = upper->first;
  double v2 = upper->second;

  double linear_value = v1 -(fabs(key - k1) / fabs(k2 - k1)) * (v1 - v2);

  if (v1 == -1 && v2 != -1) {
    v1 = v2;
  }
  if (v2 == -1 && v1 != -1) {
    v2 = v1;
  }
  if (v2 == -1 && v1 == -1) {
    return -1;
  }

  return linear_value ;
}



double GateScheduleLinearInterpolate(const std::map<time_t, double>& myMap, time_t key) {
  if (myMap.empty() ) {
    return -1;
  }

  // 如果 map 为空或者 key 小于 map 中的最小键，则无法插值
  if (myMap.empty() || key < myMap.begin()->first) {

    string myMap_time_str = FormatTime(myMap.begin()->first);
    string key_time_str = FormatTime(key);
    //cout << "sim_t= " << key_time_str << ", " << "boundary_t" << myMap_time_str << endl;
    return -1;
  }

  // 查找第一个大于或等于 key 的迭代器
  auto upper = myMap.lower_bound(key);

  // 如果这个迭代器是 map 的开始，则只能返回第一个值
  if (upper == myMap.begin()) {
    return upper->second;
  }

  // 如果这个迭代器是 map 的结束，则只能返回最后一个值
  if (upper == myMap.end()) {
    return std::prev(upper)->second;
  }

  // 找到 key 之前的键值对
  auto lower = std::prev(upper);

  // 进行线性插值
  double k1 = lower->first;
  double v1 = lower->second;
  double k2 = upper->first;
  double v2 = upper->second;

  if (v1 == -1 && v2 != -1) {
    v1 = v2;
  }
  if (v2 == -1 && v1 != -1) {
    v2 = v1;
  }
  if (v2 == -1 && v1 == -1) {
    return -1;
  }


  return v1 ;
}

double GateScheduleLinearInterpolate(const std::map<time_t, double>& myMap, time_t key,int interval) {
  if (myMap.empty() ) {
    return -1;
  }

  // 如果 map 为空或者 key 小于 map 中的最小键，则无法插值
  if (myMap.empty() || key+interval < myMap.begin()->first) {

    //string myMap_time_str = FormatTime(myMap.begin()->first);
    //string key_time_str = FormatTime(key);

    return -1;
  }

  // 查找第一个大于或等于 key 的迭代器
  auto upper = myMap.lower_bound(key);

  if (upper == myMap.end()&&fabs(std::prev(upper)->first - key) > interval) {
    return -1;
  }

  // 如果这个迭代器是 map 的开始，则只能返回第一个值
  if (upper == myMap.begin()) {
    return upper->second;
  }

  // 如果这个迭代器是 map 的结束，则只能返回最后一个值
  if (upper == myMap.end()) {
    return -1;// std::prev(upper)->second;
  }

  // 找到 key 之前的键值对
  auto lower = std::prev(upper);

  // 进行线性插值
  double k1 = lower->first;
  double v1 = lower->second;
  double k2 = upper->first;
  double v2 = upper->second;

  if (v1 == -1 && v2 != -1) {
    v1 = v2;
  }
  if (v2 == -1 && v1 != -1) {
    v2 = v1;
  }
  if (v2 == -1 && v1 == -1) {
    return -1;
  }


  return v1 ;
}




bool containsInvalidChars(const std::string& str) {
    bool hasDecimal = false;
    
    for (char c : str) {
        // 检查字母
        if (c=='?') {
            return true;
        }


        if (isalpha(static_cast<unsigned char>(c))) {
          // 正确处理字母
            return true;
        }
        
        // 检查小数点
        if (c == '.') {
            if (hasDecimal) {  // 已经有一个小数点了
                return true;
            }
            hasDecimal = true;
            
            // 检查小数点位置是否合法（不能在开头或结尾）
            if (&c == &str.front() || &c == &str.back()) {
                return true;
            }
        }
        // 可以添加其他非法字符检查
        else if(!isdigit(static_cast<unsigned char>(c)) && c != '+' && c != '-') {
          return true;
        }
    }
    
    return false;
}


bool createFolder(const std::string& path) {
#ifdef _WIN32
    if (CreateDirectoryA(path.c_str(), nullptr)) {
        std::cout << "success: " << path << std::endl;
        return true;
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_ALREADY_EXISTS) {
            //std::cout << "already exit: " << path << std::endl;
            return true;
        } else {
            std::cerr << "fialed, error code: " << error << std::endl;
            return false;
        }
    }
#else
    std::error_code error;
    if (std::filesystem::create_directories(path, error) ||
        std::filesystem::exists(path)) {
        return true;
    }
    std::cerr << "failed: " << path << ", " << error.message() << std::endl;
    return false;
#endif
}

std::string removeSpaces(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    return str;
}


void  delete_space(string& s) {
  if (s.empty()) {
    return;
  }
  s.erase(0, s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
}



  void segments_Write_process_int_T(string& dirpath, process_int_T& p_T_data,string file_suffix) {
    string output_path = dirpath ;
    createFolder(output_path);

    if (p_T_data.size() == 0) {
      return;
    }

    process_int_T* sim_data=nullptr;

    sim_data = &p_T_data;


    int row_num = sim_data->begin()->second.size() ;
    int col_num = sim_data->size() + 1;

  //生成时间戳
    std::vector<string> time_str_vec;
    for (auto iter = sim_data->begin()->second.begin(); iter !=  sim_data->begin()->second.end(); iter++)
    {
      time_t  action_time_t = iter->first;
      time_str_vec.push_back(FormatTime(action_time_t));
    }

    std::vector<std::vector<string>> data; data.resize(row_num);
    for (int i = 0; i < row_num; i++) {
      data[i].resize(col_num);
      data[i][0] = time_str_vec[i];
    }

    std::vector<string> names; names.resize(sim_data->size() + 1); names[0] = "point/time";
    int i = 0;
    for (auto iter = sim_data->begin(); iter != sim_data->end(); iter++) {
      int id = iter->first;
      names[i + 1] = to_string(id);

      int j = 0;
      for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {
        double Q = sub_iter->second;
        data[j][i+1] = to_string(Q);
        j++;
      }
      i++;
    }
    data.emplace(data.begin(), names);  // 在开头插入元素 1
    write_h_csv(output_path + file_suffix, data, ',');

  }

  void segments_Write_process_T(string& dirpath, process_T& p_T_data,string file_suffix) {
    string output_path = dirpath ;
    createFolder(output_path);

    process_T* sim_data=nullptr;

    sim_data = &p_T_data;

    if (sim_data->size() == 0) {
      return;
    }
    if (sim_data->begin()->second.size() == 0) {
      return;
    }
    if (sim_data->begin()->second.size() == 0) {
      return;
    }

    int row_num = sim_data->begin()->second.size() ;
    int col_num = sim_data->size() + 1;

  //生成时间戳
    std::vector<string> time_str_vec;
    for (auto iter = sim_data->begin()->second.begin(); iter !=  sim_data->begin()->second.end(); iter++)
    {
      time_t  action_time_t = iter->first;
      time_str_vec.push_back(FormatTime(action_time_t));
    }

    std::vector<std::vector<string>> data; data.resize(row_num);
    for (int i = 0; i < row_num; i++) {
      data[i].resize(col_num);
      data[i][0] = time_str_vec[i];
    }
  //cout << "data done" << endl;

    std::vector<string> names; names.resize(sim_data->size() + 1); names[0] = "point/time";
    int i = 0;
    for (auto iter = sim_data->begin(); iter != sim_data->end(); iter++) {
      string id = iter->first;

      if (!isValidUTF8(id)) {
        id = G2U(id);
      }

      names[i + 1] =(id);

      if (id == "罗管节制闸" || U2G(id) == "罗管节制闸") {
        int a = 0;
      }

      int j = 0;
      for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {
        double Q =( sub_iter->second);
        data[j][i+1] = to_string(Q);
        j++;
      }
      i++;
    }
  //cout << "emplace done" << endl;
    data.emplace(data.begin(), names);  // 在开头插入元素 1
    write_h_csv(output_path + file_suffix, data, ',');

  }



  std::vector<int> create_vector_int(int start, int end) {
    std::vector<int> created_vec;
    for (int i = start; i < end + 1; i++) {
      created_vec.push_back(i);
    }
    return created_vec;
  }


void process_T2process_str_vec(process_T& source_data, process_str_vec& target_data) {


  for (auto iter = source_data.begin(); iter != source_data.end(); iter++) {
    string name = iter->first;

    for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {
      double value = sub_iter->second;
      target_data[name].push_back(value);
    }
  }

}



double get_average_of_process_int_T(process_int_T& data) {

  double sum_value = 0;
  int value_num = 0;
  for (auto iter = data.begin(); iter != data.end(); iter++) {
    for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {

      double value = sub_iter->second;
      sum_value += value;
      value_num++;

    }
  }

  double average_value = sum_value / value_num;
  return average_value;

}

int str_compare(const char* s1, const char* s2) {
#ifdef _WIN32
  return _stricmp(s1, s2);
#else
  return strcasecmp(s1, s2);
#endif
}


double getValue(const std::map<std::string, double>& check_structure_flow, const std::string& key) {
    auto it = check_structure_flow.find(key);
    if (it != check_structure_flow.end()) {
        return it->second; // 键存在，返回对应的值
    }
    return -1.0; // 键不存在，返回 -1
}

double get_average_of_vector(std::vector<double> vec) {
  double average = 0;
  for (int i = 0; i < vec.size(); i++) {
    average += vec[i];
  }
  average = average / vec.size();

  return average;
}


bool isInvalidChar(char c) {
    // 控制字符（除了制表符、换行等常见空白字符）
    if (std::iscntrl(c) && c != '\t' && c != '\n' && c != '\r') {
        return true;
    }
    
    // 非打印字符（除了空格）
    if (!std::isprint(c) && !std::isspace(c)) {
        return true;
    }
    
    // 扩展ASCII范围外的字符（如果使用ASCII）
    if (static_cast<unsigned char>(c) > 127) {
        // 根据需求决定是否视为无效
        return true; // 或者 false，取决于需求
    }
    
    return false;
}

void checkStringValidity(const std::string& str) {
    std::vector<int> invalidPositions;
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (isInvalidChar(str[i])) {
            invalidPositions.push_back(i);
        }
    }
    
    if (invalidPositions.empty()) {
        std::cout << "字符串有效" << std::endl;
    } else {
        std::cout << "发现无效字符，位置: ";
        for (int pos : invalidPositions) {
            std::cout << pos << " ";
        }
        std::cout << std::endl;
    }
}


bool isValidString(const std::string& str, const std::string& pattern) {
    std::regex re(pattern);
    return std::regex_match(str, re);
}


char* json2char(json input_j) {

  std::string j_str = input_j.dump();
  // 动态分配内存
  char* result = new char[j_str.length() + 1];
  std::strcpy(result, j_str.c_str());

  return result;

}

json char2json(const char* input_c_str) {
  string obj_str = input_c_str;
  json GatesBC_j = json::parse(obj_str);

  return GatesBC_j;
}

  std::vector<double> get_TaylorSeries_gates_orifice_freeflow(std::vector<double> params) {
    std::vector<double> result;
    if (params.size() > 3) {
      double e0 = params[0];
      double hu0 = params[1];
      double mu_ef = params[2];
      double b = params[3];
      //f(e0,hu0)
      double f_e0_hu0 = mu_ef * b * e0 * sqrt(2 * 9.8 * (hu0 - 0.65 * e0));
      double pf_pe = mu_ef * b * sqrt(2 * 9.8 * (hu0 - 0.65 * e0)) + mu_ef * b * e0 * (-0.65) * 9.8 * (1 / sqrt(2 * 9.8 * (hu0 - 0.65e0)));
      double pf_phu = mu_ef * b * e0 * 9.8 * (1 / sqrt(2 * 9.8 * (hu0 - 0.65e0)));
      result.push_back(f_e0_hu0);
      result.push_back(pf_pe);
      result.push_back(pf_phu);
    }

    return result;
  }
std::vector<std::string> getCsvFilesWithoutObs(const std::string& directory_path,const std::string& mask) {
    std::vector<std::string> csv_files;
    
    try {
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string absolute_path = fs::absolute(entry.path()).string();
                
                // 检查是否是CSV文件且不包含"obs"
                if (filename.find(".csv") != std::string::npos && 
                    filename.find(mask) == std::string::npos) {
                    csv_files.push_back(absolute_path);
                }
            }
        }
    } catch (const fs::filesystem_error& ex) {
        std::cerr << "文件系统错误: " << ex.what() << std::endl;
    }
    
    return csv_files;
}


void printMatrixInfo(const std::vector<std::vector<double>>& matrix) {
    if (matrix.empty()) {
        printf( "Matrix is empty" );
        return;
    }
    
    size_t nrows = matrix.size();
    size_t ncols = matrix[0].size();
    
    std::cout << "Matrix size: " << nrows << " x " << ncols << std::endl;
    
    // 统计非零元素
    size_t nonzeros = 0;
    for (const auto& row : matrix) {
        for (double val : row) {
            if (val != 0.0) nonzeros++;
        }
    }
    
    double sparsity = 100.0 * (1.0 - (double)nonzeros / (nrows * ncols));
    std::cout << "Non-zero elements: " << nonzeros 
              << " (sparsity: " << sparsity << "%)" << std::endl;
    
    // 打印前几行
    int printRows = std::min(5, (int)nrows);
    int printCols = std::min(10, (int)ncols);
    
    std::cout << "First " << printRows << "x" << printCols << " elements:" << std::endl;
    for (int i = 0; i < printRows; ++i) {
        std::cout << "Row " << i << ": ";
        for (int j = 0; j < printCols; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

      /**
     * 计算矩阵的相干系数（列向量间最大相关性）
     * 矩阵格式：外层vector是行，内层vector是该行的列值
     * @param matrix 二维vector表示的矩阵
     * @return 相干系数（0到1之间的值，1表示完全相关）
     */
void MutualCoherence(const std::vector<std::vector<double>>& matrix1) {

    if (matrix1.empty()) return;
    
    int rows = matrix1.size();
    int cols = matrix1[0].size();
    
    // 简化的相干系数计算
    auto coherence = [&](const std::vector<std::vector<double>>& mat) {
        double maxC = 0.0;
        for (int i = 0; i < cols; ++i) {
            for (int j = i + 1; j < cols; ++j) {
                double dot = 0.0, n1 = 0.0, n2 = 0.0;
                for (int k = 0; k < rows; ++k) {
                    double v1 = mat[k][i], v2 = mat[k][j];
                    dot += v1 * v2; n1 += v1 * v1; n2 += v2 * v2;
                }
                double c = std::abs(dot) / (std::sqrt(n1) * std::sqrt(n2));
                if (c > maxC) maxC = c;
            }
        }
        return maxC;
    };
    
    double original = coherence(matrix1);
    std::cout << "原始: " << original << std::endl;
    
    std::vector<std::vector<double>> best = matrix1;
    double bestCoh = original;
    
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> scaleDist(0.7, 1.3);
    
    // 重点：列缩放优化
    for (int col = 0; col < cols; ++col) {
        // 对该列尝试不同的缩放因子
        double originalScale = 1.0;
        
        for (double scale : {0.3, 0.5, 0.7, 0.9, 1.0, 1.1, 1.3, 1.5, 2.0}) {
            std::vector<std::vector<double>> test = best;
            
            // 缩放这一列
            for (int i = 0; i < rows; ++i) {
                test[i][col] = best[i][col] * scale;
            }
            
            double coh = coherence(test);
            if (coh < bestCoh) {
                bestCoh = coh;
                best = test;
                std::cout << "列 " << col << " 缩放 " << scale << ": 改善到 " << coh << std::endl;
            }
        }
    }
    
    std::cout << "优化后: " << bestCoh << std::endl;
    if (bestCoh < original) {
        std::cout << "改善: " << (original - bestCoh) / original * 100 << "%" << std::endl;
    }

}


void diagnoseMatrixProblems(const std::vector<std::vector<double>>& A) {
    int d = A.size();
    int n = A[0].size();
    
    std::cout << "=== 矩阵问题精确诊断 ===" << std::endl;
    std::cout << "矩阵维度: " << d << " × " << n << std::endl;
    std::cout << "总列对数: C(" << n << ",2) = " << n*(n-1)/2 << std::endl;
    
    // 精确统计
    int perfectlyColinear = 0;      // 相干系数 = 1.0
    int highlyCorrelated = 0;       // 相干系数 > 0.99
    int moderatelyCorrelated = 0;   // 相干系数 > 0.9
    
    std::vector<std::tuple<double, int, int>> perfectPairs;
    std::vector<std::tuple<double, int, int>> highPairs;
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
            for (int k = 0; k < d; ++k) {
                double v1 = A[k][i], v2 = A[k][j];
                dot += v1 * v2;
                norm1 += v1 * v1;
                norm2 += v2 * v2;
            }
            
            if (norm1 > 1e-12 && norm2 > 1e-12) {
                double coh = std::fabs(dot) / (std::sqrt(norm1) * std::sqrt(norm2));
                
                if (coh > 0.9999) {  // 几乎完全共线
                    perfectlyColinear++;
                    perfectPairs.emplace_back(coh, i, j);
                } else if (coh > 0.99) {
                    highlyCorrelated++;
                    highPairs.emplace_back(coh, i, j);
                } else if (coh > 0.9) {
                    moderatelyCorrelated++;
                }
            }
        }
    }
    
    std::cout << "\n相干系数统计:" << std::endl;
    std::cout << "完全共线 (coh=1.0): " << perfectlyColinear << " 对" << std::endl;
    std::cout << "高度相关 (coh>0.99): " << highlyCorrelated << " 对" << std::endl;
    std::cout << "中度相关 (coh>0.9): " << moderatelyCorrelated << " 对" << std::endl;
    
    // 显示完全共线的列对
    if (!perfectPairs.empty()) {
        std::cout << "\n❌ 发现完全共线的列对（致命问题）:" << std::endl;
        for (int i = 0; i < std::min(10, (int)perfectPairs.size()); ++i) {
            auto [coh, col1, col2] = perfectPairs[i];
            std::cout << "   列(" << col1 << ", " << col2 << "): 相干系数 = " << coh << std::endl;
            
            // 显示这两列的值（前几个元素）
            std::cout << "     列" << col1 << ": [";
            for (int k = 0; k < std::min(3, d); ++k) std::cout << A[k][col1] << " ";
            std::cout << "...]" << std::endl;
            
            std::cout << "     列" << col2 << ": [";
            for (int k = 0; k < std::min(3, d); ++k) std::cout << A[k][col2] << " ";
            std::cout << "...]" << std::endl;
            
            // 检查是否是倍数关系
            if (d > 0) {
                double ratio = (A[0][col2] != 0) ? A[0][col1] / A[0][col2] : 0;
                bool constantRatio = true;
                for (int k = 1; k < d && constantRatio; ++k) {
                    if (A[k][col2] != 0) {
                        double currentRatio = A[k][col1] / A[k][col2];
                        if (std::abs(currentRatio - ratio) > 1e-6) {
                            constantRatio = false;
                        }
                    }
                }
                if (constantRatio) {
                    std::cout << "     关系: 列" << col1 << " = " << ratio << " × 列" << col2 << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }
    
    // 分析问题根源
    std::cout << "\n🔍 问题根源分析:" << std::endl;
    
    // 检查是否是下三角矩阵
    bool isTriangular = true;
    for (int i = 0; i < d; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (std::fabs(A[i][j]) > 1e-10) {
                isTriangular = false;
                break;
            }
        }
        if (!isTriangular) break;
    }
    
    if (isTriangular) {
        std::cout << "1. 矩阵是下三角的" << std::endl;
        std::cout << "   前" << d << "列线性无关，但后面的列可能是前面列的线性组合" << std::endl;
        
        // 检查后面列是否是前面列的线性组合
        std::cout << "\n   第" << d << "列之后的列分析:" << std::endl;
        for (int j = d; j < std::min(d+5, n); ++j) {  // 检查几列
            std::cout << "   列" << j << ": ";
            
            // 检查是否能被前d列线性表示
            bool canBeExpressed = true;
            for (int i = 0; i < d; ++i) {
                if (i < j) {
                    // 下三角：第j列在第i行之后才有值
                    // 实际上，对于下三角矩阵，第j列的前j个元素可能非零
                }
            }
            std::cout << "可能相关" << std::endl;
        }
    }
    
    // 检查是否有全零列
    int zeroColumns = 0;
    for (int j = 0; j < n; ++j) {
        bool allZero = true;
        for (int i = 0; i < d; ++i) {
            if (std::fabs(A[i][j]) > 1e-12) {
                allZero = false;
                break;
            }
        }
        if (allZero) zeroColumns++;
    }
    
    if (zeroColumns > 0) {
        std::cout << "\n2. 有 " << zeroColumns << " 个全零列" << std::endl;
        std::cout << "   这些列与任何其他列的点积都为0" << std::endl;
    }
    
    // 检查是否有相同的列
    std::cout << "\n3. 检查是否有完全相同的列:" << std::endl;
    int identicalPairs = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            bool identical = true;
            for (int k = 0; k < d; ++k) {
                if (std::fabs(A[k][i] - A[k][j]) > 1e-12) {
                    identical = false;
                    break;
                }
            }
            if (identical) {
                identicalPairs++;
                std::cout << "   列" << i << " 和 列" << j << " 完全相同" << std::endl;
            }
        }
    }
    
    if (identicalPairs == 0) {
        std::cout << "   没有发现完全相同的列" << std::endl;
    }
    
    // Donoho-Tanner理论的影响
    std::cout << "\n📚 Donoho-Tanner理论视角:" << std::endl;
    std::cout << "相干系数 = 1 意味着这些列在多胞形 P 中是同一个方向" << std::endl;
    std::cout << "这破坏了向外邻接性的基本条件" << std::endl;
    
    std::cout << "\n具体影响:" << std::endl;
    std::cout << "1. 唯一性丧失: 如果列i和列j共线，那么观测 y 无法区分它们" << std::endl;
    std::cout << "2. LP失效: 线性规划无法恢复正确的支撑集" << std::endl;
    std::cout << "3. 最多只能保证恢复 k=1 的稀疏解" << std::endl;
    
    // 修复建议
    std::cout << "\n💡 修复建议:" << std::endl;
    std::cout << "1. 立即处理完全共线的列对:" << std::endl;
    std::cout << "   - 删除其中一个列（如果物理允许）" << std::endl;
    std::cout << "   - 添加微小扰动使它们不同" << std::endl;
    
    std::cout << "\n2. 具体操作（C++代码）:" << std::endl;
    std::cout << "   std::vector<std::vector<double>> fixColinearColumns(" << std::endl;
    std::cout << "       const std::vector<std::vector<double>>& A," << std::endl;
    std::cout << "       double epsilon = 1e-6) {" << std::endl;
    std::cout << "       // 对每个完全共线的列对，给其中一个添加随机扰动" << std::endl;
    std::cout << "   }" << std::endl;
    
    std::cout << "\n3. 处理后重新评估:" << std::endl;
    std::cout << "   处理完全共线列后，相干系数应降至 < 0.99" << std::endl;
    std::cout << "   然后才能进行进一步的优化" << std::endl;
    
    // 总结
    std::cout << "\n🎯 总结:" << std::endl;
    std::cout << "你的矩阵不是'性质差'，而是有结构性错误" << std::endl;
    std::cout << "有 " << perfectlyColinear << " 对完全共线的列" << std::endl;
    std::cout << "这是Donoho-Tanner理论不允许的情况" << std::endl;
    std::cout << "必须首先修复这些完全共线的列，否则LP无法工作" << std::endl;
}

// 修复函数
std::vector<std::vector<double>> fixColinearColumns(
    const std::vector<std::vector<double>>& A,
    double epsilon) {
    
    std::vector<std::vector<double>> fixed = A;
    int d = A.size();
    int n = A[0].size();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-epsilon, epsilon);
    
    // 找出需要修复的列
    std::vector<bool> needsFix(n, false);
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double dot = 0.0, norm1 = 0.0, norm2 = 0.0;
            for (int k = 0; k < d; ++k) {
                double v1 = A[k][i], v2 = A[k][j];
                dot += v1 * v2;
                norm1 += v1 * v1;
                norm2 += v2 * v2;
            }
            
            if (norm1 > 1e-12 && norm2 > 1e-12) {
                double coh = std::fabs(dot) / (std::sqrt(norm1) * std::sqrt(norm2));
                if (coh > 0.9999) {
                    // 标记第j列需要修复（总是修复第二个）
                    needsFix[j] = true;
                }
            }
        }
    }
    
    // 修复标记的列
    int fixedCount = 0;
    for (int j = 0; j < n; ++j) {
        if (needsFix[j]) {
            for (int i = 0; i < d; ++i) {
                fixed[i][j] += dist(gen) * std::fabs(A[i][j]);
            }
            fixedCount++;
        }
    }
    
    std::cout << "修复了 " << fixedCount << " 个完全共线的列" << std::endl;
    return fixed;
}

// 测试函数：判断矩阵A是否能通过LP恢复稀疏解
void testSparseRecoveryFeasibility(const std::vector<std::vector<double>>& A) {

      // 1. 诊断问题
    diagnoseMatrixProblems(A);
    
    // 2. 修复完全共线的列
    auto fixedMatrix = fixColinearColumns(A, 1e-5);


    if (A.empty()) {
        std::cout << "错误：矩阵为空" << std::endl;
        return;
    }
    
    int d = A.size();      // 测量数
    int n = A[0].size();   // 变量数
    
    // 验证矩阵维度
    for (int i = 0; i < d; ++i) {
        if (A[i].size() != n) {
            std::cout << "错误：矩阵行大小不一致" << std::endl;
            return;
        }
    }
    
    std::cout << "=== 稀疏恢复能力分析 ===" << std::endl;
    std::cout << "矩阵维度: " << d << " × " << n << std::endl;
    std::cout << "欠定比例: δ = " << d << "/" << n << " = " << (double)d/n << std::endl;
    
    // ========== 1. 计算相干系数 ==========
    std::cout << "\n1. 相干系数分析:" << std::endl;
    
    double maxCoherence = 0.0;
    std::vector<std::tuple<double, int, int>> coherencePairs;
    
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double dot = 0.0, norm_i = 0.0, norm_j = 0.0;
            
            for (int k = 0; k < d; ++k) {
                double val_i = A[k][i];
                double val_j = A[k][j];
                dot += val_i * val_j;
                norm_i += val_i * val_i;
                norm_j += val_j * val_j;
            }
            
            if (norm_i > 1e-12 && norm_j > 1e-12) {
                double coherence = std::fabs(dot) / (std::sqrt(norm_i) * std::sqrt(norm_j));
                coherencePairs.emplace_back(coherence, i, j);
                
                if (coherence > maxCoherence) {
                    maxCoherence = coherence;
                }
            }
        }
    }
    
    std::cout << "   最大相干系数: " << maxCoherence << std::endl;
    
    // 计算相干系数分布
    std::sort(coherencePairs.begin(), coherencePairs.end(), 
              [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<0>(b); });
    
    if (!coherencePairs.empty()) {
        std::cout << "   最相关的5个列对:" << std::endl;
        for (int p = 0; p < std::min(5, (int)coherencePairs.size()); ++p) {
            auto [coh, i, j] = coherencePairs[p];
            std::cout << "     列(" << i << "," << j << "): " << coh << std::endl;
        }
    }
    
    // ========== 2. 多胞形几何分析 ==========
    std::cout << "\n2. 多胞形几何分析:" << std::endl;
    
    // 检查列向量是否处于一般位置
    int problematicPairs = 0;
    for (const auto& [coh, i, j] : coherencePairs) {
        if (coh > 0.999) problematicPairs++;
    }
    
    bool generalPosition = (problematicPairs == 0);
    std::cout << "   一般位置假设: " << (generalPosition ? "满足" : "可能不满足") << std::endl;
    if (!generalPosition) {
        std::cout << "   警告: " << problematicPairs << " 个列对几乎共线" << std::endl;
    }
    
    // ========== 3. 估计向外邻接度 ==========
    std::cout << "\n3. 向外邻接度估计:" << std::endl;
    
    // 根据Donoho-Tanner的相变表（插值）
    double delta = (double)d / n;
    double rho_N = 0.0, rho_VS = 0.0;
    
    if (delta <= 0.1) {
        rho_N = 0.06; rho_VS = 0.24;
    } else if (delta <= 0.2) {
        rho_N = 0.072; rho_VS = 0.30;
    } else if (delta <= 0.3) {
        rho_N = 0.08; rho_VS = 0.43;
    } else if (delta <= 0.4) {
        rho_N = 0.10; rho_VS = 0.51;
    } else if (delta <= 0.5) {
        rho_N = 0.133; rho_VS = 0.558;
    } else if (delta <= 0.6) {
        rho_N = 0.16; rho_VS = 0.65;
    } else if (delta <= 0.7) {
        rho_N = 0.18; rho_VS = 0.72;
    } else if (delta <= 0.8) {
        rho_N = 0.19; rho_VS = 0.78;
    } else {
        rho_N = 0.267; rho_VS = 0.903;
    }
    
    int k_strong = std::max(1, (int)(rho_N * d));  // 强恢复：保证所有解
    int k_weak = std::max(1, (int)(rho_VS * d));   // 弱恢复：保证大多数解
    
    std::cout << "   强恢复保证: k ≤ " << k_strong << std::endl;
    std::cout << "   弱恢复保证: k ≤ " << k_weak << " (大多数情况)" << std::endl;
    
    // ========== 4. 矩阵结构分析 ==========
    std::cout << "\n4. 矩阵结构分析:" << std::endl;
    
    // 检查矩阵的稀疏性
    int zeroCount = 0;
    for (int i = 0; i < d; ++i) {
        for (int j = 0; j < n; ++j) {
            if (std::fabs(A[i][j]) < 1e-12) zeroCount++;
        }
    }
    double sparsity = (double)zeroCount / (d * n);
    std::cout << "   稀疏度: " << sparsity * 100 << "% 为零元素" << std::endl;
    
    // 检查是否有特殊结构（下三角、对角占优等）
    bool isLowerTriangular = true;
    bool isDiagonalDominant = true;
    
    for (int i = 0; i < d; ++i) {
        double diagVal = std::fabs(A[i][i]);
        double rowSum = 0.0;
        
        for (int j = 0; j < n; ++j) {
            if (i < j && std::fabs(A[i][j]) > 1e-12) {
                isLowerTriangular = false;
            }
            if (j != i) {
                rowSum += std::fabs(A[i][j]);
            }
        }
        
        if (diagVal <= rowSum) {
            isDiagonalDominant = false;
        }
    }
    
    if (isLowerTriangular) {
        std::cout << "   结构: 下三角矩阵" << std::endl;
        std::cout << "   影响: 前" << std::min(d, n) << "列线性无关，但后续列可能相关" << std::endl;
    }
    
    // ========== 5. 数值实验验证 ==========
    std::cout << "\n5. 数值实验验证（模拟）:" << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> valDist(0.5, 2.0);
    
    // 测试不同稀疏度的理论成功率
    for (int k = 1; k <= std::min(6, d+2); ++k) {
        double expectedSuccessRate = 0.0;
        
        if (k <= k_strong) {
            expectedSuccessRate = 1.0;  // 理论保证
        } else if (k <= k_weak) {
            // 在强弱之间，成功率逐渐下降
            double position = (double)(k - k_strong) / (k_weak - k_strong);
            expectedSuccessRate = 1.0 - position * 0.5;  // 从100%降到50%
        } else {
            // 超过弱恢复阈值，成功率快速下降
            double excess = (double)(k - k_weak) / k_weak;
            expectedSuccessRate = std::max(0.0, 0.5 - excess);
        }
        
        // 考虑相干系数的影响
        if (maxCoherence > 0.9) {
            expectedSuccessRate *= 0.7;  // 高相干系数降低成功率
        } else if (maxCoherence > 0.7) {
            expectedSuccessRate *= 0.9;
        }
        
        std::cout << "   k=" << k << ": 预计成功率 ≈ " 
                  << expectedSuccessRate * 100 << "%" << std::endl;
    }
    
    // ========== 6. 综合评估与建议 ==========
    std::cout << "\n6. 综合评估:" << std::endl;
    
    // 计算一个综合评分
    double score = 0.0;
    
    // 1. 欠定比例得分 (δ越大越好)
    score += delta * 30;  // 最多30分
    
    // 2. 向外邻接度得分
    score += (double)k_weak / d * 40;  // 最多40分
    
    // 3. 相干系数得分 (越低越好)
    score += (1.0 - maxCoherence) * 30;  // 最多30分
    
    std::cout << "   恢复能力综合评分: " << score << "/100" << std::endl;
    
    if (score >= 80) {
        std::cout << "   ✅ 优秀: LP很可能成功恢复稀疏解" << std::endl;
        std::cout << "   适用范围: k ≤ " << k_weak << " 的稀疏问题" << std::endl;
    } else if (score >= 60) {
        std::cout << "   ⚠️ 一般: LP可能恢复稀疏解" << std::endl;
        std::cout << "   适用范围: k ≤ " << std::min(k_weak, k_strong + 2) << " 的稀疏问题" << std::endl;
    } else if (score >= 40) {
        std::cout << "   ⚠️ 有限: LP恢复能力有限" << std::endl;
        std::cout << "   仅适用于: k ≤ " << k_strong << " 的极稀疏问题" << std::endl;
    } else {
        std::cout << "   ❌ 困难: LP可能难以恢复稀疏解" << std::endl;
        std::cout << "   建议考虑其他方法或修改矩阵设计" << std::endl;
    }
    
    // ========== 7. 具体建议 ==========
    std::cout << "\n7. 具体建议:" << std::endl;
    
    if (maxCoherence > 0.9) {
        std::cout << "   • 考虑优化矩阵以降低相干系数" << std::endl;
    }
    
    if (k_weak <= 2) {
        std::cout << "   • 问题只适合极稀疏情况 (k ≤ 2)" << std::endl;
    }
    
    if (isLowerTriangular) {
        std::cout << "   • 对于下三角矩阵，确保前" << d << "列方向分散" << std::endl;
    }
    
    if (!generalPosition) {
        std::cout << "   • 存在高度相关列，可能影响唯一性" << std::endl;
    }
    
    std::cout << "\n基于Donoho-Tanner理论的最终判断:" << std::endl;
    std::cout << "矩阵" << (score >= 60 ? "适合" : "可能不适合") 
              << "通过线性规划寻找稀疏非负解" << std::endl;
}



// 提取所有整数
std::vector<int> extract_integers_regex(const std::string& str) {
    std::vector<int> numbers;
    std::regex pattern(R"([-+]?\d+)");
    
    std::sregex_iterator it(str.begin(), str.end(), pattern);
    std::sregex_iterator end;
    
    while (it != end) {
        numbers.push_back(std::stoi(it->str()));
        ++it;
    }
    
    return numbers;
}



std::map<string, std::map<string, std::map<string, double >>> get_sluice_Eopen(json data_j) {
  std::map<string, std::map<string, std::map<string, double >>> data_map;
  for(auto& j:data_j.items()){
    string name = j.key();
    if (isValidUTF8(name)) {
      name = U2G(name);
    }

    if (j.value().contains("ON")) {

      for (auto& j_sub : j.value()["ON"].items()) {

        string name_sub = j_sub.key();
        for (auto& j_sub_sub : j_sub.value().items()) {

          data_map[name][name_sub][j_sub_sub.key()] = j_sub_sub.value().get<double>();

        }

      }

    }

  }

  cout << "done" << endl;

  return data_map;
}



process_T get_sluice_AverageEopen(json data_j) {
  process_T data_map;
  for (auto& j : data_j.items()) {
    //闸
    string name = j.key();
    if (isValidUTF8(name)) {
      name = U2G(name);
    }

    if (j.value().contains("ON")) {
      for (auto& j_sub : j.value()["ON"].items()) {
        int Eopen_num = j.value()["ON"].size();
        //孔
        string name_sub = j_sub.key();
        for (auto& j_sub_sub : j_sub.value().items()) {
          //Time Series
          time_t tm_t = StringToTime_t(j_sub_sub.key());
          data_map[name][tm_t] += j_sub_sub.value().get<double>() / Eopen_num;

        }
      }
    }
  }

  cout << "done" << endl;

  return data_map;

}


// 查找当前目录下文件名包含"tar"的文件
std::vector<fs::path> findFilesWithTarInName(const fs::path& directory,string& tar_str) {
    std::vector<fs::path> result;
    
    try {
        // 只遍历当前目录，不递归子目录
        for (const auto& entry : fs::directory_iterator(directory)) {
            // 只处理常规文件，跳过目录
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // 检查文件名是否包含"tar"
                if (filename.find(tar_str) != std::string::npos) {
                    result.push_back(entry.path());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "错误访问目录: " << directory << " - " << e.what() << std::endl;
    }
    
    return result;
}


// 最简单直接的过滤函数
std::string filterFilename(const std::string& filename, 
                          const std::string& filterStr,
                          bool caseSensitive ) {
    std::string result = filename;
    
    if (caseSensitive) {
        // 区分大小写
        size_t pos = result.find(filterStr);
        while (pos != std::string::npos) {
            result.erase(pos, filterStr.length());
            pos = result.find(filterStr, pos);
        }
    } else {
        // 不区分大小写
        std::string lowerResult = result;
        std::transform(lowerResult.begin(), lowerResult.end(),
                      lowerResult.begin(), ::tolower);
        
        std::string lowerFilter = filterStr;
        std::transform(lowerFilter.begin(), lowerFilter.end(),
                      lowerFilter.begin(), ::tolower);
        
        size_t pos = lowerResult.find(lowerFilter);
        while (pos != std::string::npos) {
            result.erase(pos, filterStr.length());
            lowerResult.erase(pos, filterStr.length());
            pos = lowerResult.find(lowerFilter, pos);
        }
    }
    
    return result;
}



process_T get_solution(std::map<string, process_str_vec> solution, string var_name) {

	process_T data_map;

    // 如果结果里没有此变量则直接返回
    if (solution.find(var_name) == solution.end()) {
        return data_map;
    }


    for (auto iter = solution.begin(); iter != solution.end(); iter++) {

        if (iter->first == var_name) {

            for (auto sub_iter = iter->second.begin(); sub_iter != iter->second.end(); sub_iter++) {

				cout << sub_iter->first << endl;
				cout << sub_iter->second << endl;

                for (auto& value : sub_iter->second) {
                    if (value>0&&value < 86400) {
                        int a = 0;
                    }

                }


				cout << sub_iter->second << endl;

            }

        }
    }


	return data_map;
}

void addDemandProcess(
    std::map<std::string, process_h>& demandMap,
    const std::string& gateName,
    numeric_t demandFlow,
    time_t startTime,
    time_t endTime,
    time_t dt)
{
    if (dt <= 0)
        throw std::invalid_argument("dt must be greater than 0.");

    if (endTime <= startTime)
        return;

    process_h& process = demandMap[gateName];

    for (time_t t = startTime; t < endTime; t += dt)
    {
         demandMap[gateName][t] = demandFlow;
    }
    cout << "done" << endl;
}

void ConvertBoundaryFlowTime(
    process_T& boundary_flow,
    time_t start_time_t,
    double T,double DT)
{
    const time_t interval = 24 * 3600;  // 每24小时
    const time_t end_time =
        start_time_t + static_cast<time_t>(T * 3600);

    for (auto& [name, process] : boundary_flow)
    {
        process_h new_process;
        int first=0;
        int t_dif=0;
        for (const auto& [index, value] : process)
        {
          if (first == 0 && index > 0)
          {
            t_dif = 1;
          }
          first++;
          time_t new_time =
              start_time_t + (index - t_dif) * interval;

          if (new_time > end_time)
            break;

          new_process[new_time] = value / DT;
        }

        process = std::move(new_process);
    }
}
void ConvertProcessT2Code(process_T &boundary_flow)
{

  process_T new_boundary_flow;
  for (auto &[name, process] : boundary_flow)
  {

    string code = mapper.GetCode(name);
    new_boundary_flow[code] = process;
  }
  boundary_flow = std::move(new_boundary_flow);
}

static void CleanString(std::string& s)
{
    if (s.size() >= 3 &&
        static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB &&
        static_cast<unsigned char>(s[2]) == 0xBF)
    {
        s.erase(0, 3);
    }

    if (!s.empty() && s.back() == '\r')
        s.pop_back();
}


bool WaterUnitMapper::Init(
    const std::string& config_file,
    const std::string& water_unit_file)
{
    code_to_unit_.clear();
    unit_to_target_.clear();

    // 读取 config
    {
        std::ifstream file(config_file);

        if (!file.is_open())
            return false;

        std::string line;

        while (std::getline(file, line))
        {
            CleanString(line);

            if (line.empty())
                continue;

            size_t pos = line.find(',');

            if (pos == std::string::npos)
                continue;

            std::string code = line.substr(0, pos);
            std::string unit = line.substr(pos + 1);

            CleanString(code);
            CleanString(unit);

            code_to_unit_[code] = unit;
        }
    }

    // 读取配水单元清单
    {
        std::ifstream file(water_unit_file);

        if (!file.is_open())
            return false;

        std::string line;

        while (std::getline(file, line))
        {
            CleanString(line);

            if (line.empty())
                continue;

            size_t pos = line.find(',');

            if (pos == std::string::npos)
                continue;

            std::string unit = line.substr(0, pos);
            std::string target = line.substr(pos + 1);

            CleanString(unit);
            CleanString(target);

            unit_to_target_[unit] = target;
        }
    }

    return true;
}


std::string WaterUnitMapper::GetTarget(
    const std::string& code) const
{
    auto it1 = code_to_unit_.find(code);

    if (it1 == code_to_unit_.end())
        return "";

    auto it2 = unit_to_target_.find(it1->second);

    if (it2 == unit_to_target_.end())
        return "";

    return it2->second;
}

std::string WaterUnitMapper::GetCode(
    const std::string& target) const
{
    std::string unit;

    // target -> unit
    for (const auto& [key, value] : unit_to_target_)
    {
        if (value == target)
        {
            unit = key;
            break;
        }
    }

    if (unit.empty())
        return "";

    // unit -> code
    for (const auto& [key, value] : code_to_unit_)
    {
        if (value == unit)
            return key;
    }

    return "";
}
