#ifndef myConstants
#define myConstants

const int NSAMP_MAX = 10;
const double TWO_PI = 6.28318531;
const double C_THETA = 2.2;
const double C_PHI = 4.6;
const double E_COR = 92.729970; // 92.695588;
const double Sector_dPhi = TWO_PI/48.;
const double Tile_dPhi = TWO_PI/12.;
const double Lcal_Z = 2500.;
const double E2Q = 0.0444;       // [pC/MeV]
const double QMAX= 99.0;          // 6[pC] 1250 MIPS
const double QMIN= 0.001;        // 0.001[pC] -> 0.25 MIPS
const double E_MIN = QMIN/E2Q;
const double E_MAX = QMAX/E2Q;
const double R_MOL = 15. ;
const double PHI_OFFSET[NSAMP_MAX] = { -0.26860, -0.08968, -0.05351, -0.02773, -0.01332, -0.00904, -.00674, -0.005429, -0.002742, -0.001082};
const Double_t Elo[NSAMP_MAX] = {0.0, 0.10, 0.20,  0.45, 0.9, 1.5, 2.0, 2.5, 5.0, 15.0};
const Double_t Eup[NSAMP_MAX] = {0.1, 0.25, 0.35,  0.65, 1.3, 1.9, 2.4, 3.5, 6.0, 18.0};
const Int_t  ebeam[NSAMP_MAX] = { 5, 15, 25, 50, 100, 150, 200, 250, 500, 1500};
#ifdef rot0_hh
//                                  0      1          2          3          4          5          6          7           8          9
#ifdef si500
const double a1[NSAMP_MAX] = {0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00};
const double w1[NSAMP_MAX] = {0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00, 0.000e+00};
const double a2[NSAMP_MAX] = {6.940e+00, 3.011e+00, 3.011e+00, 2.333e+00, 2.108e+00, 1.901e+00, 2.055e+00, 2.163e+00, 1.796e+00, 1.959e+00};
const double w2[NSAMP_MAX] = {6.170e+00, 3.431e+00, 3.431e+00, 2.832e+00, 2.344e+00, 2.116e+00, 2.299e+00, 2.304e+00, 1.946e+00, 2.070e+00};
#else
const Double_t a1[NSAMP_MAX] = { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 };
const Double_t w1[NSAMP_MAX] = { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 };
const double a2[NSAMP_MAX] = {8.675e+00, 3.614e+00, 3.614e+00, 2.840e+00, 2.235e+00, 2.244e+00, 2.061e+00, 2.008e+00, 2.050e+00, 2.083e+00};
const double w2[NSAMP_MAX] = {9.071e+00, 4.049e+00, 4.049e+00, 3.270e+00, 2.401e+00, 2.424e+00, 2.210e+00, 2.087e+00, 2.074e+00, 2.085e+00};
/*
const double a1[NSAMP_MAX] = {9.345e-01, 7.222e-01, 6.268e-01, 6.890e-01, 6.103e-01, 6.038e-01, 6.373e-01, 6.363e-01, 7.673e-01};
const double w1[NSAMP_MAX] = {3.829e-01, 5.715e-01, 5.789e-01, 5.686e-01, 5.878e-01, 5.676e-01, 5.843e-01, 5.939e-01, 5.678e-01};
const double a2[NSAMP_MAX] = {2.941e+01, 1.540e+00, 1.350e+00, 1.234e+00, 1.176e+00, 1.151e+00, 1.198e+00, 1.286e+00, 1.913e+00};
const double w2[NSAMP_MAX] = {3.820e+02, 6.055e+00, 3.995e+00, 4.283e+00, 3.424e+00, 3.401e+00, 3.758e+00, 3.990e+00, 8.866e+00};
*/
#endif
//
#endif

#ifdef rot3_hh
//                                0      1      2      3      4      5      6      7      8       9
const Double_t a1[NSAMP_MAX] = { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 , 0.000 };
const Double_t w1[NSAMP_MAX] = { 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000, 0.000 , 0.000 };
const Double_t a2[NSAMP_MAX] = {3.721e+00, 1.204e+00, 1.011e+00, 8.915e-01, 8.457e-01, 9.327e-01, 8.957e-01, 8.351e-01, 1.111e+00};
const Double_t w2[NSAMP_MAX] = {1.358e+01, 3.921e+00, 3.255e+00, 2.803e+00, 2.653e+00, 2.831e+00, 2.774e+00, 2.534e+00, 3.198e+00};
//const double a1[NSAMP_MAX] = {2.374e-01, 9.173e-02, 9.460e-02, 9.165e-02, 5.543e-02, 1.156e-01, 9.141e-02, 5.151e-02, 2.231e-01};
//const double w1[NSAMP_MAX] = {7.010e-02, 8.067e-02, 1.283e-01, 8.460e-02, 7.580e-02, 1.375e-01, 1.106e-01, 8.743e-02, 3.190e-01};
//const double a2[NSAMP_MAX] = {1.156e+02, 6.710e-01, 6.655e-01, 4.289e-01, 5.469e-01, 5.370e-01, 5.148e-01, 5.820e-01, 1.540e+00};
//const double w2[NSAMP_MAX] = {3.201e+03, 3.185e+00, 3.200e+00, 1.898e+00, 2.074e+00, 2.628e+00, 2.273e+00, 2.096e+00, 1.305e+01};
#endif

#endif
