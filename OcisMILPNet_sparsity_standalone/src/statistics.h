
#ifndef STATISTIC
#define STATISTIC

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <chrono>
using namespace std;

template <typename T>
void nofindInvecAndPush(std::vector<T>& vec, T aim);

template <typename T>
bool find_vec(std::vector<T>& vec, T aim) ;

template <typename T>
int findid_vec(std::vector<T>& vec, T aim);
template <typename T>
int find_vec_id(std::vector<T>& vec, T aim);


template <typename T>
double liner_inter_2(T vec, T aim);

double HydrolicFunction(double Hu, double Hd, double dH);
double linearInterpolate(const std::map<double, double>& myMap, double key);




template <typename T>
void nofindInvecAndPush(std::vector<T>& vec, T aim) {
  if (find(vec.begin(), vec.end(), aim) != vec.end()) {

  }
  else
  {
    vec.push_back(aim);
  }
}
template <typename T>
bool find_vec(std::vector<T>& vec, T aim) {
  int isFind = 0;
  for (auto& nc : vec) {
    if (aim == nc) {
      isFind = 1;
      break;
    }
  }
  return isFind;
}

template <typename T>
int find_vec_id(std::vector<T>& notControl,T& k) {
  int isFind = -1;
  for (int i = 0; i < notControl.size(); i++) {
    if (k == notControl[i]) {
      return i;
    }
  }
  return isFind;
}


template <typename T>
int findid_vec(std::vector<T>& vec, T aim) {
  for (int i = 0; i < vec.size(); i++) {
    if (vec[i] == aim) {
      return i;
    }
  }
  return -1;
}
template <typename T>
double liner_inter_2(T vec, T aim) {
  return (vec + aim);
}


double GateFlow(std::map<std::string, double> params);



std::pair<std::string, double> getMaxValue(const std::map<std::string, double>& turnouts_nindex);



  double get_MinMaxNormalization(double& UQ,double & maxFlow) ;


  std::map<string, int> find_optimal_time_int(std::map<string, std::vector<double>>& obj_dist,double x_weight,double y_weight);
  std::map<string, int> find_optimal_time_int_low(std::map<string, std::vector<double>>& obj_dist,double x_weight,double y_weight);
   int find_optimal_time_id(std::vector<double>& value_vec,double w1,double w2);
   int find_optimal_time_id_low(std::vector<double>& value_vec,double w1,double w2);



#endif STATISTIC
