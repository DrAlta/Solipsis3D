/**
* \file 3dcWrapperConversion.h
* \brief This class gives conversion tools for the 3DCWrapper
*/

#ifndef __3dcWrapperConversion_h__
#define __3dcWrapperConversion_h__

/**
* The 3dcWrapperConversion class gives conversion tools for the 3DCWrapper
* @class _3dcWrapperConversion
* @author Rozenn Bouville Berthelot
* @date 27/02/2012
* @version 0.0
*/

#include <string>
#include <vector>
#include<math.h>

class _3dcWrapperConversion {


public:


	static std::vector<float> axis_angleToQuaternion ( std::vector<float> aaRot1 ){

		std::vector<float> res(4,0.0);
		res.at(0) = aaRot1.at(0) *sin(aaRot1.at(3)/2);
		res.at(1) = aaRot1.at(1)*sin(aaRot1.at(3)/2);
		res.at(2) = aaRot1.at(2)*sin(aaRot1.at(3)/2);
		res.at(3) = cos(aaRot1.at(3)/2);
		return res;

	}

	/*static void quaternionToAxis_angle (float qRot1 [],  float aaRot1[]){

		aaRot1[3] = acos(qRot1[3])*2;
		aaRot1[0] = qRot1[0];
		aaRot1[1] = qRot1[1];
		aaRot1[2] = qRot1[2];

		float norm = sqrt(aaRot1[0]*aaRot1[0]+aaRot1[1]*aaRot1[1]+aaRot1[2]*aaRot1[2]);
		if (norm > 0.0005){
			aaRot1[0] /= norm;
			aaRot1[1] /= norm;
			aaRot1[2] /= norm;
		}
	}*/


};

#endif // __3dcWrapperConversion_h__
