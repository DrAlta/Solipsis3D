/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

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

#ifndef _SOLIDBOOLOP_H_
#define _SOLIDBOOLOP_H_

#include <map>
#include <vector>

#include "csg/geometry.h"
#include "csg/isect.h"
#include "csg/error.h"
#include "csg/raytrace.h"
#include "csg/vector.h"


static void do_matching_contours(BREP_WING* wingB);
static void IterateMatchingContours(BREP_FACE* faceA, BREP_FACE* faceB, void (*func)(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB));
static void unset_mark(BREP_CONTOUR* contour);
static unsigned int inoutmark(BREP_CONTOUR* contour);
static void mark_contour(BREP_CONTOUR* contour, unsigned int mask);
static void keep_one_same(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB);
static void keep_one_opposite(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB);
static void partition_shells(BREP_SHELL* shellA, BREP_SHELL* shellB);
static void contour_propagate_mark(BREP_CONTOUR* contour);
static void wing_propagate_mark(BREP_WING* wing, unsigned int mark);
static void contour_mark_unmarked(BREP_CONTOUR* contour);
static void csg_propagate_mark(SOLID* solid);
static void csg_mark_unmarked(SOLID* solid, SOLID* othersolid);
static void mark_out(BREP_CONTOUR* contour);
static void mark_in(BREP_CONTOUR* contour);
static void mark_in_reverse(BREP_CONTOUR* contour);
static void mark_with_outmask(BREP_CONTOUR* contour);

static unsigned long GetVertexId(BREP_VERTEX *vertex);
static void OutputVertexId(BREP_VERTEX *vertex);
static void OutputReverseContour(BREP_CONTOUR* contour);
static void DoOutputContour(CONTOUR* contour);
static void OutputFaceWithHoles(BREP_FACE *face);
static int FaceWithHoles(BREP_FACE* face);
static void OutputFace(BREP_FACE* face);
static void OutputShell(BREP_SHELL* shell);

static VECTOR GetNormal( VECTOR v1, VECTOR v2, VECTOR v3 );

// class Constructive Solid Geometry
class SBO
{
public:
	// Case of boolean operation
	typedef enum Operation { OP_UNION, OP_INTERSECTION, OP_DIFFERENCE };

	// Constructor & Destructor
	SBO() {};
	~SBO() {};

	// Reads the objects and collects them into one SOLID
	SOLID* dataLoad(float* vertex, 
					unsigned int numVertex, 
					unsigned int vertexDeclSize, 
					unsigned int* index, 
					unsigned int numIndex );

	// Apply the boolean operation between SOLID A & B
	void applyBoolOp(Operation op, SOLID* solidA, SOLID* solidB);

	// Extract vertex a index datas from the modified solid
	void OutputSolid(BREP_SOLID* solid,
		float* &vertex, unsigned int &vertexCount, unsigned int vertexDecl,
		unsigned int* &index, unsigned int &indexCount );

private:
	void AddVertex(VECTOR vector);



private:
	std::vector<VERTEX*> points;
	SOLID* currentSolid;
	SHELL* currentShell;
	FACE* currentFace;
	CONTOUR* currentContour;

	//	Operation operation;
	bool newObject;
};

#endif // !_SOLIDBOOLOP_H_