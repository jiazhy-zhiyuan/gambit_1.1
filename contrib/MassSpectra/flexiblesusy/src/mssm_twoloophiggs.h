
#ifndef MSSM_TWOLOOPHIGGS_H
#define MSSM_TWOLOOPHIGGS_H

#ifdef __cplusplus
extern "C" {
#endif

/** \file mssm_twoloophiggs.h
   - Project:     SOFTSUSY 
   - Author:      Ben Allanach 
   - Manual:      hep-ph/0104145, Comp. Phys. Comm. 143 (2002) 305 
   - Webpage:     http://hepforge.cedar.ac.uk/softsusy/

   \brief Contains two-loop routines from Slavich et al.
*/

/**  Two-loop O(a_t a_s) corrections to the Higgs tadpoles. 
     Written by P. Slavich (e-mail: slavich@mppmu.mpg.de). 
     Based on A. Dedes and P. Slavich, hep-ph/0212132. 

     Last update:  12/12/2002. 

     I/O PARAMETERS: 
     t = m_top^2, g = m_gluino^2, T1 = m_stop1^2, T2 = m_stop2^2, 
     st = sin(theta_stop), ct = cos(theta_stop), q = Q^2 (ren. scale), 
     mu = Higgs mixing parameter, tanb = tan(beta), vv = v^2, 
     gs = strong coupling constant
     Si = 1/vi*dVeff/dvi = 2-loop corrections to the Higgs tadpoles. 

     Notice: we assume that the 1-loop part is computed in terms of 
             running (DRbar) parameters, evaluated at the scale Q. */
int ewsb2loop_(double *t, double *g, double * t1, double *t2, 
			  double *st, double *ct, double *q, double *mu, 
			  double *tanb, double *vv, double *gs, 
			  double *s1, double *s2); 


/**  Two-loop O(a_t a_s) corrections to the CP-even Higgs mass matrix. 
     Routine written by P. Slavich (e-mail: slavich@pd.infn.it). 
     Based on G. Degrassi, P. Slavich and F. Zwirner, 
     Nucl. Phys. B611 (2001) 403 [hep-ph/0105096]. 

     Last update:  13/12/2001. 

     I/O PARAMETERS: 
     t = m_top^2, g = m_gluino^2, T1 = m_stop1^2, T2 = m_stop2^2, 
     st = sin(theta_stop), ct = cos(theta_stop), q = Q^2 (ren. scale), 
     mu = Higgs mixing parameter, tanb = tan(beta), v2 = v^2, 
     OS = renormalization scheme for 1-loop (0 = DRbar, 1 = On-Shell), 
     Sij = 2-loop corrections to the CP-even Higgs mass matrix elements. */
int dszhiggs_(double * t, double * g, double * T1, double * T2, 
			  double * st, double * ct, double * q, double * mu, 
			  double * tanb, double *v2, double * gs, int * OS, 
			  double * S11, double * S22, double * S12);

/**  Two-loop O(a_t a_s) corrections to the CP-odd Higgs mass in the 
     DRbar scheme. Written by P. Slavich (e-mail: slavich@pd.infn.it). 
     Based on G. Degrassi, P. Slavich and F. Zwirner, 
     Nucl. Phys. B611 (2001) 403 [hep-ph/0105096]. 
     Last update:  13/12/2001. 

     I/O PARAMETERS: 
     t = m_top^2, g = m_gluino^2, T1 = m_stop1^2, T2 = m_stop2^2, 
     st = sin(theta_stop), ct = cos(theta_stop), q = Q^2 (ren. scale), 
     mu = Higgs mixing parameter, tanb = tan(beta), v2 = v^2, 
     DMA = 2-loop corrections to the CP-odd Higgs mass. */
int dszodd_(double *rmtsq, double *mgsq, double *mst1sq, 
			double *mst2sq, double *sxt, double *cxt, 
			double *scalesq, double *amu, double *tbeta, 
			double *vev2, double *gstrong, double *p2s); 

/**  
     Two-loop O(a_t^2 + at ab + ab^2) corrections to the CP-even Higgs masses 
     Written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
     Based on A. Dedes, G. Degrassi and P. Slavich, hep-ph/0305127.

     Last update:  13/05/2003.

     I/O PARAMETERS:
     t = m_top^2, b = m_bot^2, A0 = m_A^2, T1 = m_stop1^2, T2 = m_stop2^2,
     B1 = m_sbot1^2, B2 = m_sbot2^2, st = sin(theta_stop), 
     ct = cos(theta_stop), sb = sin(theta_sbot), cb = cos(theta_sbot),
     q = Q^2 (ren. scale), mu = Higgs mixing parameter, tanb = tan(beta), 
     vv = v^2,
     Sij = 2-loop corrections to the CP-even Higgs mass matrix elements,

     Notice: we assume that the 1-loop part is computed in terms of 
             running (DRbar) parameters, evaluated at the scale Q. The 
             parameters in the bottom/sbottom sector should be computed 
             in terms of the "resummed" bottom Yukawa coupling.

*/
int ddshiggs_(double * t, double * b, double * A0, double * T1, 
			 double * T2, double * B1, double * B2, double * st, 
			 double * ct, double * sb, double * cb, double * q, 
			 double * mu, double * tanb, double * vv,  
			 double * S11, double * S12, double * S22);

/**  
     Two-loop O(a_t^2 + at ab + ab^2) corrections to the CP-odd Higgs mass
     Written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
     Based on A. Dedes, G. Degrassi and P. Slavich, hep-ph/0305127.

     Last update:  13/05/2003.

     I/O PARAMETERS:
     t = m_top^2, b = m_bot^2, A0 = m_A^2, T1 = m_stop1^2, T2 = m_stop2^2,
     B1 = m_sbot1^2, B2 = m_sbot2^2, st = sin(theta_stop), 
     ct = cos(theta_stop), sb = sin(theta_sbot), cb = cos(theta_sbot),
     q = Q^2 (ren. scale), mu = Higgs mixing parameter, tanb = tan(beta), 
     vv = v^2,
     DMA = 2-loop corrections to the CP-odd Higgs mass.

     Notice: we assume that the 1-loop part is computed in terms of 
             running (DRbar) parameters, evaluated at the scale Q. The 
             parameters in the bottom/sbottom sector should be computed 
             in term of the "resummed" bottom Yukawa coupling.

*/
int ddsodd_(double * t, double * b, double * A0, double * T1, 
			 double * T2, double * B1, double * B2, double * st, 
			 double * ct, double * sb, double * cb, double * q, 
			 double * mu, double * tanb, double * vv,  
			 double * dMA);

/**  
     Two-loop O(a_t^2 + at ab + ab^2) corrections to the 
     minimization conditions of the effective potential. 
     Written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
     Based on A. Dedes, G. Degrassi and P. Slavich, hep-ph/0305127.

     Last update:  13/05/2003.

     I/O PARAMETERS:
     t = m_top^2, b = m_bot^2, A0 = m_A^2, T1 = m_stop1^2, T2 = m_stop2^2,
     B1 = m_sbot1^2, B2 = m_sbot2^2, st = sin(theta_stop), 
     ct = cos(theta_stop), sb = sin(theta_sbot), cb = cos(theta_sbot),
     q = Q^2 (ren. scale), mu = Higgs mixing parameter, tanb = tan(beta), 
     vv = v^2,
     Si = 1/vi*dVeff/dvi = 2-loop corrections to the Higgs tadpoles,

     Notice: we assume that the 1-loop part is computed in terms of 
             running (DRbar) parameters, evaluated at the scale Q. The 
             parameters in the bottom/sbottom sector should be computed 
             in term of the "resummed" bottom Yukawa coupling.

*/
int ddstad_(double * t, double * b, double * A0, double * T1, 
			 double * T2, double * B1, double * B2, double * st, 
			 double * ct, double * sb, double * cb, double * q, 
			 double * mu, double * tanb, double * vv,  
			 double * s1, double * s2);


/**
   Two-loop O(a_tau^2) corrections to the CP-odd Higgs mass in the
   DRbar scheme. Written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
   Based on A. Brignole, G. Degrassi, P. Slavich and F. Zwirner, 
   hep-ph/0112177 with appropriate substitutions for top -> tau.
   
   Last update:  19/06/2003.
   
   I/O PARAMETERS:
   t = m_tau^2, A0 = m_A^2, BL = m_snu^2, T1 = m_stau1^2, T2 = m_stau2^2,
   st = sin(theta_stau), ct = cos(theta_stau), q = Q^2 (ren. scale),
   mu = Higgs mixing parameter, tb = tan(beta), v2 = v^2, 
   DMA = 2-loop corrections to the CP-odd Higgs mass.
 */
int tausqodd_(double * t, double * A0, double * BL, double * T1, 
			 double * T2, double * st, double * ct, double * q, 
			 double * mu, double * tanb, double * vv,  
			 double * dMA);

/**
   Two-loop O(a_tau^2) corrections to the CP-even Higgs mass matrix. 
   Routine written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
   Based on A. Brignole, G. Degrassi, P. Slavich and F. Zwirner, 
   hep-ph/0112177 with appropriate substitutions for top -> tau.

   Last update:  19/06/2003.
   
   I/O PARAMETERS:
   t = m_tau^2, A0 = m_A^2, BL = m_snu^2, T1 = m_stau1^2, T2 = m_stau2^2,
   st = sin(theta_stau), ct = cos(theta_stau), q = Q^2 (ren. scale),
   mu = Higgs mixing parameter, tb = tan(beta), v2 = v^2, 
   OS = renormalization scheme for 1-loop (0 = DRbar, 1 = On-Shell),
   Sij = 2-loop corrections to the CP-even Higgs mass matrix elements.
*/
int tausqhiggs_(double * t, double * A0, double * BL, double * T1, 
			   double * T2, double * st, double * ct, double * q,
			   double * mu, double * tanb, double * vv, int * OS,
			   double * S11, double * S22, double * S12);


/**
     Two-loop O(a_t^2) corrections to the Higgs tadpoles. 
     Written by P. Slavich (e-mail: slavich@mppmu.mpg.de).
     Based on A. Dedes and P. Slavich, hep-ph/0212132 
     with appropriate substitutions for top -> tau.

     Last update:  19/06/2003.
     
     I/O PARAMETERS:
     t = m_tau^2, A0 = m_A^2, BL = m_snu^2, T1 = m_stau1^2, T2 = m_stau2^2,
     st = sin(theta_stau), ct = cos(theta_stau), q = Q^2 (ren. scale),
     mu = Higgs mixing parameter, tb = tan(beta), vv = v^2,
     Si = 1/vi*dVeff/dvi = 2-loop corrections to the Higgs tadpoles.

     Notice: we assume that the 1-loop part is computed in terms of 
     running (DRbar) parameters, evaluated at the scale Q.
*/
int tausqtad_(double * t, double * A0, double * BL, double * T1, 
			 double * T2, double * st, double * ct, double * q, 
			 double * mu, double * tanb, double * vv,  
			 double * s1, double * s2);

#define tadpole_higgs_2loop_at_as_mssm                ewsb2loop_
#define tadpole_higgs_2loop_ab_as_mssm                ewsb2loop_
#define tadpole_higgs_2loop_at_at_mssm                ddstad_
#define tadpole_higgs_2loop_atau_atau_mssm            tausqtad_

#define self_energy_higgs_2loop_at_as_mssm            dszhiggs_
#define self_energy_higgs_2loop_ab_as_mssm            dszhiggs_
#define self_energy_higgs_2loop_at_at_mssm            ddshiggs_
#define self_energy_higgs_2loop_atau_atau_mssm        tausqhiggs_

#define self_energy_pseudoscalar_2loop_at_as_mssm     dszodd_
#define self_energy_pseudoscalar_2loop_ab_as_mssm     dszodd_
#define self_energy_pseudoscalar_2loop_at_at_mssm     ddsodd_
#define self_energy_pseudoscalar_2loop_atau_atau_mssm tausqodd_

#ifdef __cplusplus
}
#endif

#endif
