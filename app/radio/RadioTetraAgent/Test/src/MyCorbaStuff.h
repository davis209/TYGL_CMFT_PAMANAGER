/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/MyCorbaStuff.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains a few useful methods for using CORBA
  *
  */

#ifndef MY_CORBA_STUFF_H
#define MY_CORBA_STUFF_H

class MyCorbaStuff
{
public:

	/**
	 *  initialiseCORBA
	 *  
	 *  Calls initialisation and activate functions on CORBAUtil
	 */
	static void initialiseCORBA();

	/**
	 *  shutdownCORBA
	 *  
	 *  Shuts down and cleans up CORBA
	 */
	static void shutdownCORBA();
};

#endif // MY_CORBA_STUFF_H
