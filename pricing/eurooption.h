
// http://xerces.apache.org/xerces-c/


namespace Pricing {

  class Time {

    double time_;
    
  public:
    Time(double time)
      : time_(time) {}
    Time(const Time& time)
      : time_(time.time_) {}
    operator double() const { return time_; }  
  };
  
  class OptionData {

  public:
    typedef enum { CALL = 0, PUT = 1 } CallPut;

  private:
    double strike_;
    Time maturity_;
    CallPut cp_;

  public:
    OptionData(double strike, const Time& maturity, CallPut cp)
      : strike_(strike), maturity_(maturity), cp_(cp)
    {}
    double strike() const { return strike_; }
    const Time& maturity() const { return maturity_; }
    CallPut callput() const { return cp_; }
  };

  class YieldCurve {
    double rate_;
  public:
    YieldCurve(double rate)
      : rate_(rate)
    { }
    double rateBtw(const Time& startTime, const Time& endTime) const
    {
      return rate_;
    }
    double discountFactor(const Time& startTime, const Time& endTime, double* = 0) const;
  };

  class Volatility {
    double vol_;
  public:
    Volatility(double vol)
      : vol_(vol)
    { }
    double getVolatility(/* various params */) const
    {
      return vol_;
    }
  };

  class Pricer {
    // private data mostly cache data
  public:
    double eurocall(const OptionData& option,
		    const YieldCurve& yc,
		    const Volatility& vol,
		    const Time& now,
		    double underlyingPrice);
  };

  double Normal(double);
		
} // namespace
