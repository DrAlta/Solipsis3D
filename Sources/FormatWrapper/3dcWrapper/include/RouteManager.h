/**
* \file RouteManager.h
* \brief A class to manage an 3dc file route
*/

#ifndef _ROUTEMANAGER_H
#define _ROUTEMANAGER_H

#include <string>
#include <vector>

using namespace std;

struct WRoute {
	string fromFileName;
	string fromNodeName;
	string fromFieldName;
	string toFileName;
	string toNodeName;
	string toFieldName;
	string convertor;
};

class RouteManager{

public:

	void addRoute(string fromFileName, string fromNodeName, string fromFieldName, string toFileName, string toNodeName, string toFieldName, string convertor = string() ){
			WRoute route;
			route.fromFileName = fromFileName;
			route.fromNodeName = fromNodeName;
			route.fromFieldName = fromFieldName;
			route.toFileName = toFileName;
			route.toNodeName = toNodeName;
			route.toFieldName = toFieldName;
			string sNodeName = fromFileName + "." + fromNodeName;
			route.convertor = convertor;
			rMap.insert( RouteMap::value_type(sNodeName,route) );
	}

	/**
	* @fn vector<WRoute> getRoutes( string nodeName )
	* @brief given an node return all route beginning from node
	* @param nodeName identifier of the node, ( ex: "File-1.Node-1" )
	* @return an array of Route all beginning from node <nodeName>
	*/
	vector<WRoute> getRoutes( string nodeName ){

		vector<WRoute> res;
		RouteMap::iterator it;
		pair<RouteMap::iterator,RouteMap::iterator> ret;
		ret = rMap.equal_range(nodeName);
		for(it=ret.first; it!=ret.second; it++){
			res.push_back( (*it).second );
		}
		return res;
	}

	/**
	* @fn vector<WRoute> getRoutes( string fileName, string nodeName )
	* @brief given an node return all route beginning from node
	* @param fileName identifier of the file, ( ex: "File-1" )
	* @param nodeName identifier of the node, ( ex: "Node-1" )
	* @return an array of Route all beginning from node <fileName>.<nodeName>
	*/
	vector<WRoute> getRoutes( string fileName, string nodeName ){

		vector<WRoute> res;
		RouteMap::iterator it;
		pair<RouteMap::iterator,RouteMap::iterator> ret;
		string sNodeName = fileName + "." + nodeName;
		ret = rMap.equal_range(sNodeName);
		for(it=ret.first; it!=ret.second; it++){
			res.push_back( (*it).second );
		}
		return res;
	}

	WRoute getRoutes( int n ){

		WRoute res = WRoute();
		if(n<rMap.size()){
			RouteMap::iterator it;
			int cpt = 0;
			for(it=rMap.begin(); it!=rMap.end(); it++){
				if(cpt==n) return (*it).second;
				cpt++;
			}
		}
		else return res;
	}

	bool needConversion( WRoute route ){
		return !route.convertor.empty();
	}



	int getNbRoutes(){
		return rMap.size();
	}

	


private:


	void initialize(){
		rMap = RouteMap();
	}

	void clean(){
		rMap.clear();
	}

	typedef multimap<string, WRoute> RouteMap;
	RouteMap rMap;



};

#endif // _ROUTEMANAGER_H