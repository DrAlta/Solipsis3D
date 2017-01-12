// FriendUserDB.h

#pragma once

#include <utility>
#include <list>
#include <string>
#include <map>

using namespace std;

typedef pair<string, string> String_Pair;

namespace Solipsis {
	/** Class managing storage of the friends of a user relative to the social networks he is subscribing
	*/

	typedef enum SNType {
		SNFacebook = 0,		// Facebook relative element
		SNTwitter,		// Twitter relative element
		SNOpensocial,	// OpenSocial relative element
		SNOther			// Other
	} SNTypeEnum;

	typedef list<String_Pair> FriendUserListElt;
	typedef map<SNTypeEnum, FriendUserListElt> FriendUserListEltMap;
	typedef map<string, FriendUserListEltMap> FriendUserDBMap;

	class FriendUserDB
	{
	public:

		/// Constructor
		FriendUserDB();
		// Destructor
		~FriendUserDB();

		// Add a user friend list of type SNid to a user uid 
		void addUserFriendList(const string& uid, SNTypeEnum snid, FriendUserListElt fULElt);
		
		// Get List of Friends 
		FriendUserListElt getUserFriendList(const string& uid, SNTypeEnum snid);

		// Check if a visitor (identified by vUid) is a friend of the local user lId in the Social Network snid
		bool matchFriend(const SNTypeEnum snid, const string& lId, const string& vUid);

	private:
		FriendUserDBMap mfriendUserDBMap;
		FriendUserListEltMap mLocalFriendUserListMap;	
		// Je stocke dans cette BD : 
		// * 1 table de correspondance AvatarID - SNid (FB,Twitter, OpenSocial)
		// * 1 table d'éléments composés de :
		//		- avatarId ou IdSolipsis du User
		//		- type/nom du SN i.e. FACEBOOK, TWITTER, OPENSOCIAL
		//		- liste des Ids dans le SN (converti en Uid/avatarID ?)
		// * 1 mécanisme de convertion de AVatarID -> SNId
		// * 1 mécanisme de convertion inverse SNid -> AvatarId

	};

	// Define a postfix increment operator for SNType.
	inline SNType operator++( SNType &rs, int ) {
		return rs = (SNType)(rs + 1);
	}
}
