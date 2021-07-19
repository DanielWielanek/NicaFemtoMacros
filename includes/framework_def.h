/*
 * exp_includes.h
 *
 *  Created on: 31 mar 2020
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */


#define MPD


NicaQACoreManager* GetCoreManager()
{
#ifdef CBM
  return new CbmQACoreManager();
#else
#ifdef MPD
  return new MpdQACoreManager();
#else
#ifdef BMN

#endif
#endif
  return nullptr;
#endif
}
