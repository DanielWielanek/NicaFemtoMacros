/*
 * exp_includes.h
 *
 *  Created on: 31 mar 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */


#define CBM


NicaQACoreManager* GetCoreManager()
{
#ifdef CBM
  return new CbmQACoreManager();
#else
#ifdef MPD

#else
#ifdef BMN

#endif
#endif
  return nullptr;
#endif
}
