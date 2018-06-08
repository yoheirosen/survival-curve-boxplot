#include <fstream>
#include "banddepth.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  string json_out_path = "data.json";
  if(argc != 2) { 
    throw runtime_error("no file input provided"); 
  }
  
  string tsv_path = argv[1];
  
  vector<string> names;
  vector<vector<size_t> > times;
  vector<vector<double> > values;
  
  ifstream tsv_in;
  tsv_in.open(tsv_path, ios::in);
  
  if(!tsv_in.is_open()) {
    throw runtime_error("invalid input file");
  }
  
  size_t t_max;
  double v_max;
  tsv_in >> t_max;
  tsv_in >> v_max;
  cerr << t_max << " " << v_max << endl;
  
  bool get_name = true;
  bool get_time = true;
  string name_datum;
  size_t time_datum;
  double value_datum;
  while((get_name && tsv_in >> name_datum) ||
        (get_time && tsv_in >> time_datum) ||
        tsv_in >> value_datum) {
    if(get_name) {
      cerr << "name ";
      names.push_back(name_datum);
      get_name = false;
      times.push_back(vector<size_t>(0));
      values.push_back(vector<double>(0));
    } else {
      if(get_time) {
        cerr << "time ";
        times.back().push_back(time_datum);
        get_time = false;
      } else {
        cerr << "value ";
        values.back().push_back(value_datum);
        get_time = true;
      }
    }
    if(tsv_in.peek() == '\n') {
      get_name = true;
      get_time = true;
      cerr << endl;
      if(times.back().size() != values.back().size()) {
        throw runtime_error("missing value for time-value pair in input");
      }
    }
  }
  cerr << endl;
    
  tsv_in.close();
  
  ofstream json_out;
  json_out.open(json_out_path, ios::out);
  generate_statistics(names, times, values, json_out);
  json_out.close();
}