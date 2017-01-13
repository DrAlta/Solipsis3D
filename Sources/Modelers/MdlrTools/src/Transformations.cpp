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

#include "transformations.h"

namespace Solipsis {

Transformations::Transformations(void)
{
    mMode = SELECT ;
    mOldpos = Vector3::ZERO ;

    mAxeClicked = Transformations::NONE ;
    mPlaneClicked = Transformations::NONE ;

    // Create the resource group
    mResourceGroup = "TransformationsResources";
    ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
    ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);
}

//-------------------------------------------------------------------------------------
Transformations::~Transformations(void)
{
    // Destroy the resource group
    ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
}

//-------------------------------------------------------------------------------------
void Transformations::firstClickForTransformation( const OIS::MouseEvent &e)
{
    RaySceneQueryResult &result = raySceneQuery(e);
    firstClickForTransformation(result);
}

void Transformations::firstClickForTransformation( RaySceneQueryResult &result )
{
    String NameAxeX ;
    String NameAxeY ;
    String NameAxeZ ;

    switch( mMode )
    {
    case Transformations::MOVE :
        NameAxeX = "moveX" ;
        NameAxeY = "moveY" ;
        NameAxeZ = "moveZ" ;
        break;
    case Transformations::ROTATE :
        NameAxeX = "rotateX" ;
        NameAxeY = "rotateY" ;
        NameAxeZ = "rotateZ" ;
        break;
    case Transformations::SCALE :
        NameAxeX = "scaleX" ;
        NameAxeY = "scaleY" ;
        NameAxeZ = "scaleZ" ;
        break;
    }
    onClickToTransformObject(result,NameAxeX, NameAxeY, NameAxeZ);
    mOldpos = Vector3::ZERO ;//Update mOldpos

}
//-------------------------------------------------------------------------------------
void Transformations::releasedClickForTransformation ()
{
#if 1 // GILLES MDLR
    mNode_X->removeAllChildren();
    mNode_Y->removeAllChildren();
    mNode_Z->removeAllChildren();
#else
	SceneNode* node;
	switch( mMode )
	{
		case Transformations::MOVE :
		{
			switch (mAxeClicked)
			{
				case Transformations::X :
					node = mPlaneX->getParentSceneNode() ;
					mNode_X->detachObject(mPlaneZ);
					mNode_X->detachObject(mPlaneY);
					break ;
				case Transformations::Y :
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case Transformations::Z :
					mNode_Z->detachObject(mPlaneY);
					mNode_Z->detachObject(mPlaneZ);
					break ;
			}
			break;
		}
		case Transformations::ROTATE :
		{
			switch (mAxeClicked)
			{
				case Transformations::X :
					mNode_X->detachObject(mPlaneX);
					mNode_X->detachObject(mPlaneY);
					mNode_X->detachObject(mPlaneZ);
					break ;
				case Transformations::Y :
					mNode_Y->detachObject(mPlaneX);
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case Transformations::Z :
					mNode_Z->detachObject(mPlaneX);
					mNode_Z->detachObject(mPlaneY);
					mNode_Z->detachObject(mPlaneZ);
					break ;
			}
			break;							
		}
		case Transformations::SCALE :
		{
			switch (mAxeClicked)
			{
				case Transformations::X :
					mNode_X->detachObject(mPlaneY);
					mNode_X->detachObject(mPlaneZ);
					break ;
				case Transformations::Y :
					mNode_Y->detachObject(mPlaneY);
					mNode_Y->detachObject(mPlaneZ);
					break ;
				case Transformations::Z :
					mNode_Z->detachObject(mPlaneY);
					mNode_Z->detachObject(mPlaneZ);
					break ;
			}
			break;
		}
	}
#endif
	mAxeClicked = Transformations::NONE;
	mPlaneClicked = Transformations::NONE;
	mOldpos = Vector3::ZERO;	
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::drapNdrop( const OIS::MouseEvent &e )
{
    Vector3 newpos = getMousePosOnDummyPlane (e);
    return drapNdrop(newpos);
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::drapNdrop( Vector3 newpos )
{
    //Calculate the mouse mouvement on the scene
    //!!!	Vector3 newpos = getMousePosOnDummyPlane (x, y);
    Vector3 moving = Vector3::ZERO ; //newpos - mOldpos ;

    if(mOldpos != Vector3::ZERO)
    {
        //Calculate transformation
        // Jerome
        /*		switch(mMode )
        {
        case Transformations::MOVE :	//Move objects
        {
        Matrix3 nodeSelectionRotationMatrix;
        nodeSelectionOrientation.ToRotationMatrix(nodeSelectionRotationMatrix);
        //nodeSelectionRotationMatrix.Inverse();
        Vector3 delta=newpos-mOldpos;
        Vector3 newdelta=delta*nodeSelectionRotationMatrix;

        switch (mAxeClicked )	//search axis
        {
        case Transformations::X :		//move X		
        moving.x = newdelta.x; //newpos.x - mOldpos.x ;
        break;
        case Transformations::Y :		//move Y
        moving.y = newdelta.y;//newpos.y - mOldpos.y ;
        break ;
        case Transformations::Z :		//move Z
        moving.z = newdelta.z; //newpos.z - mOldpos.z ;
        break ;
        }
        break;
        }
        case Transformations::ROTATE :	//Rotate objects
        {
        switch (mAxeClicked )	//search axis
        {
        case Transformations::X :		//move X		
        //moving.z = newpos.x - mOldpos.x ;
        moving.x =  newpos.y - mOldpos.y ;
        break;
        case Transformations::Y :		//move Y
        //moving.x = newpos.y - mOldpos.y ;
        moving.z = mOldpos.y - newpos.y ;
        break ;
        case Transformations::Z :		//move Z
        //moving.y = newpos.x - mOldpos.x ;
        moving.y = newpos.x - mOldpos.x ;
        break ;
        }
        break;
        }
        case Transformations::SCALE :	//Scale objects, but not gizmos
        {
        moving = Vector3 (1, 1, 1);
        switch (mAxeClicked )	//search axis
        {
        case Transformations::X :		//move X		
        moving.z = abs( 1 + (newpos.x - mOldpos.x)/15.0 ) ;
        break;
        case Transformations::Y :		//move Y
        moving.y = abs( 1 + (mOldpos.y - newpos.y )/30.0 ) ;
        break ;
        case Transformations::Z :		//move Z
        moving.x = abs( 1 + (mOldpos.x - newpos.x )/30.0 ) ;
        //moving.z = abs( 1 + (newpos.z - mOldpos.z)/100.0 ) ;

        break ;
        }
        break;
        }
        }
        */

        switch(mMode )
        {
        case Transformations::MOVE :	//Move objects
            {
                switch (mAxeClicked )	//search axis
                {
                case Transformations::X :		//move X		
                    moving.x = newpos.x - mOldpos.x ;
                    break;
                case Transformations::Y :		//move Y
                    moving.y = newpos.y - mOldpos.y ;
                    break ;
                case Transformations::Z :		//move Z
                    moving.z = newpos.z - mOldpos.z ;
                    break ;
                }
                break;
            }
        case Transformations::ROTATE :	//Rotate objects
            {
                switch (mAxeClicked )	//search axis
                {
                case Transformations::X :		//move X		
                    //moving.z = newpos.z - mOldpos.z ;
                    moving.z = newpos.x - mOldpos.x ;
                    break;
                case Transformations::Y :		//move Y
                    moving.x = newpos.x - mOldpos.x ;
                    break ;
                case Transformations::Z :		//move Z
                    //moving.y = -(newpos.y - mOldpos.y) ;
                    moving.y = newpos.x - mOldpos.x ;
                    break ;
                }
                break;
            }
        case Transformations::SCALE :	//Scale objects, but not gizmos
            {
                moving = Vector3 (1, 1, 1);
                switch (mAxeClicked )	//search axis
                {
                case Transformations::X :		//move X		
                    moving.x = abs( 1 + (newpos.x - mOldpos.x)/100.0 ) ;
                    break;
                case Transformations::Y :		//move Y
                    moving.y = abs( 1 + (newpos.y - mOldpos.y)/100.0 ) ;
                    break ;
                case Transformations::Z :		//move Z
                    moving.z = abs( 1 + (newpos.z - mOldpos.z)/100.0 ) ;
                    break ;
                }
                break;
            }
        }
    }
    mOldpos = newpos;
    return moving;
}
//-------------------------------------------------------------------------------------
void Transformations::eventSelection()
{
    showGizmosMove(false) ;
    showGizmosScale(false) ;
    showGizmosRotate(false) ;

    mNode_X->detachAllObjects();
    mNode_Y->detachAllObjects();
    mNode_Z->detachAllObjects();

    mMode = SELECT ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventMove()
{
    // remove all glow effect on the axes's material
    MaterialPtr material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoX");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoY");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoZ");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");

    mNode_X->detachAllObjects();
    mNode_Y->detachAllObjects();
    mNode_Z->detachAllObjects();

    //Attach the good Gizmos
    attachMoveGizmos(true);

    showGizmosMove(true) ;
    showGizmosScale(false) ;
    showGizmosRotate(false) ;

    mMode = MOVE ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventRotation()
{
    // remove all glow effect on the axes's material
    MaterialPtr material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoX");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoY");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoZ");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");

    mNode_X->detachAllObjects();
    mNode_Y->detachAllObjects();
    mNode_Z->detachAllObjects();

    //Attach the good Gizmos
    attachRotateGizmos(true);

    showGizmosMove(false) ;
    showGizmosScale(false) ;
    showGizmosRotate(true) ;

    mMode = ROTATE ;
}
//-------------------------------------------------------------------------------------
void Transformations::eventScale()
{
    // remove all glow effect on the axes's material
    MaterialPtr material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoX");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoY");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
    material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoZ");
    material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");

    mNode_X->detachAllObjects();
    mNode_Y->detachAllObjects();
    mNode_Z->detachAllObjects();

    //Attach the good Gizmos
    attachScaleGizmos(true);

    showGizmosMove(false) ;
    showGizmosScale(true) ;
    showGizmosRotate(false) ;

    mMode = SCALE ;
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosMove(bool pShow)
{
    m_moveX->setVisible(pShow);
    m_moveY->setVisible(pShow);
    m_moveZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosScale(bool pShow)
{
    m_scaleX->setVisible(pShow);
    m_scaleY->setVisible(pShow);
    m_scaleZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::showGizmosRotate(bool pShow)
{
    m_rotateX->setVisible(pShow);
    m_rotateY->setVisible(pShow);
    m_rotateZ->setVisible(pShow);
}
//-------------------------------------------------------------------------------------
void Transformations::createGizmos(SceneNode* pGizmosParentNode, SceneManager * pSceneMgr, Camera* pCamera)
{
    mCamera = pCamera ;
    mRaySceneQuery = pSceneMgr->createRayQuery( Ray() );

    //SceneNodes :
    m_GizmosAxesNode = pGizmosParentNode->createChildSceneNode("GizmosAxesNode");
	
	//Create sceneNodes
	mNode_X = m_GizmosAxesNode->createChildSceneNode("move_widget_x");
	mNode_Y = m_GizmosAxesNode->createChildSceneNode("move_widget_y");
	mNode_Z = m_GizmosAxesNode->createChildSceneNode("move_widget_z");

	createGizmosMove(pSceneMgr);
	createGizmosScale(pSceneMgr);
	createGizmosRotate(pSceneMgr);

	MovablePlane * movPlane ;
	movPlane = new MovablePlane("dummy_plane_x");
	movPlane->normal = Vector3::UNIT_Y;
	MeshManager::getSingleton().createPlane("dummy_plane_x", mResourceGroup,*movPlane, 100, 100,1, 1, true, 1, 1, 1, Vector3::UNIT_X);
	mPlaneX = pSceneMgr->createEntity( "dummy_plane_x", "dummy_plane_x" );
	mPlaneX->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
	mPlaneX->setVisible(false);
#if 1 // GILLES MDLR
    mPlane_Node_X = pSceneMgr->createSceneNode("move_Plane_X");
    mPlane_Node_X->attachObject(mPlaneX);
    mPlane_Node_X->setInheritOrientation(false);
#endif

	movPlane = new MovablePlane("dummy_plane_y");
	movPlane->normal = Vector3::UNIT_Z;
	MeshManager::getSingleton().createPlane("dummy_plane_y", mResourceGroup,*movPlane, 100, 100, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
	mPlaneY = pSceneMgr->createEntity( "dummy_plane_y", "dummy_plane_y" );
	mPlaneY->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    mPlaneY->setVisible(false);
#if 1 // GILLES MDLR
    mPlane_Node_Y = pSceneMgr->createSceneNode("move_Plane_Y");
    mPlane_Node_Y->attachObject(mPlaneY);
    mPlane_Node_Y->setInheritOrientation(false);
#endif

	movPlane = new MovablePlane("dummy_plane_z");
	movPlane->normal = Vector3::UNIT_X;
	MeshManager::getSingleton().createPlane("dummy_plane_z", mResourceGroup,*movPlane, 100, 100, 1, 1, true, 1, 1, 1, Vector3::UNIT_Z);
	mPlaneZ = pSceneMgr->createEntity( "dummy_plane_z", "dummy_plane_z" );
	mPlaneZ->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
	mPlaneZ->setVisible(false);
#if 1 // GILLES MDLR
    mPlane_Node_Z = pSceneMgr->createSceneNode("move_Plane_Z");
    mPlane_Node_Z->attachObject(mPlaneZ);
    mPlane_Node_Z->setInheritOrientation(false);
#endif
}

//-------------------------------------------------------------------------------------
void Transformations::destroyGizmos(SceneManager * pSceneMgr)
{
    pSceneMgr->destroyEntity("AxeTest");
    pSceneMgr->destroyEntity("dummy_plane_x");
    pSceneMgr->destroyEntity("dummy_plane_y");
    pSceneMgr->destroyEntity("dummy_plane_z");
    pSceneMgr->destroyEntity("moveX");
    pSceneMgr->destroyEntity("moveY");
    pSceneMgr->destroyEntity("moveZ");
    pSceneMgr->destroyEntity("scaleX");
    pSceneMgr->destroyEntity("scaleY");
    pSceneMgr->destroyEntity("scaleZ");
    pSceneMgr->destroyEntity("rotateX");
    pSceneMgr->destroyEntity("rotateY");
    pSceneMgr->destroyEntity("rotateZ");
    // Meshes should be freed when no more referenced
    /*    MeshManager::getSingleton().remove("axes.mesh");
    MeshManager::getSingleton().remove("dummy_plane_x");
    MeshManager::getSingleton().remove("dummy_plane_y");
    MeshManager::getSingleton().remove("dummy_plane_z");
    MeshManager::getSingleton().remove("axe_move_x.mesh");
    MeshManager::getSingleton().remove("axe_move_y.mesh");
    MeshManager::getSingleton().remove("axe_move_z.mesh");
    MeshManager::getSingleton().remove("axe_scale_x.mesh");
    MeshManager::getSingleton().remove("axe_scale_y.mesh");
    MeshManager::getSingleton().remove("axe_scale_z.mesh");
    MeshManager::getSingleton().remove("axe_rotate_x.mesh");
    MeshManager::getSingleton().remove("axe_rotate_y.mesh");
    MeshManager::getSingleton().remove("axe_rotate_z.mesh");*/
    // Cloned materials should be freed when no more referenced
    /*    MaterialManager::getSingleton().remove("MaterialGizmosX");
    MaterialManager::getSingleton().remove("MaterialGizmosY");
    MaterialManager::getSingleton().remove("MaterialGizmosZ");*/
}

//-------------------------------------------------------------------------------------
void Transformations::createGizmosMove(SceneManager * pSceneMgr)
{
    //Entity* OgreAxes = pSceneMgr->createEntity( "AxeTest", "axes.mesh" );
    //OgreAxes->getSubEntity(0)->getMaterial()->clone("MaterialGizmos");

    MaterialPtr material = MaterialManager::getSingletonPtr()->create( "MaterialGizmos", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME );
    material->setLightingEnabled( false );
    material->setSceneBlending( SBT_TRANSPARENT_ALPHA );
    material->setDepthFunction( CMPF_ALWAYS_PASS );
    material->setDepthCheckEnabled( false );
    material->setDepthWriteEnabled( true );
    material->setCullingMode( Ogre::CULL_NONE );
    material->setDiffuse( ColourValue::White );
    material->setAmbient( .5, .5, .5 );
    //material->setColourWriteEnabled( true );
    material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");

    material->clone("MaterialGizmoX");
    material->clone("MaterialGizmoY");
    material->clone("MaterialGizmoZ");

    m_moveX = pSceneMgr->createEntity( "moveX", "axe_move_x.mesh"  );
    m_moveX->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_moveX->getSubEntity(0)->setMaterialName("MaterialGizmoX");
    mNode_X->roll( Degree(-90));
    mNode_X->yaw( Degree (90));

    m_moveY = pSceneMgr->createEntity( "moveY", "axe_move_y.mesh"  );
    m_moveY->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_moveY->getSubEntity(0)->setMaterialName("MaterialGizmoY");
    mNode_Y->yaw(Degree(90));

    m_moveZ = pSceneMgr->createEntity( "moveZ", "axe_move_z.mesh"  );
    m_moveZ->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_moveZ->getSubEntity(0)->setMaterialName("MaterialGizmoZ");
    mNode_Z->pitch( Degree(90));

    //Hide objects :
    m_moveX->setVisible(false);
    m_moveY->setVisible(false);
    m_moveZ->setVisible(false);
}
//-------------------------------------------------------------------------------------
void Transformations::createGizmosScale(SceneManager * pSceneMgr)
{
    m_scaleX = pSceneMgr->createEntity( "scaleX", "axe_scale_x.mesh"  );
    m_scaleX->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_scaleX->getSubEntity(0)->setMaterialName("MaterialGizmoX");

    m_scaleY = pSceneMgr->createEntity( "scaleY", "axe_scale_y.mesh"  );
    m_scaleY->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_scaleY->getSubEntity(0)->setMaterialName("MaterialGizmoY");

    m_scaleZ = pSceneMgr->createEntity( "scaleZ", "axe_scale_z.mesh"  );
    m_scaleZ->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_scaleZ->getSubEntity(0)->setMaterialName("MaterialGizmoZ");

    m_scaleX->setVisible(false);
    m_scaleY->setVisible(false);
    m_scaleZ->setVisible(false);

}
//-------------------------------------------------------------------------------------
void Transformations::createGizmosRotate(SceneManager * pSceneMgr)
{
    m_rotateX = pSceneMgr->createEntity( "rotateX", "axe_rotate_x.mesh"  );
    m_rotateX->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_rotateX->getSubEntity(0)->setMaterialName("MaterialGizmoX");

    m_rotateY = pSceneMgr->createEntity( "rotateY", "axe_rotate_y.mesh"  );
    m_rotateY->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_rotateY->getSubEntity(0)->setMaterialName("MaterialGizmoY");

    m_rotateZ = pSceneMgr->createEntity( "rotateZ", "axe_rotate_z.mesh"  );
    m_rotateZ->setQueryFlags(SceneManager::ENTITY_TYPE_MASK);
    m_rotateZ->getSubEntity(0)->setMaterialName("MaterialGizmoZ");

    m_rotateX->setVisible(false);
    m_rotateY->setVisible(false);
    m_rotateZ->setVisible(false);
}
//-------------------------------------------------------------------------------------
void Transformations::attachMoveGizmos(bool pAttach)
{
    if (pAttach)		//attach Gizmos
    {
        mNode_X->attachObject(m_moveX);	//XX arrows
        mNode_Y->attachObject(m_moveY);	//YY arrows
        mNode_Z->attachObject(m_moveZ);	//ZZ arrows
    }
    else				//detach gizmos
    {
        mNode_X->detachObject(m_moveX);	//XX arrows
        mNode_Y->detachObject(m_moveY);	//YY arrows
        mNode_Z->detachObject(m_moveZ);	//ZZ arrows
    }
}
//-------------------------------------------------------------------------------------
void Transformations::attachScaleGizmos(bool pAttach)
{
    if (pAttach)		//attach Gizmos
    {
        mNode_X->attachObject(m_scaleX);	//XX arrows
        mNode_Y->attachObject(m_scaleY);	//YY arrows
        mNode_Z->attachObject(m_scaleZ);	//ZZ arrows
    }
    else				//detach gizmos
    {
        mNode_X->detachObject(m_scaleX);	//XX arrows
        mNode_Y->detachObject(m_scaleY);	//YY arrows
        mNode_Z->detachObject(m_scaleZ);	//ZZ arrows
    }
}
//-------------------------------------------------------------------------------------
void Transformations::attachRotateGizmos(bool pAttach)
{
    if (pAttach)		//attach Gizmos
    {
        mNode_X->attachObject(m_rotateX);	//XX arrows
        mNode_Y->attachObject(m_rotateY);	//YY arrows
        mNode_Z->attachObject(m_rotateZ);	//ZZ arrows
    }
    else				//detach gizmos
    {
        mNode_X->detachObject(m_rotateX);	//XX arrows
        mNode_Y->detachObject(m_rotateY);	//YY arrows
        mNode_Z->detachObject(m_rotateZ);	//ZZ arrows
    }
}

//-------------------------------------------------------------------------------------
RaySceneQueryResult& Transformations::raySceneQuery( const OIS::MouseEvent &e )
{
    Ray mouseRay = mCamera->getCameraToViewportRay( 
        0.5/float(e.state.width), 
        0.5/float(e.state.height) );
    return raySceneQuery(mouseRay);
}
//-------------------------------------------------------------------------------------
RaySceneQueryResult& Transformations::raySceneQuery( Ray mouseRay )
{
    // Setup the ray scene query, use CEGUI's mouse position
    //CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
    // TODO : get Navi coord here !!
    //Ray mouseRay = mCamera->getCameraToViewportRay( 
    //	mousePos.d_x/float(e.state.width), 
    //	mousePos.d_y/float(e.state.height) );
    /*!!!
    Ray mouseRay = mCamera->getCameraToViewportRay( 
    0.5/float(x), 
    0.5/float(y) );
    */
    mRaySceneQuery->setRay( mouseRay );
    mRaySceneQuery->setSortByDistance( true, 5 );

    return mRaySceneQuery->execute();
}
//-------------------------------------------------------------------------------------
void Transformations::onClickToTransformObject(const OIS::MouseEvent &e, const String pNameAxeX,
    const String pNameAxeY, const String pNameAxeZ)
{
    onClickToTransformObject(raySceneQuery(e), pNameAxeX, pNameAxeY, pNameAxeZ);
}
//-------------------------------------------------------------------------------------
void Transformations::onClickToTransformObject(RaySceneQueryResult &result, const String pNameAxeX,
    const String pNameAxeY, const String pNameAxeZ)
{
    bool manuel_objet = false ;		//for search manuel object

    //!!!	RaySceneQueryResult &result = raySceneQuery(e);// Execute query
    RaySceneQueryResult::iterator itrRSQR =  result.begin( ); //+1

    if( ! result.empty() )
    {
        Ogre::String name;
        //search if there is a gizmo object in the same distance of the current object
        for(;itrRSQR != result.end() ; itrRSQR++)
        {
            name = itrRSQR->movable->getName();
            if( (name == "moveX") ||  (name == "moveY") || (name == "moveZ") ||
                (name == "scaleX") ||  (name == "scaleY") || (name == "scaleZ") ||
                (name == "rotateX") ||  (name == "rotateY") || (name == "rotateZ") )
            {
                manuel_objet = true ;
                break;
            }
        }

        if(manuel_objet)	//if we click on a gizmos
        {					//add planes to detect the mouse position on there
            MovableObject * nodeM = itrRSQR->movable ;
            String name = nodeM->getName();

            // remove all glow effect on the axes's material
            MaterialPtr material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoX");
            material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
            material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
            material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoY");
            material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
            material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");
            material = MaterialManager::getSingletonPtr()->getByName("MaterialGizmoZ");
            material->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
            material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName("axes.png");

            // update the selected axes's material (add a glow effect)
            material = ((Entity*)nodeM)->getSubEntity(0)->getMaterial();
            TextureUnitState* textureUnit = material->getTechnique(0)->getPass(0)->createTextureUnitState("glow");
            //textureUnit->setAnimatedTextureName( "glow.png", 4, 0.5 );
            textureUnit->setTextureName( "axes_selected.png" );
            textureUnit->setScrollAnimation( 0., -0.5 );
            textureUnit->setColourOperationEx( LBX_ADD_SIGNED, LBS_CURRENT, LBS_TEXTURE, ColourValue::White, ColourValue::White, 1. );

			switch( mMode )
			{
				case Transformations::MOVE :
				{
					if (name == pNameAxeX)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
#else
                        nodeM->getParentSceneNode()->attachObject(mPlaneY);
                        nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
						mAxeClicked = Transformations::X ;
					}
					else if (name == pNameAxeY)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Y);
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Z);
#else
                        nodeM->getParentSceneNode()->attachObject(mPlaneY);
                        nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
						mAxeClicked = Transformations::Y ;
					}
					else if( name == pNameAxeZ)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
#else
                        nodeM->getParentSceneNode()->attachObject(mPlaneY);
                        nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
						mAxeClicked = Transformations::Z ;
					}
                    break;
				}
				case Transformations::ROTATE :
				{
					if (name == pNameAxeX)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->removeAllChildren();
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Y);
                        mAxeClicked = Transformations::Y ;
#else
                        nodeM->getParentSceneNode()->detachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						mAxeClicked = Transformations::Y ;
#endif
					}
					else if (name == pNameAxeY)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->removeAllChildren();
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Z);
                        mAxeClicked = Transformations::Z ;
#else
                        nodeM->getParentSceneNode()->detachObject(mPlaneZ);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
						mAxeClicked = Transformations::Z ;
#endif
					}
					else if( name == pNameAxeZ)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->removeChild(mPlane_Node_Y);
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Y);
                        mAxeClicked = Transformations::X ;
#else
                        nodeM->getParentSceneNode()->detachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneX);
						mAxeClicked = Transformations::X ;
#endif        
					}		
					break;
				}
				case Transformations::SCALE :
				{
					if (name == pNameAxeX)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
#else
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
                        mAxeClicked = Solipsis::Transformations::X ;
					}
					else if (name == pNameAxeY)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Y);
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_Z);
#else
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
						mAxeClicked = Solipsis::Transformations::Y ;
					}
					else if( name == pNameAxeZ)
					{
#if 1 // GILLES MDLR
                        nodeM->getParentSceneNode()->addChild(mPlane_Node_X);
#else
						nodeM->getParentSceneNode()->attachObject(mPlaneY);
						nodeM->getParentSceneNode()->attachObject(mPlaneZ);
#endif
						mAxeClicked = Solipsis::Transformations::Z ;
					}
					break;
				}
			}
		}
	}


    mRaySceneQuery->clearResults();
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::getMousePosOnDummyPlane (const OIS::MouseEvent &e)
{
    Ray mouseRay = mCamera->getCameraToViewportRay( 
        0.5/float(e.state.width), 
        0.5/float(e.state.height) );
    return getMousePosOnDummyPlane(mouseRay);
}
//-------------------------------------------------------------------------------------
Vector3 Transformations::getMousePosOnDummyPlane (Ray mouseRay)
{
    //ray tracing :
    mRaySceneQuery->clearResults();
    mRaySceneQuery->setRay(mouseRay);
    mRaySceneQuery->setSortByDistance(true);
    mRaySceneQuery->setQueryMask(SceneManager::ENTITY_TYPE_MASK);
    RaySceneQueryResult &result = mRaySceneQuery->execute();

    RaySceneQueryResult::iterator itrRSQR =  result.begin( );

    if( ! result.empty() )
    {
        for ( ;itrRSQR != result.end(); itrRSQR++)	//search the good plane
        {
            if( itrRSQR->movable->getMovableType() == "Entity")
            {
                String name = itrRSQR->movable->getName();	//search the good object (dummy_plane) :
                if(name == mPlaneX->getName())
                {
                    if(mPlaneClicked == Transformations::NONE)	//if it is the first clck ...
                        mPlaneClicked = Transformations::X ;		//	remenber this plane

                    if(mPlaneClicked == Transformations::X)
                    {
                        //We can calculate the new position :
                        //LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Transformations::getMousePosOnDummyPlane() Dist X : %.2f", (*itrRSQR).distance);
                        Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
                        mRaySceneQuery->clearResults();
                        return pos ;
                    }
                }
                else if(name == mPlaneY->getName())
                {
                    if(mPlaneClicked == Transformations::NONE)	//if it is the first clck ...
                        mPlaneClicked = Transformations::Y ;		//	remenber this plane

                    if(mPlaneClicked == Transformations::Y)
                    {
                        //We can calculate the new position :
                        //LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Transformations::getMousePosOnDummyPlane() Dist Y : %.2f", (*itrRSQR).distance);
                        Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
                        mRaySceneQuery->clearResults();
                        return pos ;
                    }
                }
                else if(name == mPlaneZ->getName())
                {
                    if(mPlaneClicked == Transformations::NONE)	//if it is the first clck ...
                        mPlaneClicked = Transformations::Z ;		//	remenber this plane

                    if(mPlaneClicked == Transformations::Z)
                    {
                        //We can calculate the new position :
                        //LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Transformations::getMousePosOnDummyPlane() Dist Z : %.2f", (*itrRSQR).distance);
                        Vector3 pos = mouseRay.getPoint((*itrRSQR).distance);
                        mRaySceneQuery->clearResults();
                        return pos ;
                    }
                }
            }

        }
    }

    mRaySceneQuery->clearResults();
    return mOldpos;
}
//-------------------------------------------------------------------------------------
Transformations::Mode Transformations::getMode()
{
    return mMode;
}
 //-------------------------------------------------------------------------------------
Transformations::AxeClicked Transformations::getAxeClicked()
{
    return mAxeClicked;
}

} //namespace