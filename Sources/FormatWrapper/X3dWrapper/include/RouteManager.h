/**
* \file RouteManager.h
* \brief A class to manage an X3D file route
*/

#ifndef _ROUTEMANAGER_H
#define _ROUTEMANAGER_H

#include <string>
#include <vector>

using namespace std;

struct WRoute {
	string fromNodeName;
	string fromFieldName;
	string toNodeName;
	string toFieldName;
};

class RouteManager{

public:

	void addRoute(string fromNodeName, string fromFieldName, string toNodeName, string toFieldName){
			WRoute route;
			route.fromNodeName = fromNodeName;
			route.fromFieldName = fromFieldName;
			route.toNodeName = toNodeName;
			route.toFieldName = toFieldName;
			rMap.insert( RouteMap::value_type(fromNodeName,route) );
	}

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