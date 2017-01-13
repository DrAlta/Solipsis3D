/**
* \file ColladaWrapperTools.h
* \brief This class gives tools for the ColladaWrapper
*/

#ifndef __ColladaWrapperTools_h__
#define __ColladaWrapperTools_h__

/**
* The ColladaWrapperTools class gives tools for the ColladaWrapper
* @class ColladaWrapperTools
* @author Rozenn Bouville Berthelot
* @date 24/01/2012
* @version 0.0
*/

class ColladaWrapperTools {


public:

	static vector<float> stringToFloatArray ( string sValues ){

		vector<float> vValues;
		string buf;
		stringstream sStream(sValues);
		float fValue;
		while( sStream >> buf ){
			istringstream isStream(buf);
			if( !(isStream >> fValue).fail() ) vValues.push_back( fValue );
			else break;
		}
		return vValues;
	}

	

};

#endif // __ColladaWrapperTools_h__

