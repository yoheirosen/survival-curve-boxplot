// JUST A SKETCH
// THIS IS GOING TO BE FULL OF TYPOS/INCORRECT SYNTAX

// Please note that this is not a conventional curve boxplot; it is designed for time series data with uncertain (or even missing) data at sparse timepoints

template<struct T>
class interpolation {
public:
  virtual double operator(double t_l, double t_r, T y_l, T y_r, double t) const;
};

template<struct T = double>
class linearInterpolation : public interpolation {
public:
  virtual double operator(double t_l, double t_r, T y_l, T y_r, double t) const {
    double d = t - t_1 / t_2 - t_1;
    return d * y_2 + (1 - d) * y_1;
  }
};

template<struct T  = double>
class curve {
private:
  const interpolation<T>* interpolation_function;
public:
  const vector<double> times;
  const vector<T> values;

  curve(vector<double> times, vector<T> values, const interpolation<T>* interpolation_function);
  T operator() {double t} const;
  size_t size() const { return times.size(); }  
};

template<struct T = double>
class distribution {
public:
  virtual T random() const;
  virtual T mean() const;
  virtual T median() const;
  virtual double percentile(T value) const;
  virtual T value_at_percentile(double percentile) const;
};

class univariateNormal: public distribution<double> {
private:
  double mean;
  double standard_deviation;
public:
  univariateNormal(double mean, double standard_deviation);
};

template<struct T = double>
class observationSet {
public:
  const vector<double> times;
  const vector<T> values;
  const vector<distribution<T> > distributions;
  observationSet(vector<double> times, vector<T> values, vector<distribution<T> > distributions);
  size_t size() const { return times.size(); }
};

template<struct T = double, int k = 3>
class curveSet {
private:
  vector<curve> curves;
public:
  curveSet(const vector<observationSet<T> >& observations, const interpolation<T>* interpolation_function);
  bool contained(const curve& f) const;
  bool approx_contained(const curve& f, double wiggle_room) const;
};

template<struct T = double, int k> 
curveSet::curveSet(const vector<observationSet<T> >& observations, const interpolation<T>* interpolation_function) {
  for(size_t i = 0; i < k; i++) {
    size_t which_patient = myRandom::uniform_size_t(observations.size());
    vector<T> sampled_values;
    for(size_t j = 0; j < observations[which_patient].size(); j++) {
      sampled_values.push_back(observations[which_patient].distributions[j].random();
    }
    curves.push_back(observations[which_patient].times, values, interpolation_function);
  }
}
