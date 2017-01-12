/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - Fran�ois FOURNEL & Patrice DESFONDS

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __MyZipArchive_h__
#define __MyZipArchive_h__

#include <OgrePrerequisites.h>
//regular mem handler
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>

#include "Path.h"

using namespace Ogre;

namespace Solipsis {


class FileBuffer;


/// brief This is the class representing a MyZipArchive. From a path it can read or write zip and files in this zip.
/// file MyZipArchive.h
/// author Fran�ois FOURNEL & Patrice DESFONDS
/// date 2007.06.25

class MyZipArchive
{
public:
	///brief Constructor
	///param path Full path of the archive, universal for all OS (with "/").
	MyZipArchive(const String& path);

	///brief Destructor
	~MyZipArchive();



	///brief Method which tell whether or not the archive is present on the path.
	///return true if the archive is present on the path, false else
	bool isArchivePresent();

	///brief Method which test the presence of a file in the zip archive. Will return false too if the archive doesn't exist.
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	///return true if the file is present in the archive, false else.
	bool isFilePresent(const String& filePath);



	///brief Method which return the data of a file of the archive (in a FileBuffer). it will be an empty FileBuffer if the archive or the file isn't present.
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	///return The data of a file of the archive.
	FileBuffer readFile(const String& filePath);

	///brief Method which return the name of a file of the archive (in a string). it will be an empty name if the archive or the file isn't present.
	///param pNumFile Number of the file in the archive, 0 is the first.
	///return Name of a file of the archive.
	String getName(const int pNumFile);

	///brief Method which write a file to the zip. If the file already exists, it will be overwritten (It will be removed with the function removeFile). It will do nothing if the archive or the file isn't present. 
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	///param fileBuffer File to write.
	void writeFile(const String& filePath, const FileBuffer& fileBuffer);

	///brief see writeFile upper for doc. This methos automatically creates the FileBuffer.
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	void writeFile(const String& filePath);

	///brief Method which remove a file with a given path in a zip (if it is possible). In fact it rebuild the zip ,according to the original zip, without the file to remove. It won't do anything if the archive or the file is not present.
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	void removeFile(const String& filePath);

	///brief Method which copy a file into another archive.
	///param filePath Path of the file in the archive, universal for all OS (with "/").
	///param destFilePath Path of the file in the destination archive, universal for all OS (with "/").
	///param destArchive Destination archive.
    void copyFile(const String& filePath, const String& destFilePath, MyZipArchive& destArchive);

	///brief Method which return the number of file present in the current archive
	///return the number of file present in this archive
	int getNbFile() ;



private:
	Path* mPath;		///brief Full path of the archive, universal for all OS (with "/").

};

} //namespace


#endif
