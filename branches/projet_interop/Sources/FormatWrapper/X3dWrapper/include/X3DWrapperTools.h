/**
* \file X3DWrapperTools.h
* \brief This class gives tools for the X3DWrapper
*/

#ifndef __X3DWrapperTools_h__
#define __X3DWrapperTools_h__

/**
* The X3DWrapperTools class gives tools for the X3DWrapper
* @class X3DWrapperTools
* @author Rozenn Bouville Berthelot
* @date 19/08/2011
* @version 0.0
*/

class X3DWrapperTools {


public:

	static void addTranslations( float trans1[], float trans2[], float res[] ){
		
		res[0] = trans1[0]+trans2[0];
		res[1] = trans1[1]+trans2[1];
		res[2] = trans1[2]+trans2[2];
	}

	static void subTranslations( float trans1[], float trans2[], float res[] ){
		
		res[0] = trans1[0]-trans2[0];
		res[1] = trans1[1]-trans2[1];
		res[2] = trans1[2]-trans2[2];
	}
	

	static void  addScales( float scale1[], float scale2[], float res[] ){
		
		res[0] = scale1[0]*scale2[0];
		res[1] = scale1[1]*scale2[1];
		res[2] = scale1[2]*scale2[2];
	}

	static void  addRotations( float rot1[], float rot2[], float res[] ){

		float qRot1 [4], qRot2 [4], qRes[4];
		axis_angleToQuaternion(rot1,qRot1);
		axis_angleToQuaternion(rot2,qRot2);

		qRes[0] = qRot1[0] + qRot2[0];
		qRes[1] = qRot1[1] + qRot2[1];
		qRes[2] = qRot1[2] + qRot2[2];
		qRes[3] = qRot1[3] + qRot2[3];
		
		quaternionToAxis_angle(qRes,res);

	}

	static void  subRotations( float rot1[], float rot2[], float res[] ){

		float qRot1 [4], qRot2 [4], qRes[4];
		axis_angleToQuaternion(rot1,qRot1);
		axis_angleToQuaternion(rot2,qRot2);

		qRes[0] = qRot1[0] - qRot2[0];
		qRes[1] = qRot1[1] - qRot2[1];
		qRes[2] = qRot1[2] - qRot2[2];
		qRes[3] = qRot1[3] - qRot2[3];
		
		quaternionToAxis_angle(qRes,res);

	}

	static void axis_angleToQuaternion ( float aaRot1[],  float qRot1 []){

		qRot1[0] = aaRot1[0]*sin(aaRot1[3]/2);
		qRot1[1] = aaRot1[1]*sin(aaRot1[3]/2);
		qRot1[2] = aaRot1[2]*sin(aaRot1[3]/2);
		qRot1[3] = cos(aaRot1[3]/2);

	}

	static void quaternionToAxis_angle (float qRot1 [],  float aaRot1[]){

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
	}


};

#endif // __X3DWrapperTools_h__
