/**************************************************************************
 * 3D Studio File Toolkit for Release 3 
 * 
 * (C) Copyright 1997 by Autodesk, Inc. 
 *
 * License Agreement
 *
 * This Autodesk Program is copyrighted by Autodesk, Inc. and is
 * licensed to you (individual or a legal entity) under the following
 * conditions:
 *
 * You may use, modify, copy, reproduce, distribute, sell, and market
 * the Autodesk Program, incorporated in whole or a portion thereof,
 * solely as a part of a Larger Work ("Larger Work" is defined as a
 * work which contains the Autodesk Program or portions thereof with
 * software/programs not governed by the terms of this License) provided
 * such Larger Works:
 *     (i)   are designed and intended to work solely with Autodesk, Inc.
 *           products,
 *     (ii.) conspicuously contain Autodesk's copyright notice
 *           "(C) Copyright 1995 by Autodesk, Inc.",
 *     (iii) contain a copy of this license along with the Autodesk
 *           Program, (iv) contain the disclaimer of warranty and all
 *           notices that refer to this License and to the absence of
 *           any warranty;
 *     (v)   add substantial value in addition to the Autodesk Program. 
 *
 * Any derivative or modification of this Autodesk Program must be
 * distributed, published and licensed under the same conditions as
 * this License.  
 *
 * You may not license or distribute the Autodesk Program as a standalone
 * program or product including OEM and private label.
 *
 * You may not use, copy, modify, sublicense or distribute the Autodesk
 * Program or any portion thereof in any form if such use or distribution
 * is not expressly licensed and or is expressly prohibited under this
 * License. 
 * 
 * You acknowledge and agree that Autodesk shall own all right, title
 * and interest in the Autodesk Program and all rights in patents whether
 * now known or hereafter discovered. You do not hold and shall not claim
 * any interest whatsoever in the Autodesk Program.  
 *
 * You agree that the Autodesk Program, any portion or derivative
 * thereof will not be shipped, transferred or exported into any country
 * or used in any manner prohibited by the United States Export
 * Administration Act or any other applicable export control law,
 * restriction or regulation.
 *
 * NO WARRANTY.
 * AUTODESK PROVIDES THIS PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE. AUTODESK, INC. DOES
 * NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE UNINTERRUPTED
 * OR ERROR FREE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF
 * THE PROGRAM IS WITH YOU. SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 * (NOT AUTODESK) ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR
 * OR CORRECTION. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL
 * PART OF THIS LICENSE. NO USE OF THE PROGRAM IS AUTHORIZED HEREUNDER
 * EXCEPT UNDER THIS DISCLAIMER.
 *
 * LIMITATION OF LIABILITY.
 * IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL AUTODESK, OR ANY
 * OTHER PARTY WHO MAY MODIFY AND/OR REDISTRIBUTE THIS PROGRAM AS
 * PERMITTED HEREIN, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL,
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE
 * OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO LOSS OF
 * DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR
 * THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
 * SOFTWARE), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES. 
 *
 * This License will be governed by the laws of the State of California,
 * U.S.A., excluding the application of its conflicts of law rules.
 * If any part of this License is found void and unenforceable, it will
 * not affect the validity of the balance of the License, which shall
 * remain valid and enforceable according to its terms.
 *
 * This License and the rights granted hereunder will terminate
 * automatically if you fail to comply with the terms herein.  All
 * sublicenses to the Autodesk Program which are properly granted shall
 * survive any termination of this license.
 *************************************************************************/ 

#ifndef OMNM_3DS_H
#define OMNM_3DS_H

#include "3dstype.h"
#include "3dsprim.h"
#include "dbase3ds.h"
#include "chunk3ds.h"

/* Public Typedefs */

/* Used By OmnilightMotion3ds */

typedef struct {
   char3ds name[11];    /* Name of the light object node */
   char3ds parent[22];  /* Name of the parent object */

   ushort3ds flags1;    /* flags field from node header -fixup later */
   ushort3ds flags2;    /* flags2 field from node header -fixup later */

   ulong3ds npkeys;     /* Number of position keys */
   ushort3ds npflag;     /* Loop control flag for position keys */
   keyheader3ds *pkeys; /* Spline values for position keys */
   point3ds *pos;       /* Position keys */

   ulong3ds nckeys;     /* Number of color keys */
   ushort3ds ncflag;     /* Loop control flag for color keys */
   keyheader3ds *ckeys; /* Spline values for position keys */
   fcolor3ds *color;   /* Color keys */

} kfomni3ds;
/* End Public */

/* Public Prototypes */
void InitOmnilightMotion3ds(kfomni3ds **light, 
			    ulong3ds npkeys, 
			    ulong3ds nckeys);

void ReleaseOmnilightMotion3ds(kfomni3ds **light);

void GetOmnilightNodeNameList3ds(database3ds *db, 
				 namelist3ds **list);

void GetOmnilightMotionByName3ds(database3ds *db, 
				 char3ds *name, 
				 kfomni3ds **kfomni);

void GetOmnilightMotionByIndex3ds(database3ds *db, 
				  ulong3ds index,
				  kfomni3ds **kfomni);

ulong3ds GetOmnilightNodeCount3ds(database3ds *db);

void PutOmnilightMotion3ds(database3ds *db, 
			   kfomni3ds *kfomni);

void PutOmnilightMotion3ds(database3ds *db, 
			   kfomni3ds *kfomni);

void DeleteOmnilightMotionByIndex3ds(database3ds *db, ulong3ds index);
void DeleteOmnilightMotionByName3ds(database3ds *db, char3ds *name);
void CopyOmnilightMotionByIndex3ds(database3ds *destdb, database3ds *srcdb, ulong3ds index);
void CopyOmnilightMotionByName3ds(database3ds *destdb, database3ds *srcdb, char3ds *name);
/* End Public */
void GetOmnilightMotion3ds(chunk3ds *pOmniChunk,
			   kfomni3ds **kfomni);




#endif
