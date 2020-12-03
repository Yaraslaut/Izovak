#include "FitCurve.h"
#include <cmath>


std::pair<vector,vector> Extrapolator::AddDataAndGetUpdate(double x, double y, double t_max = -1)
{
  m_X.append(x);
  m_Y.append(y);
  MakeFit();
  UpdateTime();
  if(t_max > 0)
    {
      return CalculateExtrapol(t_max);
    }
  return CalculateExtrapol(t_max);
}


void Extrapolator::UpdateTime()
{
  m_minTime = 1;
  m_maxTime = 2;
}


void Extrapolator::MakeFit()
{
  // save old params
  double A = m_A;
  double B = m_B;
  double C = m_C;
  double freq = m_freq;
  double prevError = FindError();
  FitCurve(); // try to make better fitting

  if(FindError() > prevError) // unsuccessful fitting
    {
      m_A = A;
      m_B = B;
      m_C = C;
      m_freq = freq;
    }
}

void Extrapolator::FitCurve()
{
  return;
}


double Extrapolator::Ellipse(double t)
{
  auto ellipse = std::sqrt( std::pow(m_A*std::cos(m_freq*t),2) + std::pow(m_B*std::sin(m_freq*t),2));
  return ellipse + m_C;
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
      return;
    }
  m_C = (-m_maxVal + m_minVal)/2 ;
  m_A = m_maxVal - m_C;
  m_B = m_minVal - m_C;
}

void Extrapolator::FillBounds()
{
}


void Extrapolator::Initialize()
{
  DefineBoundsAndGuessedValues();
}

// mean square error between m_Y - real data and Ellipse(t) - calculated data
double Extrapolator::FindError()
{
  double Error = 0;
  for(int i =0; i < m_X.size(); ++i)
    {
      Error += std::pow(Ellipse(m_X[i]) - m_Y[i],2);
    }
  return Error / m_X.size();
}
