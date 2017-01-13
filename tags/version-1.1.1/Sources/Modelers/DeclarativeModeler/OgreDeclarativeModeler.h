#ifndef _OGRE_DECLARATIVE_MODELER_H_
#define _OGRE_DECLARATIVE_MODELER_H_

#include <Ogre.h>

#include "DeclarativeModeler.h"

/**
 * \file DeclarativeModeler.h
 * \brief Le modeleur, il parse le texte, r�cup�re les acteurs et calcule leur propri�t�s
 * \date Octobre 2008
 */
class OgreDeclarativeModeler : public DeclarativeModeler
{
public:

	/**
	 * \brief Constructeur : initialise un modeleur � partir d'une chaine de caract�re
	 * \param s : La chaine de caract�re
	 */
	OgreDeclarativeModeler( const std::string & s );

	/**
	 * \bried Destructeur
	 */
	virtual ~OgreDeclarativeModeler();
	
	virtual void assignModelToActor( Actor* );

	virtual void assignModelsToActors();

protected:

};

#endif /*_OGRE_DECLARATIVE_MODELER_H_*/
