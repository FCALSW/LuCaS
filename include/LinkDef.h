
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ struct Hit_t+;
#pragma link C++ struct Track_t+;
#pragma link C++ class std::vector<Hit_t>+;
#pragma link C++ class std::vector<Track_t>+;

#pragma link(1,2) C++ global gROOT;
#pragma link(1,2) C++ global gEnv;

#pragma link(1,2) C++ enum EMessageTypes;
#endif
