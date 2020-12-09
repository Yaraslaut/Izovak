#include "FitCurve.h"

#include <ctime>
#include <cmath>


// std::pair<X,Y>
std::pair<vector,vector> Extrapolator::AddDataAndGetUpdate(double _x, double _y, double t_max = -1)
{
  spdlog::info("------------------------------------");
  m_X.push_back(_x);
  m_Y.push_back(_y);
  MakeFit();
  UpdateTime();
  if(t_max > 0)
    {
      return CalculateExtrapol(t_max);
    }
  return CalculateExtrapol(std::max(m_maxTime,m_minTime)*1.5);
}

// std::pair<X,Y>
std::pair<vector,vector> Extrapolator::AddDataAndGetUpdate(vector _x, vector _y, double t_max = -1)
{
  spdlog::info("------------------------------------");

  for ( auto x : _x)
    {
      m_X.push_back(x);
    }
  for ( auto y : _y)
    {
      m_Y.push_back(y);
    }


  MakeFit();
  UpdateTime();
  if(t_max > 0)
    {
      return CalculateExtrapol(t_max);
    }
  return CalculateExtrapol(std::max(m_maxTime,m_minTime)*1.5);
}


void Extrapolator::UpdateTime()
{
  auto x_0 = m_X[0];
  auto dx = m_X[1] - m_X[0];

  double tempMaxVal = 0;
  double tempMinVal = 1e300;
  int i = 0;

  int foundInd = 0;
  while(foundInd != 2)
    {
      auto time = x_0 + i*dx;
      auto valAtX = Ellipse(time);
      if(valAtX > tempMaxVal)
        {
          tempMaxVal = valAtX;
          m_maxTime = time;
          if(tempMaxVal > Ellipse(time + dx))
            {
              foundInd ++;
            }
        }
      if(valAtX < tempMinVal)
        {
          tempMinVal = valAtX;
          m_minTime = time;

          if(tempMinVal < Ellipse(time + dx))
            {
              foundInd ++;
            }
        }
      i++;
    }
  spdlog::info("Max {} and min {} time ",m_maxTime,m_minTime);
}


void Extrapolator::MakeFit()
{
  spdlog::info("Making fit");

  // save old params
  double A = m_A;
  double B = m_B;
  double C = m_C;
  double freq = m_freq;
  double prevError = FindError();

  spdlog::debug("Previous error : {}",prevError);

  FitCurve(); // try to make better fitting

  if(FindError() > prevError) // unsuccessful fitting
    {
      spdlog::debug("Fitting was unsuccessful");
      m_A = A;
      m_B = B;
      m_C = C;
      m_freq = freq;
    }
  spdlog::debug("New Error is {}",FindError());
}

void Extrapolator::FitCurve()
{

  spdlog::info("Fitting curve");
  auto functionToCall = [&](double A,double B, double C,double f)
  {
    return FindError(A,B,C,f);
  };

  auto result  = dlib::find_min_global(functionToCall,
                                       lower_b,
                                       upper_b,
                                       std::chrono::milliseconds(1000));

  m_A = result.x(0);
  m_B = result.x(1);
  m_C = result.x(2);
  m_freq = result.x(3);
  spdlog::info("Value of Error {}",result.y);
  spdlog::info("Value of A: {}, B: {}, C: {}, f: {}",m_A, m_B, m_C, m_freq);
}


double Extrapolator::Ellipse(double t)
{
  auto ellipse = std::sqrt( std::pow(m_A*std::cos(m_freq*t),2) + std::pow(m_B*std::sin(m_freq*t),2));
  return ellipse + m_C;
};

double Extrapolator::Ellipse(double t, double A, double B, double C, double f)
{
  auto ellipse = std::sqrt( std::pow(A*std::cos(f*t),2) + std::pow(B*std::sin(f*t),2));
  return ellipse + C;
};


double Extrapolator::LowerWithSign(double _val)
{
  if (_val > 0)
    {
      return _val*(1-TRESSHOLD_FOR_INITIAL_VALS);
    }
  return _val*(1 + TRESSHOLD_FOR_INITIAL_VALS);
}

double Extrapolator::UpperWithSign(double _val)
{
  if (_val > 0)
    {
      return _val*(1+TRESSHOLD_FOR_INITIAL_VALS);
    }
  return _val*(1 - TRESSHOLD_FOR_INITIAL_VALS);
}


void Extrapolator::DefineBoundsAndGuessedValues()
{
  spdlog::info("Define bounds and guessed values of parameters");
  // A is bigger then B
  m_C = (-m_maxVal + m_minVal)/2 ;
  m_A = m_maxVal - m_C;
  m_B = m_minVal - m_C;
  FillBounds();
  double error = FindError();

  // B is bigger then A 
  m_A = m_minVal - m_C;
  m_B = m_maxVal - m_C;
  FillBounds();
  if ( FindError() < error)
    {
      spdlog::info("Resulted params A:{}, B:{}, C:{}, f:{}", m_A,m_B,m_C,m_freq);
      return;
    }
  m_C = (-m_maxVal + m_minVal)/2 ;
  m_A = m_maxVal - m_C;
  m_B = m_minVal - m_C;
  spdlog::info("Resulted params A:{}, B:{}, C:{}, f:{}", m_A,m_B,m_C,m_freq);

}

void Extrapolator::FillBounds()
{
  auto loA = LowerWithSign(m_A);
  auto upA = UpperWithSign(m_A);
  auto loB = LowerWithSign(m_B);
  auto upB = UpperWithSign(m_B);
  auto loC = LowerWithSign(m_C);
  auto upC = UpperWithSign(m_C);
  auto loF = LowerWithSign(m_freq);
  auto upF = UpperWithSign(m_freq);
  spdlog::debug("Resulted bound");
  spdlog::debug("A: {} and {}",upA,loA);
  spdlog::debug("B: {} and {}",upB,loB);
  spdlog::debug("C: {} and {}",upC,loC);
  spdlog::debug("F: {} and {}",upF,loF);
  lower_b = {loA,loB,loC,loF};
  upper_b = {upA,upB,upC,upF};
}

std::pair<vector,vector> Extrapolator::CalculateExtrapol(double _maxTime)
{
  double dx = m_X[1] - m_X[0];
  double x = m_X[0];
  vector xVec;
  vector yVec;
  while (x < _maxTime)
    {
      xVec.push_back(x);
      yVec.push_back(Ellipse(x));
      x += dx;
    }
  return std::pair<vector,vector> (xVec,yVec);
}

std::pair<vector,vector> Extrapolator::CalculateExtrapol()
{
  return CalculateExtrapol(m_maxTime*1.2);
}


void Extrapolator::Initialize()
{
  spdlog::info("Initialize of extrapolator");
  DefineBoundsAndGuessedValues();
  MakeFit();
}

// mean square error between m_Y - real data and Ellipse(t) - calculated data
double Extrapolator::FindError()
{
  auto error = FindError(m_A,m_B,m_C,m_freq);
  spdlog::debug("Error is {}", error);
  return  error;
}


double Extrapolator::FindError(double A, double B, double C, double f)
{
  double Error = 0;
  for(int i =0; i < m_X.size(); ++i)
    {
      Error += std::pow(Ellipse(m_X[i],A,B,C,f) - m_Y[i],2);
    }

  return Error / m_X.size();;
}
