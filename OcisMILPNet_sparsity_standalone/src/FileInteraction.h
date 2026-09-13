/*****************************************************************//**
 * \file   FileInteraction.h
 * \brief  file handle method of houvert
 *
 * \author houvert
 * \date   November 2022
 *********************************************************************/
#ifndef FILEINTERACTION
#define FILEINTERACTION

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#define _access access
#define _mkdir(path) mkdir((path), 0755)
#define _T(x) x
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <iterator>
# include <memory>
#include <unordered_map>
# include <math.h>
# include <stdlib.h>
# include <stdio.h>
#include <iomanip>
#include <functional>
#include <random>
#include <filesystem>
#include <map>
# include <vector>
# include <algorithm>
#include "statistics.h"
#include "./nlohmann/json.hpp"
#include "TimeConversino.h"
#include "SimpleIni.h"

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
    #define PATH_SEPARATOR '\\'
    #define PATH_SEPARATOR_STR "\\"
#else
    #include <iconv.h>
    #define PATH_SEPARATOR '/'
    #define PATH_SEPARATOR_STR "/"
#endif


 //#include "sqlite/sqlite3.h"
using namespace std;
using numeric_t = double;
using process_k = std::map<int, numeric_t>;
using process_h = std::map<time_t, numeric_t>;
using process_T = std::map<string, process_h >;

using process_str_vec = std::map<string, std::vector<numeric_t> >;

using process_obj_T = std::map<string, process_T >;

using process_int_T = std::map<int, process_h >;

using process_obj_int_T = std::map<string, process_int_T >;

using process_id_T = std::map<int, process_int_T >;
using process_strT = std::map<string, numeric_t >;
using h_csv = std::vector<std::vector<string>>;//2D matrix; all values are read as strings,h_csv[row][column]
using h_csv_double = std::vector<std::vector<double>>;
using h_label = std::vector<string>;
using json = nlohmann::json;
extern std::ofstream gateFlowVolumeFile;
extern std::ofstream changeQFile;

void ocis_init_encoding();

void process_T2process_str_vec(process_T& source_data,process_str_vec& target_data);

struct CalculationParams
{

    time_t start_time_t;
    int epoch=1800;
    int dt = 0.5;
    int T = 24;
    string schedule_json_output_path;

    double sim_dt = 3600;
    double sim_dx = 50;
    string sim_inpath = "./";
    string sim_outpath = "./";

    int output_label = 0;
    int sim_vwn_label = 0;
};

// ============================================================
// Input file encoding mode
// ============================================================
enum class TextEncodingMode
{
    Auto,       // Automatically detect UTF-8 or GB18030
    UTF8,       // Force UTF-8 decoding
    GB18030     // Force GBK/GB18030 decoding
};


// ============================================================
// Check whether the entire string is valid UTF-8
// ============================================================
static bool is_valid_utf8(const std::string& text)
{
    const auto* data =
        reinterpret_cast<const unsigned char*>(text.data());

    const std::size_t size = text.size();

    for (std::size_t i = 0; i < size;)
    {
        const unsigned char c = data[i];

        // ASCII
        if (c <= 0x7F)
        {
            ++i;
            continue;
        }

        // Two-byte UTF-8 sequence
        if (c >= 0xC2 && c <= 0xDF)
        {
            if (i + 1 >= size ||
                (data[i + 1] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 2;
            continue;
        }

        // Three-byte UTF-8 sequence: E0
        if (c == 0xE0)
        {
            if (i + 2 >= size ||
                data[i + 1] < 0xA0 ||
                data[i + 1] > 0xBF ||
                (data[i + 2] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 3;
            continue;
        }

        // Three-byte UTF-8 sequence: E1-EC, EE-EF
        if ((c >= 0xE1 && c <= 0xEC) ||
            (c >= 0xEE && c <= 0xEF))
        {
            if (i + 2 >= size ||
                (data[i + 1] & 0xC0) != 0x80 ||
                (data[i + 2] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 3;
            continue;
        }

        // Three-byte UTF-8 sequence: ED, excluding the surrogate range
        if (c == 0xED)
        {
            if (i + 2 >= size ||
                data[i + 1] < 0x80 ||
                data[i + 1] > 0x9F ||
                (data[i + 2] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 3;
            continue;
        }

        // Four-byte UTF-8 sequence: F0
        if (c == 0xF0)
        {
            if (i + 3 >= size ||
                data[i + 1] < 0x90 ||
                data[i + 1] > 0xBF ||
                (data[i + 2] & 0xC0) != 0x80 ||
                (data[i + 3] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 4;
            continue;
        }

        // Four-byte UTF-8 sequence: F1-F3
        if (c >= 0xF1 && c <= 0xF3)
        {
            if (i + 3 >= size ||
                (data[i + 1] & 0xC0) != 0x80 ||
                (data[i + 2] & 0xC0) != 0x80 ||
                (data[i + 3] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 4;
            continue;
        }

        // Four-byte UTF-8 sequence: F4
        if (c == 0xF4)
        {
            if (i + 3 >= size ||
                data[i + 1] < 0x80 ||
                data[i + 1] > 0x8F ||
                (data[i + 2] & 0xC0) != 0x80 ||
                (data[i + 3] & 0xC0) != 0x80)
            {
                return false;
            }

            i += 4;
            continue;
        }

        return false;
    }

    return true;
}


// ============================================================
// Convert GBK/GB18030 to UTF-8
// ============================================================
static bool gb18030_to_utf8(
    const std::string& input,
    std::string& output,
    std::string* error_message = nullptr)
{
    output.clear();

    if (input.empty())
    {
        return true;
    }

#ifdef _WIN32

    if (input.size() >
        static_cast<std::size_t>(INT_MAX))
    {
        if (error_message != nullptr)
        {
            *error_message =
                "输入文本过大，无法执行编码转换";
        }

        return false;
    }

    // Windows code page 54936 corresponds to GB18030
    constexpr UINT source_code_page = 54936;

    const int wide_length =
        MultiByteToWideChar(
            source_code_page,
            0,
            input.data(),
            static_cast<int>(input.size()),
            nullptr,
            0);

    if (wide_length <= 0)
    {
        if (error_message != nullptr)
        {
            *error_message =
                "GB18030 转 UTF-16 失败，错误码：" +
                std::to_string(GetLastError());
        }

        return false;
    }

    std::wstring wide_text(
        static_cast<std::size_t>(wide_length),
        L'\0');

    if (MultiByteToWideChar(
            source_code_page,
            0,
            input.data(),
            static_cast<int>(input.size()),
            wide_text.data(),
            wide_length) <= 0)
    {
        if (error_message != nullptr)
        {
            *error_message =
                "GB18030 转 UTF-16 失败，错误码：" +
                std::to_string(GetLastError());
        }

        return false;
    }

    const int utf8_length =
        WideCharToMultiByte(
            CP_UTF8,
            0,
            wide_text.data(),
            wide_length,
            nullptr,
            0,
            nullptr,
            nullptr);

    if (utf8_length <= 0)
    {
        if (error_message != nullptr)
        {
            *error_message =
                "UTF-16 转 UTF-8 失败，错误码：" +
                std::to_string(GetLastError());
        }

        return false;
    }

    output.resize(
        static_cast<std::size_t>(utf8_length));

    if (WideCharToMultiByte(
            CP_UTF8,
            0,
            wide_text.data(),
            wide_length,
            output.data(),
            utf8_length,
            nullptr,
            nullptr) <= 0)
    {
        output.clear();

        if (error_message != nullptr)
        {
            *error_message =
                "UTF-16 转 UTF-8 失败，错误码：" +
                std::to_string(GetLastError());
        }

        return false;
    }

    return true;

#else

    // Use iconv on Linux/macOS
    iconv_t converter =
        iconv_open("UTF-8", "GB18030");

    if (converter ==
        reinterpret_cast<iconv_t>(-1))
    {
        // Some systems expose only the GBK encoding name
        converter =
            iconv_open("UTF-8", "GBK");
    }

    if (converter ==
        reinterpret_cast<iconv_t>(-1))
    {
        if (error_message != nullptr)
        {
            *error_message =
                "iconv 不支持 GB18030 或 GBK 编码";
        }

        return false;
    }

    std::size_t input_left = input.size();

    char* input_pointer =
        const_cast<char*>(input.data());

    output.resize(
        input.size() * 4 + 16);

    char* output_pointer =
        output.data();

    std::size_t output_left =
        output.size();

    while (true)
    {
        const std::size_t result =
            iconv(
                converter,
                &input_pointer,
                &input_left,
                &output_pointer,
                &output_left);

        if (result !=
            static_cast<std::size_t>(-1))
        {
            break;
        }

        if (errno == E2BIG)
        {
            const std::size_t written =
                output.size() - output_left;

            output.resize(
                output.size() * 2);

            output_pointer =
                output.data() + written;

            output_left =
                output.size() - written;

            continue;
        }

        if (error_message != nullptr)
        {
            if (errno == EILSEQ)
            {
                *error_message =
                    "文件中存在非法的 GBK/GB18030 字节";
            }
            else if (errno == EINVAL)
            {
                *error_message =
                    "文件末尾存在不完整的多字节字符";
            }
            else
            {
                *error_message =
                    "iconv 编码转换失败，errno：" +
                    std::to_string(errno);
            }
        }

        iconv_close(converter);
        output.clear();

        return false;
    }

    const std::size_t written =
        output.size() - output_left;

    output.resize(written);

    iconv_close(converter);

    return true;

#endif
}


// ============================================================
// Generic text-file reader
//
// Features:
// 1. Read arbitrary text files;
// 2. Automatically detect UTF-8 BOM, UTF-8, and GBK/GB18030;
// 3. Normalize output to UTF-8;
// 4. Support Windows, Linux, and macOS;
// 5. Support UTF-8 paths containing Chinese characters on Windows.
// ============================================================
static bool read_text_file_utf8(
    const std::string& filename,
    std::string& utf8_text,
    TextEncodingMode mode = TextEncodingMode::Auto,
    std::string* error_message = nullptr,
    std::string* detected_encoding = nullptr)
{
    utf8_text.clear();

    if (error_message != nullptr)
    {
        error_message->clear();
    }

    if (detected_encoding != nullptr)
    {
        detected_encoding->clear();
    }

#ifdef _WIN32

    // filename is expected to be a UTF-8 path
    const std::filesystem::path file_path =
        std::filesystem::u8path(filename);

    std::ifstream input_file(
        file_path,
        std::ios::binary);

#else

    std::ifstream input_file(
        filename,
        std::ios::binary);

#endif

    if (!input_file.is_open())
    {
        if (error_message != nullptr)
        {
            *error_message =
                "无法打开文件：" + filename;
        }

        return false;
    }

    std::string raw_data{
        std::istreambuf_iterator<char>(input_file),
        std::istreambuf_iterator<char>()
    };

    input_file.close();

    if (raw_data.empty())
    {
        if (detected_encoding != nullptr)
        {
            *detected_encoding = "empty";
        }

        return true;
    }

    // UTF-8 BOM
    const bool has_utf8_bom =
        raw_data.size() >= 3 &&
        static_cast<unsigned char>(raw_data[0]) == 0xEF &&
        static_cast<unsigned char>(raw_data[1]) == 0xBB &&
        static_cast<unsigned char>(raw_data[2]) == 0xBF;

    if (has_utf8_bom)
    {
        utf8_text =
            raw_data.substr(3);

        if (detected_encoding != nullptr)
        {
            *detected_encoding = "UTF-8 BOM";
        }

        return true;
    }

    // Force UTF-8 decoding
    if (mode == TextEncodingMode::UTF8)
    {
        if (!is_valid_utf8(raw_data))
        {
            if (error_message != nullptr)
            {
                *error_message =
                    "文件不是合法的 UTF-8 编码：" +
                    filename;
            }

            return false;
        }

        utf8_text =
            std::move(raw_data);

        if (detected_encoding != nullptr)
        {
            *detected_encoding = "UTF-8";
        }

        return true;
    }

    // Force GB18030 decoding
    if (mode == TextEncodingMode::GB18030)
    {
        if (!gb18030_to_utf8(
                raw_data,
                utf8_text,
                error_message))
        {
            return false;
        }

        if (detected_encoding != nullptr)
        {
            *detected_encoding = "GBK/GB18030";
        }

        return true;
    }

    // Auto: prefer a fully valid UTF-8 interpretation
    if (is_valid_utf8(raw_data))
    {
        utf8_text =
            std::move(raw_data);

        if (detected_encoding != nullptr)
        {
            *detected_encoding = "UTF-8";
        }

        return true;
    }

    // If not UTF-8, try GBK/GB18030
    if (!gb18030_to_utf8(
            raw_data,
            utf8_text,
            error_message))
    {
        if (error_message != nullptr &&
            error_message->empty())
        {
            *error_message =
                "无法识别文件编码：" + filename;
        }

        return false;
    }

    if (detected_encoding != nullptr)
    {
        *detected_encoding = "GBK/GB18030";
    }

    return true;
}

bool isExist(string filepath);
bool isDirExist(const std::string& dirpath);
string fiterstr(string instr);
string filterSpecialChars(const string& instr) ;
string getBeforeLastUnderscore(const string& filename) ;
process_h jsonToProcess_h(json aimjson);
process_k Process_hToprocess_k(process_h aimprocess_h);
void AdjustProcss_kByProp(process_k& input_k, double prop);
nlohmann::json ReadJson(string filename);
void WriteJson(json data, string dirpath, string filename);
std::vector<string> readPaths(string filename);
std::string G2U(const std::string& gbk);
std::string U2G(const std::string& utf8);

void check_elements_is_null(h_csv&,int row,int col);
/**
 * .
 * @brief  Split s by delimiter delim and append each token to elems
 */
void split(const std::string& s, char delim, std::vector<std::string>& elems);

void split_s(const std::string& s, char delim, std::vector<std::string>& elems);
//csv
/**
 * .
 * @brief Read txt/csv files using delimiter; txt uses ' ' or '\t', and csv uses ','
 */
h_csv read_h_csv(string filename, char delimiter);
h_csv read_h_csv_from_string(const std::string& data, char delimiter);
// equal to getline(stringstream,string,delimiter)

bool isValidString(const std::string& str, const std::string& pattern);
void checkStringValidity(const std::string& str) ;
bool isInvalidChar(char c);
process_T read_csv_tm(string path);
process_T read_tidyData(string path,int value_col);
process_T read_tidyData(string path,int obj_col,int tm_col,int value_col);
process_T read_jsonData(string path);

CalculationParams read_Generalization_input_json(string inpath);
CalculationParams read_Generalization_input_json(const char* input_json_c_str);

std::map<string, std::map<string, std::map<string, double >>> get_sluice_Eopen(json data_j);
process_T get_sluice_AverageEopen(json data_j);
std::string filterFilename(const std::string& filename,
  const std::string& filterStr,
  bool caseSensitive );

h_csv create_h_csv(int row,int col);
h_csv_double create_h_csv_double(int row,int col);
std::vector<int> create_vector_int(int start, int end);
bool createFolder(const std::string& path);


// if there no mkpath, mkdir them
void mkdir_h(string mkpath);

bool isValidUTF8(const std::string& string);
bool isPossibleGBK(const std::string& str);



double GateScheduleLinearInterpolate(const std::map<time_t, double>& myMap, time_t key,int interval);
double GateScheduleLinearInterpolate(const std::map<time_t, double>& myMap, time_t key);
double intkey_LinearInterpolate(const std::map<int, double>& myMap, int key);

bool containsInvalidChars(const std::string& str);

std::string removeSpaces(std::string str) ;
void  delete_space(string& s) ;

/**
 * .
 * @brief write txt('\t')  csv(',')
 */
void write_h_csv(string filename, h_csv outcsv, char delimiter);
void write_h_csv_double(string filename, h_csv_double outcsv, char delimiter);

void segments_Write_process_int_T(string& dirpath, process_int_T& p_T_data, string file_suffix);
void segments_Write_process_T(string& dirpath, process_T& p_T_data, string file_suffix);
void write_tidyData(string path,process_T& data);
void write_tidyData(string path,std::map<string,time_t>& data);
void write_tidyData(string path,std::unordered_map<string,double>& data,std::vector<string> label);
void write_tidyData(string path,std::unordered_map<string,string>& data,std::vector<string> label);
void write_process_strT(process_strT data, string dirpath, string filename);

double get_average_of_process_int_T(process_int_T& data) ;
// Overload operator<< for vector output
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  os << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    os << vec[i];
    if (i != vec.size() - 1) {
      os << ", ";  // Insert a comma and space between elements
    }
  }
  os << "]";
  return os;
}


int str_compare(const char* s1, const char* s2);



double getValue(const std::map<std::string, double>& check_structure_flow, const std::string& key);

double get_average_of_vector(std::vector<double> vec);

char* json2char(json input_j);
json char2json(const char* input_c_str);

std::vector<double> get_TaylorSeries_gates_orifice_freeflow(std::vector<double> params);

namespace fs = std::filesystem;

std::vector<std::string> getCsvFilesWithoutObs(const std::string& directory_path, const std::string& mask);

// Extract all integers
std::vector<int> extract_integers_regex(const std::string& str);


void diagnoseMatrixProblems(const std::vector<std::vector<double>>& A);
std::vector<std::vector<double>> fixColinearColumns(
  const std::vector<std::vector<double>>& A,
  double epsilon = 1e-6);
void  MutualCoherence(const std::vector<std::vector<double>>& matrix1);
void printMatrixInfo(const std::vector<std::vector<double>>& matrix);
void testSparseRecoveryFeasibility(const std::vector<std::vector<double>>& A);

// Find files in the current directory whose names contain "tar"
std::vector<fs::path> findFilesWithTarInName(const fs::path& directory,string& tar_str);


process_T get_solution(std::map<string, process_str_vec> solution, string var_name);
void addDemandProcess(
  std::map<std::string, process_h>& demandMap,
  const std::string& gateName,
  numeric_t demandFlow,
  time_t startTime,
  time_t endTime,
  time_t dt);


static std::vector<std::string> collect_boundary_times(const process_T& boundary_flow)
{
  std::vector<std::string> times;

  for (const auto& series_pair : boundary_flow) {
    const auto& one_series = series_pair.second;
    if (!one_series.empty()) {
      times.reserve(one_series.size());
      for (const auto& time_value_pair : one_series) {
        times.push_back(FormatTime(time_value_pair.first));
      }
      break;
    }
  }

  return times;
}

static int find_boundary_start_index(const process_T& boundary_flow, time_t start_t)
{
  std::vector<std::string> times = collect_boundary_times(boundary_flow);
  if (times.empty()) {
    return 0;
  }

  int best_index = 0;
  long long best_abs_dt = LLONG_MAX;

  for (int i = 0; i < static_cast<int>(times.size()); ++i) {
    const std::string& cur_time_str = times[i];
    if (cur_time_str.empty()) {
      continue;
    }

    time_t cur_t = StringToTime_t(cur_time_str);
    long long abs_dt = std::llabs(
      static_cast<long long>(cur_t) - static_cast<long long>(start_t)
    );

    if (cur_t >= start_t) {
      return i;
    }

    if (abs_dt < best_abs_dt) {
      best_abs_dt = abs_dt;
      best_index = i;
    }
  }

  return best_index;
}

static std::string get_boundary_time_string_by_index(
  const process_T& boundary_flow,
  int absolute_index)
{
  std::vector<std::string> times = collect_boundary_times(boundary_flow);
  if (times.empty()) {
    return "";
  }

  if (absolute_index < 0) {
    absolute_index = 0;
  }
  if (absolute_index >= static_cast<int>(times.size())) {
    absolute_index = static_cast<int>(times.size()) - 1;
  }

  return times[absolute_index];
}

static double get_boundary_value_by_index(
  const process_T& boundary_flow,
  const std::string& object_name,
  int absolute_index)
{
  auto series_it = boundary_flow.find(object_name);
  if (series_it == boundary_flow.end() || series_it->second.empty()) {
    return 0.0;
  }

  std::string time_key = get_boundary_time_string_by_index(boundary_flow, absolute_index);
  if (time_key.empty()) {
    return 0.0;
  }

  const auto& one_series = series_it->second;
  auto value_it = one_series.find(StringToTime_t(time_key));
  if (value_it == one_series.end()) {
    return 0.0;
  }

  return value_it->second;
}


static string get_obj_by_name_str(
  const process_T& boundary_flow,
  const std::string& object_name,
  int absolute_index)
{
  auto series_it = boundary_flow.find(object_name);
  if (series_it == boundary_flow.end() || series_it->second.empty()) {
    //for (const auto& pair : boundary_flow) {
    //  cout << "get_obj_by_name_str:" << pair.first<<", my input obj: "<<object_name << endl;
    //}

    return "null";
  }
  else
  {
    string return_str = "key: " + series_it->first + ", object_name_input: " + object_name ;
    return return_str;

  }
}


static void utf8_printf(const char* format, ...)
{
    if (format == nullptr)
    {
        return;
    }

    va_list args;
    va_start(args, format);

    va_list argsCopy;
    va_copy(argsCopy, args);

    const int requiredLength =
        std::vsnprintf(nullptr, 0, format, argsCopy);

    va_end(argsCopy);

    if (requiredLength < 0)
    {
        va_end(args);
        return;
    }

    std::vector<char> buffer(
        static_cast<size_t>(requiredLength) + 1,
        '\0');

    std::vsnprintf(
        buffer.data(),
        buffer.size(),
        format,
        args);

    va_end(args);

    const std::string utf8Text(
        buffer.data(),
        static_cast<size_t>(requiredLength));

#ifdef _WIN32

    HANDLE consoleHandle =
        GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD consoleMode = 0;

    const bool isConsole =
        consoleHandle != nullptr &&
        consoleHandle != INVALID_HANDLE_VALUE &&
        GetConsoleMode(consoleHandle, &consoleMode);

    if (isConsole)
    {
        // Convert UTF-8 to UTF-16 and write directly to the Windows console
        const int wideLength =
            MultiByteToWideChar(
                CP_UTF8,
                MB_ERR_INVALID_CHARS,
                utf8Text.data(),
                static_cast<int>(utf8Text.size()),
                nullptr,
                0);

        if (wideLength > 0)
        {
            std::wstring wideText(
                static_cast<size_t>(wideLength),
                L'\0');

            const int convertedLength =
                MultiByteToWideChar(
                    CP_UTF8,
                    MB_ERR_INVALID_CHARS,
                    utf8Text.data(),
                    static_cast<int>(utf8Text.size()),
                    wideText.data(),
                    wideLength);

            if (convertedLength > 0)
            {
                DWORD written = 0;

                WriteConsoleW(
                    consoleHandle,
                    wideText.data(),
                    static_cast<DWORD>(wideText.size()),
                    &written,
                    nullptr);

                return;
            }
        }
    }

    /*
     * The output target is not a console, for example:
     * program.exe > log.txt
     *
     * In this case, write UTF-8 bytes directly.
     */
    std::fwrite(
        utf8Text.data(),
        1,
        utf8Text.size(),
        stdout);

    std::fflush(stdout);

#else

    // Linux and macOS terminals typically use UTF-8
    std::fwrite(
        utf8Text.data(),
        1,
        utf8Text.size(),
        stdout);

    std::fflush(stdout);

#endif
}

  /*
 * Perform mathematical floor division on integers.
 *
 * C++ integer division truncates toward zero, so negative operands require special handling.
 */
static std::int64_t FloorDiv(
    std::int64_t value,
    std::int64_t divisor)
{
    if (divisor <= 0)
    {
        throw std::invalid_argument(
            "divisor 必须大于 0");
    }

    std::int64_t quotient = value / divisor;
    const std::int64_t remainder = value % divisor;

    if (remainder < 0)
    {
        --quotient;
    }

    return quotient;
}

/*
 * Perform mathematical ceiling division on integers.
 */
static std::int64_t CeilDiv(
    std::int64_t value,
    std::int64_t divisor)
{
    return -FloorDiv(-value, divisor);
}

void ConvertBoundaryFlowTime(
  process_T& boundary_flow,
  time_t start_time_t,
  double T,double DT);

void ConvertProcessT2Code(process_T& boundary_flow);

class WaterUnitMapper
{
public:
    bool Init(
        const std::string& config_file,
        const std::string& water_unit_file);

    std::string GetTarget(const std::string& code) const;
    std::string GetCode(
    const std::string& target) const;
private:
    std::unordered_map<std::string, std::string> code_to_unit_;
    std::unordered_map<std::string, std::string> unit_to_target_;
};

extern WaterUnitMapper mapper;
#endif FILEINTERACTION
