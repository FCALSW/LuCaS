#ifndef BANNER_HH
#define BANNER_HH

const std::string LUCAS_VERSION ( "v2.0.0");

void print_lucas_banner(){
std::cout<<std::endl<<std::endl<<std::endl;
std::cout<<"****************************************************"<<std::endl;
std::cout<<"*                                                  *"<<std::endl;
std::cout<<"*           LumiCal Simulation Tool                *"<<std::endl;
std::cout<<"*               version 2.0.0          *"<<std::endl;
std::cout<<"*                                                  *"<<std::endl;
std::cout<<"****************************************************"<<std::endl;
}

inline std::string LucasVersion() { return LUCAS_VERSION; }

#endif
