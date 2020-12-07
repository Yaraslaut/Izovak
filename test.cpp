#include "Extrapolator/FitCurve.h"
#include <spdlog/spdlog.h>
#include <fstream>




double Ellipse(double t, double A, double B, double C, double f)
 {
   auto ellipse = std::sqrt( std::pow(A*std::cos(f*t),2) + std::pow(B*std::sin(f*t),2));
   return ellipse + C;
 };

int main(){
#ifdef USE_SPDLOG
  spdlog::set_level(spdlog::level::debug); 
#endif

  const int N = 100;
  std::vector<double> X_vec {std::vector<double>(N)};
  std::vector<double> Y_vec {std::vector<double>(N)};
  double dt = 0.1;
  for (int i = 0 ; i < N; ++i)
    {
      X_vec[i] = i*dt;
      Y_vec[i] = Ellipse(i*dt,-4,8,0.1,0.01);
    }
  //Extrapolator extr {X_vec,Y_vec};
  //extr.FitCurve();


  std::vector<double> X_data;
  std::vector<double> Y_data;
  std::string line;

  std::ifstream file;
  file.open("/media/yaraslau/Data/Izovak/Extrapolator/data_first_part.txt",std::ifstream::in);
  if (file.is_open())//Если открытие файла прошло успешно
    {
      while (getline(file, line))
        {
          spdlog::debug("{}", line);
          std::string::size_type sz;     // alias of size_t
          X_data.push_back(std::stod(line,&sz));
          Y_data.push_back(std::stod(line.substr(sz)));
        }
    }
  file.close();
  std::vector<double> X_part;
  std::vector<double> Y_part;

  for(int i = 0; i < 50; ++i)
    {
      X_part.push_back(X_data[i]);
      Y_part.push_back(Y_data[i]);
    }

  double maxVal = 33.9;
  double minVal = 10.7;
  double freq = 0.0126;

  Extrapolator Test{X_part,Y_part,maxVal,minVal,freq};

  for(int i = 50; i < X_data.size(); ++i)
    {
      Test.AddDataAndGetUpdate(X_data[i],Y_data[i],300.);
    }

  return 0;
}
