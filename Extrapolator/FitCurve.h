#pragma once
#include <vector>
#include <utility>

typedef std::vector<double> vector;
constexpr  int TRESSHOLD_FOR_INITIAL_VALS = 0.2;

class Extrapolator{

  Extrapolator() = default;

  /*
    Curve parameters
    m_A and m_B - axis of ellipse
    m_C - translation factor of ellipse center
    m_Phi - initial phase in parametric ellipse curve (t,phi)

                            ********************
                    ********         ^ |         ********
              ******                 B |                 ******
        ******                         |                       ******
      **                               |                             **
    **                                 |                               **
  **                                   |                                 **
  **                                   |                                 **
  **                                   |                                 **
  **   <                               | <A>                         >   **
    ** --------------------------------|-------------------------------  **
      **                               |                             **
        ******                         |                       ******
              ******                   |                 ******
                    ********         ^ |        ********
                            ********************
    */

  double m_A;
  double m_B;
  double m_C;
  double m_freq;

  /*
    Y

    |              ****                < maxVal
    |            **    *
    |          **        *
    |         *            *  *
    |                       *          < minVal
    ----------------------------------    X
                    ^       ^
              maxTime       minTime
  */
  // data of measurements
  vector m_X;
  vector m_Y;

  // max and min values of data
  double m_maxVal;
  double m_minVal;

  double m_maxTime; // time when max value of curve will be reached
  double m_minTime; // time when min value of curve will be reached

  //------------------ FUNCTIONS-------------------------
  //function to create curve
  double Ellipse(double t);

  // find bound for solver a first guess of values
  void DefineBoundsAndGuessedValues();
  void FillBounds();

  // find error of current guess
  double FindError();

  // initialize extrapolator
  void Initialize();

  // utils to create bounds, taking in account sign of _Val
  // bound: (Lower, Upper)
  // positive : (0.8 * val, 1.2 * val)
  // negative : (1.2 * val, 0.8 * val)
  double LowerWithSign(double _val);
  double UpperWithSign(double _val);

  // Main function to fit data
  void MakeFit();
  // function to call dlib library
  void FitCurve();

  // create arrays of extrapolated data
  std::pair<vector,vector> CalculateExtrapol(double _maxTime);
  std::pair<vector,vector> CalculateExtrapol();


  // update time of min and max vals of extrapolated data
  void UpdateTime();

public:
  // in case only measurements are known
  Extrapolator(vector _vecX, vector _vecY): m_X(_vecX), m_Y(_vecY){Initialize();}

  // in case then min and max theoretical data available
  Extrapolator(vector _vecX, vector _vecY, double _max,double _min):
    m_X(_vecX), m_Y(_vecY), m_maxVal(_max), m_minVal(_min){Initialize();}

  // initial freq also is known
  Extrapolator(vector _vecX,vector _vecY, double _max,double _min, double _freq):
    m_X(_vecX), m_Y(_vecY), m_maxVal(_max), m_minVal(_min), m_freq(_freq){Initialize();}

  double GetMaxTime(){return m_maxTime;};
  double GetMinTime(){return m_minTime;};

  std::pair<vector,vector> AddDataAndGetUpdate(vector _x, vector _y, double t_max = -1);
  std::pair<vector,vector> AddDataAndGetUpdate(double x, double y, double t_max = -1);

};
