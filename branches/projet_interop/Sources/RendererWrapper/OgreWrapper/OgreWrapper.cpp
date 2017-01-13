#include "OgreWrapper.h"
#include <OgrePrefabFactory.h>

OgreWrapper::OgreWrapper(Ogre::Root * mRoot, Ogre::SceneManager* scnMgr){
	init();
	this->cRoot = mRoot;
	this->cSceneMgr = scnMgr;
	this->curSceneNode = cSceneMgr->getRootSceneNode();
	initPrimitive();
	//m_configuration = new WrapperConfiguration(WRAPPER_TYPE,RENDERER_NAME,NULL,WRAPPER_VERSION);
}
OgreWrapper::~OgreWrapper(){
	cSceneMgr = NULL;
}

void OgreWrapper::init(){
	this->cRoot = NULL;
	this->cSceneMgr = NULL;
	this->curSceneNode = NULL;
	this->curEntity = NULL;
	this->curMaterialName = "";
	this->curManualObject = NULL;
	meshIsGeneric = false;
	mGenericSphere = NULL;
	mGenericCylinder = NULL;
	m_configuration->setIdentifier(RENDERER_NAME);
	m_configuration->setVersion(WRAPPER_VERSION);
	m_configuration->setType(WRAPPER_TYPE);
	_deletedNodesArray = std::vector<string>();
}


void OgreWrapper::clearPrivateDatas(){
	curEntity = NULL;
	curMaterialName = "";
	curManualObject = NULL;
	meshIsGeneric=false;
}

string OgreWrapper::createViewpoint( const std::vector<float> &position, const std::vector<float> &orientation, const string &viewpointId ){
	//string OgreWrapper::createViewpoint( const std::vector<float> &position, const std::vector<float> &orientation, string viewpointId ){

	//this->cSceneMgr->destroyAllCameras();

	//Ogre::Camera * mCamera = this->cSceneMgr->createCamera(viewpointId);
	//mCamera->setPosition(Real(position.at(0)),Real(position.at(1)),Real(position.at(2)));
	//mCamera->setOrientation(Quaternion(Real(orientation.at(0)),Real(orientation.at(1)),Real(orientation.at(2)),Real(orientation.at(3))));
	////mCamera->lookAt(Vector3(0,0,-300));
	//mCamera->setNearClipDistance(5);

	//Viewport* vp = cRoot->getRe


	/*RenderWindow* mWindow = cRoot->initialise(true,viewpointId);
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));
	mCamera->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));*/

	/*if(cFrameListener) delete cFrameListener;
	cFrameListener= new ExampleFrameListener(mWindow, mCamera);
	cFrameListener->showDebugOverlay(true);

	cRoot->addFrameListener(cFrameListener);*/
	return "";
}

string OgreWrapper::createGroupNode( const string &nodeId ){

	string res = string();
	if(!this->cSceneMgr->hasSceneNode(nodeId)){
		res = createSceneNode(nodeId);
	}
	return res;
}


string OgreWrapper::createTransformNode( const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const string &transformId ){

	string res = string();
	if(!this->cSceneMgr->hasSceneNode(transformId)){
		this->curSceneNode = this->curSceneNode->createChildSceneNode(
			transformId, 
			Vector3( Real(translation.at(0)),Real(translation.at(1)),Real(translation.at(2)) ), 
			Quaternion( Radian(Real(rotation.at(3))), Vector3( Real(rotation.at(0)),Real(rotation.at(1)),Real(rotation.at(2)) ) ) );
		this->curSceneNode->scale(Vector3(Real(scale.at(0)),Real(scale.at(1)),Real(scale.at(2))));
		this->curSceneNode->setInitialState();	
		res = transformId;
	}
	else {
		fprintf(stderr, "A SceneNode named %s already exists \n", transformId.c_str() );
	}
	return res;
}

void OgreWrapper::setTransformNode( const string &nodeId, const std::vector<float> &translation, const std::vector<float> &scale, const std::vector<float> &rotation, const TransformSpace relativeTo){

	Vector3 posI;
	SceneNode * sNode;
	Node * parentNode;
	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		sNode = this->cSceneMgr->getSceneNode(nodeId);
	}
	else if(this->cSceneMgr->hasEntity(nodeId) ){
		Entity * ent = this->cSceneMgr->getEntity(nodeId);
		sNode = ent->getParentSceneNode();
	}
	if(sNode){
		//sNode->resetToInitialState();
		bool isISG = sNode->isInSceneGraph();
		if( relativeTo == TransformSpace::WORLD){
			if(!translation.empty()){
				Vector3 pos = sNode->_getDerivedPosition();
				Vector3 vUpdated = Vector3(Real(translation.at(0)), Real(translation.at(1)), Real(translation.at(2)));
				Vector3 res = vUpdated-pos;
				sNode->translate(res);
			}
			if(!rotation.empty()) {
				Quaternion rot =  sNode->_getDerivedOrientation();
				Quaternion rotUpdated = Quaternion(Real(rotation.at(0)),Real(rotation.at(1)),Real(rotation.at(2)), Real(rotation.at(3)));
				Quaternion rotRes = rotUpdated-rot;
				sNode->rotate( rotRes );
			}
		}
		else{
			posI = sNode->_getDerivedPosition();
			/*this->cSceneMgr->getRootSceneNode()->_update(true,false);
			posI = sNode->_getDerivedPosition();*/
			Vector3 posL = sNode->getPosition();
			if(!translation.empty()){
				Vector3 vUpdated = Vector3(Real(translation.at(0)), Real(translation.at(1)), Real(translation.at(2)));
				Vector3 posR = vUpdated-posL;
				//sNode->translate(posR);
			}
			if(!translation.empty())sNode->setPosition( Real(translation.at(0)), Real(translation.at(1)), Real(translation.at(2)));
			if(!rotation.empty()) sNode->setOrientation(Real(rotation.at(0)),Real(rotation.at(1)),Real(rotation.at(2)), Real(rotation.at(3)));


		}
		if(!scale.empty()) sNode->setScale( Vector3(Real(scale.at(0)),Real(scale.at(1)),Real(scale.at(2)))  );
		//parentNode = sNode->getParentSceneNode();
		//parentNode = sNode->getParent();
		//if(parentNode) parentNode->_update(true,false);
		//sNode->_update(true,true);
		//sNode->needUpdate(true);
		/*SceneNode::queueNeedUpdate(sNode);
		SceneNode::processQueuedUpdates();*/
	}
}

string OgreWrapper::createPlane( const float width, const float height, const std::vector<float> &orientation, const string &shapeId ){


	string res = string();
	if(this->cRoot){
		if( !this->cSceneMgr->hasEntity(shapeId) ){

			try{
				Ogre::String meshName =  shapeId + "_mesh.mesh";
				Ogre::String entityName = shapeId;
				Ogre::Plane plane( Vector3(  Real(orientation.at(0)), Real(orientation.at(1)),  Real(orientation.at(2)) ), 0);
				Ogre::MeshPtr mptr = MeshManager::getSingleton().createPlane( meshName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, Real(width), Real(height),1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);
				this->curEntity = this->cSceneMgr->createEntity( entityName, meshName );
				res = shapeId;
			}
			catch( Ogre::Exception e ) {
				fprintf(stderr, "An exception has occurred: %s\n", e.what());
			}
		}
	}
	return res;
}

string OgreWrapper::createGround( const string &shapeId,const std::vector<float> &normal, const float constant, const std::vector<float> &color, const std::string urlTexture ){

	string res = string();
	if(this->cRoot){
		if( !this->cSceneMgr->hasEntity(shapeId) ){

			try{
				Ogre::String meshName =  shapeId + "_mesh.mesh";
				Ogre::String entityName = shapeId;

				Ogre::Plane plane( Vector3(  Real(normal.at(0)), Real(normal.at(1)),  Real(normal.at(2)) ), constant);
				Ogre::MeshPtr mptr = MeshManager::getSingleton().createPlane( meshName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, GROUND_WIDTH, GROUND_HEIGHT,1,1,true,1,1,1,Ogre::Vector3::UNIT_Z);

				Ogre::Entity * groundEntity = this->cSceneMgr->createEntity( entityName, meshName );

				Ogre::String groundMaterialName = String(shapeId) + ".material";
				MaterialPtr  material = MaterialManager::getSingleton().create( groundMaterialName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

				if(!urlTexture.empty()){
					size_t lastSlash = urlTexture.find_last_of("\\");
					Ogre::String mResourceFolder = urlTexture.substr(0,lastSlash+1);
					Ogre::String fileName = urlTexture.substr(lastSlash+1);
					material->getTechnique(0)->getPass(0)->createTextureUnitState(fileName);
				}

				groundEntity->setMaterialName(groundMaterialName);
				groundEntity->setCastShadows(true);

				cSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(groundEntity);

				res = shapeId;
			}
			catch( Ogre::Exception e ) {
				fprintf(stderr, "OgreWrapper => An exception has occurred: %s\n", e.what());
			}

		}
	}
	return res;
}

string OgreWrapper::createSphere(const float radius, const string &shapeId ){

	string res = string();
	float curRadius;

	if(this->cRoot){
		if( !this->cSceneMgr->hasEntity(shapeId) ){
			MeshPtr mptr = mGenericSphere->getMesh()->clone( String(shapeId) + ".mesh" );
			if( !mptr.isNull() ){	
				meshIsGeneric = true;
				curRadius = radius * 2;
				this->curEntity = this->cSceneMgr->createEntity( String(shapeId), String(shapeId) + ".mesh" );
				res = createSceneNode(shapeId + "_SphereScaleNode");
				if(!res.empty()){
					res = shapeId;
					this->curSceneNode->scale(Ogre::Vector3(curRadius,curRadius,curRadius));
				}
			}
		}
		else fprintf(stderr, "An Entity named %s already exists \n", shapeId.c_str() );
	}
	return res;
}

string OgreWrapper::createCylinder(const float radius, const float height, const string &shapeId ){

	string res = string();
	float curRadius;

	if(this->cRoot){
		if( !this->cSceneMgr->hasEntity(shapeId) ){
			MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(shapeId) + ".mesh" );
			if ( !mptr.isNull() ){
				meshIsGeneric = true;
				curRadius = radius * 2;
				this->curEntity = this->cSceneMgr->createEntity( String(shapeId), String(shapeId) + ".mesh" );
			}
			res = createSceneNode(shapeId + "_CylinderScaleNode");
			if(!res.empty()){
				res = shapeId;
				this->curSceneNode->scale(Ogre::Vector3(curRadius,height,curRadius));
			}
		}
		else fprintf(stderr, "An Entity named %s already exists \n", shapeId.c_str() );
	}
	return res;
}

string OgreWrapper::createBox( const std::vector<float> &size, const string &shapeId ){

	string res = string();
	if( !this->cSceneMgr->hasManualObject(shapeId) ){

		if(!size.empty()){
			this->curManualObject =  cSceneMgr->createManualObject(shapeId);
			curManualObject->begin(this->curMaterialName);

			float coordX = size.at(0);
			float coordY = size.at(1);
			float coordZ = size.at(2);
			const float sqrt13 = 0.577350269f; /* sqrt(1/3) */

			curManualObject->position(coordX,-coordY,coordZ);curManualObject->normal(sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(1,0);
			curManualObject->position(-coordX,-coordY,-coordZ);curManualObject->normal(-sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(0,1);
			curManualObject->position(coordX,-coordY,-coordZ);curManualObject->normal(sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(1,1);
			curManualObject->position(-coordX,-coordY,coordZ);curManualObject->normal(-sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(0,0);
			curManualObject->position(coordX,coordY,coordZ);curManualObject->normal(sqrt13,sqrt13,sqrt13);curManualObject->textureCoord(1,0);
			curManualObject->position(-coordX,-coordY,coordZ);curManualObject->normal(-sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(0,1);
			curManualObject->position(coordX,-coordY,coordZ);curManualObject->normal(sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(1,1);
			curManualObject->position(-coordX,coordY,coordZ);curManualObject->normal(-sqrt13,sqrt13,sqrt13);curManualObject->textureCoord(0,0);
			curManualObject->position(-coordX,coordY,-coordZ);curManualObject->normal(-sqrt13,sqrt13,-sqrt13);curManualObject->textureCoord(0,1);
			curManualObject->position(-coordX,-coordY,-coordZ);curManualObject->normal(-sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(1,1);
			curManualObject->position(-coordX,-coordY,coordZ);curManualObject->normal(-sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(1,0);
			curManualObject->position(coordX,-coordY,-coordZ);curManualObject->normal(sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(0,1);
			curManualObject->position(coordX,coordY,-coordZ);curManualObject->normal(sqrt13,sqrt13,-sqrt13);curManualObject->textureCoord(1,1);
			curManualObject->position(coordX,-coordY,coordZ);curManualObject->normal(sqrt13,-sqrt13,sqrt13);curManualObject->textureCoord(0,0);
			curManualObject->position(coordX,-coordY,-coordZ);curManualObject->normal(sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(1,0);
			curManualObject->position(-coordX,-coordY,-coordZ);curManualObject->normal(-sqrt13,-sqrt13,-sqrt13);curManualObject->textureCoord(0,0);
			curManualObject->position(-coordX,coordY,coordZ);curManualObject->normal(-sqrt13,sqrt13,sqrt13);curManualObject->textureCoord(1,0);
			curManualObject->position(coordX,coordY,-coordZ);curManualObject->normal(sqrt13,sqrt13,-sqrt13);curManualObject->textureCoord(0,1);
			curManualObject->position(-coordX,coordY,-coordZ);curManualObject->normal(-sqrt13,sqrt13,-sqrt13);curManualObject->textureCoord(1,1);
			curManualObject->position(coordX,coordY,coordZ);curManualObject->normal(sqrt13,sqrt13,sqrt13);curManualObject->textureCoord(0,0);

			curManualObject->triangle(0,1,2);      curManualObject->triangle(3,1,0);
			curManualObject->triangle(4,5,6);      curManualObject->triangle(4,7,5);
			curManualObject->triangle(8,9,10);      curManualObject->triangle(10,7,8);
			curManualObject->triangle(4,11,12);   curManualObject->triangle(4,13,11);
			curManualObject->triangle(14,8,12);   curManualObject->triangle(14,15,8);
			curManualObject->triangle(16,17,18);   curManualObject->triangle(16,19,17);
			curManualObject->end();
		}
		res = shapeId;
	}
	return res;
}


std::vector<float> OgreWrapper::getBoundingBox( const std::vector<float> &vertices){


	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = maxX = minY = maxY = minZ = maxZ = 0; 
	float val;
	int i;

	int nbCoord = vertices.size();

	for(i=0; i<nbCoord; i++){
		val = vertices.at(i);
		if(i%3==0){
			if(val<minX) minX=val;
			else if(val>maxX) maxX=val;
		}
		else if(i%3==1){
			if(val<minY) minY=val;
			else if(val>maxY) maxY=val;
		}
		else if(i%3==2){
			if(val<minZ) minZ=val;
			else if(val>maxZ) maxZ=val;
		}
	}
	float values [] = { minX, maxX, minY, maxY, minZ, maxZ };
	std::vector<float> res(values, values + sizeof(values) / sizeof(float) );

	return res;
}

string OgreWrapper::createMesh( const std::vector<float> &vertices,const std::vector<float> &normals, const std::vector<float> &textureCoord, const std::vector<float> &colours, const std::vector<unsigned short> &faces, const string &shapeId ){

	string res = string();
	HardwareVertexBufferSharedPtr vbuf;
	HardwareVertexBufferSharedPtr normalbuf;
	HardwareVertexBufferSharedPtr cbuf;
	HardwareVertexBufferSharedPtr texturebuf;
	HardwareIndexBufferSharedPtr  ibuf;

	if( !this->cSceneMgr->hasEntity(shapeId) ){
		if(this->cRoot){
			if(!vertices.empty() && vertices.size()>2){

				Ogre::String meshName =  shapeId + "_mesh.mesh";
				Ogre::String entityName = shapeId;

				Ogre::MeshPtr ogreMesh = MeshManager::getSingleton().createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				SubMesh* sub = ogreMesh->createSubMesh();

				size_t offset = 0;
				size_t normalOffset = 0;
				size_t colorOffset = 0;
				size_t texOffset = 0;
				unsigned short bindingIdx = 0;

				const int NUM_VERTICES = vertices.size()/3;
				const int NUM_INDICES = faces.size(); 

				const float* verticesArray;
				if(!vertices.empty()) verticesArray = &vertices[0];

				const float* normalArray;
				if(!normals.empty()) normalArray = &normals[0];

				const float* textureArray;
				if(!textureCoord.empty()) textureArray = &textureCoord[0];

				const unsigned short* facesArray;
				if(!faces.empty()) facesArray = &faces[0];

				std::vector<RGBA> colourVect;
				RGBA * colourArray;
				if(!colours.empty() && NUM_VERTICES!=0){
					RenderSystem* rs = Root::getSingleton().getRenderSystem();
					for(int i=0; i<NUM_VERTICES; i++){
						ColourValue curColor = ColourValue(colours.at(i),colours.at(i+1),colours.at(i+2));
						RGBA curRGBAColor = curColor.getAsRGBA();
						colourVect.push_back(curRGBAColor);
						rs->convertColourValue(curColor,(uint32*)&colourVect[i]);
					}
					colourArray = &colourVect[0];
				}

				ogreMesh->sharedVertexData = new VertexData();
				ogreMesh->sharedVertexData->vertexCount = NUM_VERTICES;

				VertexDeclaration* decl = ogreMesh->sharedVertexData->vertexDeclaration;
				VertexBufferBinding* bind = ogreMesh->sharedVertexData->vertexBufferBinding;

				// vertices buffer
				decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);//position
				offset += VertexElement::getTypeSize(VET_FLOAT3);

				vbuf = 
					HardwareBufferManager::getSingleton().createVertexBuffer(
					offset, NUM_VERTICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

				vbuf->writeData(0,vbuf->getSizeInBytes(), verticesArray, true );
				bind->setBinding(bindingIdx,vbuf);
				bindingIdx++;
				//vbuf->lock(HardwareBuffer::HBL_DISCARD);

				// normal buffer
				if(!normals.empty()){
					decl->addElement(bindingIdx, normalOffset, VET_FLOAT3, VES_NORMAL); //normal
					normalOffset += VertexElement::getTypeSize(VET_FLOAT3);

					normalbuf = 
						HardwareBufferManager::getSingleton().createVertexBuffer(
						normalOffset, NUM_VERTICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

					normalbuf->writeData(0,normalbuf->getSizeInBytes(), normalArray, true);
					bind->setBinding(bindingIdx,normalbuf);
					bindingIdx++;
					//normalbuf->lock(HardwareBuffer::HBL_DISCARD);
				}

				// color buffer
				if(!colours.empty()){
					decl->addElement(bindingIdx, colorOffset, VET_COLOUR, VES_DIFFUSE); 
					colorOffset += VertexElement::getTypeSize(VET_COLOUR);

					cbuf = 
						HardwareBufferManager::getSingleton().createVertexBuffer(
						colorOffset, NUM_VERTICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

					cbuf->writeData(0, cbuf->getSizeInBytes(), colourArray , true);	
					bind->setBinding(bindingIdx, cbuf);
					bindingIdx++;
					//cbuf->lock(HardwareBuffer::HBL_DISCARD);
				}

				// texture buffer
				if(!textureCoord.empty()){
					decl->addElement(bindingIdx, texOffset, VET_FLOAT2, VES_TEXTURE_COORDINATES); //normal
					texOffset += VertexElement::getTypeSize(VET_FLOAT2);

					texturebuf = 
						HardwareBufferManager::getSingleton().createVertexBuffer(
						texOffset, NUM_VERTICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

					texturebuf->writeData(0,texturebuf->getSizeInBytes(), textureArray, true);
					bind->setBinding(bindingIdx,texturebuf);
					bindingIdx++;
					//texturebuf->lock(HardwareBuffer::HBL_DISCARD);
				}

				if(!faces.empty()){

					sub->useSharedVertices = true;
					//HardwareIndexBufferSharedPtr  ibuf;

					if(NUM_VERTICES > NB_VERTICES_LIMIT ) ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
						HardwareIndexBuffer::IT_32BIT, NUM_INDICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY
						);
					else ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
						HardwareIndexBuffer::IT_16BIT, NUM_INDICES, HardwareBuffer::HBU_STATIC_WRITE_ONLY
						);
					sub->indexData->indexBuffer = ibuf;
					sub->indexData->indexCount = NUM_INDICES;
					sub->indexData->indexStart = 0;

					ibuf->writeData(0, ibuf->getSizeInBytes(), facesArray, true );
					//ibuf->lock(HardwareBuffer::HBL_DISCARD);
				}

				/// Set bounding information (for culling)
				std::vector<float>  bBox = getBoundingBox(vertices);
				ogreMesh->_setBounds(AxisAlignedBox(bBox.at(0),bBox.at(2),bBox.at(4),bBox.at(1),bBox.at(3),bBox.at(5)));
				ogreMesh->_setBoundingSphereRadius(Math::Sqrt(3*bBox.at(1)*bBox.at(3)));
				//ogreMesh->sharedVertexData->closeGapsInBindings();

				/// Notify -Mesh object that it has been loaded
				ogreMesh->load();

				if(!vbuf.isNull()) vbuf->lock(HardwareBuffer::HBL_DISCARD);
				if(!normalbuf.isNull()) normalbuf->lock(HardwareBuffer::HBL_DISCARD);
				if(!cbuf.isNull()) cbuf->lock(HardwareBuffer::HBL_DISCARD);
				if(!texturebuf.isNull()) texturebuf->lock(HardwareBuffer::HBL_DISCARD);
				if(!ibuf.isNull()) ibuf->lock(HardwareBuffer::HBL_DISCARD);

				Ogre::Resource::LoadingState lState =  ogreMesh->getLoadingState();

				this->curEntity = this->cSceneMgr->createEntity(entityName,meshName);
				res=entityName;
			}
		}
	}
	return res;
}

/**
transparency [0:1]: 0: non transparent, 1: transparent
alpha [0:1]: 0: transparent, 1: non transparent
*/
string OgreWrapper::createMaterial(const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> &diffuseColor, const std::vector<float> &emissiveColor, const std::vector<float> &specularColor, const string &materialId ){
	//string OgreWrapper::createMaterial(float ambientIntensity, float shininess, float alpha, float diffuseColor[3], float emissiveColor[3], float specularColor[3], string materialId ){

	this->curMaterialName = materialId;

	MaterialPtr  material = MaterialManager::getSingleton().create( curMaterialName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if(material.isNull()) return "";
	else{
		//material->getTechnique(0)->getPass(0)->setSceneBlending(SceneBlendType::SBT_TRANSPARENT_ALPHA);
		material->getTechnique(0)->getPass(0)->setShininess(Real(shininess));

		/*bool prog = material->getTechnique(0)->getPass(0)->isProgrammable();
		bool transp = material->getTechnique(0)->getPass(0)->isTransparent();*/

		if(specularColor.size()>=4)
			material->getTechnique(0)->getPass(0)->setSpecular(Real(specularColor.at(0)),Real(specularColor.at(1)),Real(specularColor.at(2)),Real(specularColor.at(3)));
		else material->getTechnique(0)->getPass(0)->setSpecular(Real(specularColor.at(0)),Real(specularColor.at(1)),Real(specularColor.at(2)),Real(alpha));

		material->getTechnique(0)->getPass(0)->setAmbient(Real(emissiveColor.at(0)),Real(emissiveColor.at(1)),Real(emissiveColor.at(2)));

		if(diffuseColor.size()>=4)
			material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor.at(0)),Real(diffuseColor.at(1)),Real(diffuseColor.at(2)),Real(diffuseColor.at(3)));
		else material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor.at(0)),Real(diffuseColor.at(1)),Real(diffuseColor.at(2)),Real(alpha));
		//material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor.at(0)),Real(diffuseColor[1]),Real(diffuseColor[2]),Real(alpha));

		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(true);
		material->getTechnique(0)->getPass(0)->setVertexColourTracking(TVC_AMBIENT);
		material->setCullingMode(CullingMode::CULL_NONE);
		return materialId;
	}
}
void OgreWrapper::setMaterial(const string &nodeId, const float ambientIntensity, const float shininess, const float alpha, const std::vector<float> diffuseColor, const std::vector<float> emissiveColor, const std::vector<float> specularColor){
	//void OgreWrapper::setMaterial(string nodeId, float ambientIntensity, float shininess, float alpha, float * diffuseColor, float * emissiveColor, float * specularColor ){

	MaterialPtr  material = MaterialManager::getSingleton().getByName(nodeId);

	if(shininess!=-1) material->getTechnique(0)->getPass(0)->setShininess(Real(shininess));
	if(alpha!=-1 && !specularColor.empty()){
		//material->getTechnique(0)->getPass(0)->setSpecular(Real(specularColor[0]),Real(specularColor[1]),Real(specularColor[2]),Real(alpha));
		if(specularColor.size()>=4)
			material->getTechnique(0)->getPass(0)->setSpecular(Real(specularColor.at(0)),Real(specularColor.at(1)),Real(specularColor.at(2)),Real(specularColor.at(3)));
		else material->getTechnique(0)->getPass(0)->setSpecular(Real(specularColor.at(0)),Real(specularColor.at(1)),Real(specularColor.at(2)),Real(alpha));
	}
	else if(alpha!=-1){
		Ogre::ColourValue  cValue =material->getTechnique(0)->getPass(0)->getSpecular();
		cValue.a = alpha;
		material->getTechnique(0)->getPass(0)->setSpecular(cValue);
	}
	else if(!specularColor.empty()){
		Ogre::ColourValue cValue = material->getTechnique(0)->getPass(0)->getSpecular();
		cValue.r = Real(specularColor.at(0));
		cValue.g = Real(specularColor.at(1));
		cValue.b = Real(specularColor.at(2));
		if(specularColor.size()>=4) cValue.a = Real(specularColor.at(3));
		material->getTechnique(0)->getPass(0)->setSpecular(cValue);
	}
	if(!diffuseColor.empty()){
		//material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor[0]),Real(diffuseColor[1]),Real(diffuseColor[2]),Real(alpha));
		if(diffuseColor.size()>=4)
			material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor.at(0)),Real(diffuseColor.at(1)),Real(diffuseColor.at(2)),Real(diffuseColor.at(3)));
		else material->getTechnique(0)->getPass(0)->setDiffuse(Real(diffuseColor.at(0)),Real(diffuseColor.at(1)),Real(diffuseColor.at(2)),Real(alpha));
	}
	if(!emissiveColor.empty()){
		material->getTechnique(0)->getPass(0)->setAmbient(Real(emissiveColor.at(0)),Real(emissiveColor.at(1)),Real(emissiveColor.at(2)));
	}
}

string OgreWrapper::createTexture(const string &url, const string &textureId ){

	size_t lastSlash = url.find_last_of("\\");
	Ogre::String mResourceFolder = url.substr(0,lastSlash+1);
	Ogre::String fileName = url.substr(lastSlash+1);

	/* Prefered method: resourceLocationExists does not exist in the version of ogre currently used */
	/*bool resExists = ResourceGroupManager::getSingleton().resourceLocationExists(mResourceFloder,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);*/
	/*if(!resExists) ResourceGroupManager::getSingleton().addResourceLocation(mResourceFolder,"FileSystem"); */ 
	/* Alternative method */
	ResourceGroupManager::getSingleton().removeResourceLocation(mResourceFolder);
	ResourceGroupManager::getSingleton().addResourceLocation(mResourceFolder,"FileSystem");

	MaterialPtr  material = MaterialManager::getSingleton().getByName(curMaterialName);
	if(material.isNull()){
		this->curMaterialName = textureId;
		material = MaterialManager::getSingleton().create( curMaterialName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}
	
	Image img;
	try {
		Ogre::TexturePtr mTex = Ogre::TextureManager::getSingleton().load(url,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		material->getTechnique(0)->getPass(0)->createTextureUnitState()->setTextureName(mTex->getName());
		material->getTechnique(0)->getPass(0)->setSceneBlending(SceneBlendType::SBT_REPLACE);
	}
	catch(Ogre::FileNotFoundException e){
		printf("OgreWrapper => An exception has occured: ",e.what());
		return string();
	}
	return textureId;
}

void OgreWrapper::attachShapeToSceneGraph( const string &shapeId, const bool show, const bool cascade){

	string objName;

	try{
		if(this->cRoot){
			if(curSceneNode->getParent()==NULL) createSceneNode(shapeId);
			if(curEntity){
				curEntity->setCastShadows(true);
				this->curEntity->setMaterialName(curMaterialName);
				this->curSceneNode->attachObject(this->curEntity);
				objName=this->curEntity->getName();
			}
			else if(curManualObject){
				curManualObject->setCastShadows(true);
				this->curManualObject->setMaterialName(0,curMaterialName);
				this->curSceneNode->attachObject(this->curManualObject);
				objName=this->curManualObject->getName();
			}

			this->curSceneNode->getAttachedObject(objName)->setVisible(show);
			if(cascade) this->curSceneNode->setVisible(show);
			/**
			si le mesh courant est une sphere, on a crée un nouveau noeud scene afin de lui appliquer un scale
			apres ajout de la sphere on remonte donc au noeud parent sinon le scale sera appliqué à tous les enfants
			*/
			if(meshIsGeneric) this->curSceneNode = curSceneNode->getParentSceneNode();
			clearPrivateDatas();
			//cSceneMgr->setDisplaySceneNodes(true);
		}
	}
	catch ( Ogre::Exception e ){
		printf("OgreWrapper => An exception has occured: ",e.what());
	}
}

string OgreWrapper::createSceneNode(const string nodeId){

	string res =  string();
	if(!this->cSceneMgr->hasSceneNode(nodeId)){
		if(this->cRoot){
			try {
				this->curSceneNode = this->curSceneNode->createChildSceneNode(nodeId);
				res=nodeId;
			}
			catch( Ogre::Exception e ) { 
				fprintf(stderr, "An exception has occurred: %s\n", e.what());
			}
		}
	}
	else {
		fprintf(stderr, "A SceneNode named %s already exists \n", nodeId.c_str() );
	}
	return res;
}

void OgreWrapper::closeNode (const string &nodeId ){

	if(this->cRoot){
		string curNodeName = curSceneNode->getName();
		if(curSceneNode->getParent()==NULL) return;
		else if(curNodeName==nodeId) this->curSceneNode = this->curSceneNode->getParentSceneNode();
	}
}

//void OgreWrapper::setNodeVisibility ( const string &nodeId, const bool isVisible ){
//
//	SceneNode * sNode;
//	if( this->cSceneMgr->hasSceneNode(nodeId) ){
//		sNode = this->cSceneMgr->getSceneNode(nodeId);
//		sNode->setVisible(isVisible);
//	}
//}

void OgreWrapper::showNode (const string &nodeId ){

	SceneNode * sNode;
	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		sNode = this->cSceneMgr->getSceneNode(nodeId);
		sNode->setVisible(true);
	}
}

void OgreWrapper::hideNode (const string &nodeId ){

	SceneNode * sNode;
	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		sNode = this->cSceneMgr->getSceneNode(nodeId);
		sNode->setVisible(false);
	}
}

std::vector<string> OgreWrapper::deleteNodeChildren (const string &nodeId ){

	std::vector<string> res = std::vector<string>() ;
	_deletedNodesArray.clear();

	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		try { 
			SceneNode * sNode = this->cSceneMgr->getSceneNode(nodeId);
			destroyAllAttachedMovableObjects( sNode );
			this->curSceneNode = sNode;
		}
		catch( Ogre::Exception e ) {fprintf(stderr, "An exception has occurred: %s\n", e.what());}
	}
	return _deletedNodesArray;
}

std::vector<string> OgreWrapper::deleteNode ( const string &nodeId ){

	std::vector<string> res = std::vector<string>() ;
	_deletedNodesArray.clear();

	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		try { 
			SceneNode * sNode = this->cSceneMgr->getSceneNode(nodeId);
			destroyAllAttachedMovableObjects( sNode );
			this->curSceneNode = sNode->getParentSceneNode();
			_deletedNodesArray.push_back(sNode->getName());
			sNode->getCreator()->destroySceneNode(sNode);
		}
		catch( Ogre::Exception e ) {fprintf(stderr, "An exception has occurred: %s\n", e.what());}
	}
	return _deletedNodesArray;
}

void OgreWrapper::destroyAllAttachedMovableObjects( SceneNode* pSceneNode ){

	// Destroy all the attached objects
	SceneNode::ObjectIterator itObject = pSceneNode->getAttachedObjectIterator();

	while ( itObject.hasMoreElements() ){
		MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
		_deletedNodesArray.push_back(pObject->getName());
		if(pObject->getMovableType()==Ogre::ManualObjectFactory::FACTORY_TYPE_NAME) pSceneNode->getCreator()->destroyManualObject(static_cast<Ogre::ManualObject*>(pObject));
		else  pSceneNode->getCreator()->destroyMovableObject( pObject );
	}

	// Recurse to child SceneNodes
	SceneNode::ChildNodeIterator itChild = pSceneNode->getChildIterator();

	while ( itChild.hasMoreElements() ){
		SceneNode* pChildNode = static_cast<SceneNode*>(itChild.getNext());
		_deletedNodesArray.push_back(pChildNode->getName());
		destroyAllAttachedMovableObjects( pChildNode );
	}
	pSceneNode->removeAndDestroyAllChildren();
}

void OgreWrapper::getNodeTransform( const string &nodeId, std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation ){

	SceneNode * sNode;
	if(!nodeId.empty()){
	if( this->cSceneMgr->hasSceneNode(nodeId) ){
		sNode = this->cSceneMgr->getSceneNode(nodeId);
		sNode->_update(true,true);
	}
	else if (this->cSceneMgr->hasEntity(nodeId)){
		Entity * ent = this->cSceneMgr->getEntity(nodeId);
		sNode = ent->getParentSceneNode();
	}
	else if (this->cSceneMgr->hasManualObject(nodeId)){
		ManualObject * mObj = this->cSceneMgr->getManualObject(nodeId);
		sNode = mObj->getParentSceneNode();
	}
	if(sNode){
		Vector3 pos = sNode->_getDerivedPosition();
		translation.push_back(pos.x);
		translation.push_back(pos.y);
		translation.push_back(pos.z);

		Vector3 vScale = sNode->_getDerivedScale();
		scale.push_back(vScale.x);
		scale.push_back(vScale.y);
		scale.push_back(vScale.z);

		Quaternion vOrientation = sNode->_getDerivedOrientation();
		rotation.push_back(vOrientation.x);
		rotation.push_back(vOrientation.y);
		rotation.push_back(vOrientation.z);
		rotation.push_back(vOrientation.w);

	}
	}
}

void OgreWrapper::getCurrentTransform( std::vector<float> &translation, std::vector<float> &scale, std::vector<float> &rotation ){

	SceneNode * sNode = this->curSceneNode;
	sNode->_update(true,true);

	Vector3 pos = sNode->_getDerivedPosition();
	translation.push_back(pos.x);
	translation.push_back(pos.y);
	translation.push_back(pos.z);

	Vector3 vScale = sNode->_getDerivedScale();
	scale.push_back(vScale.x);
	scale.push_back(vScale.y);
	scale.push_back(vScale.z);

	Quaternion vOrientation = sNode->_getDerivedOrientation();
	rotation.push_back(vOrientation.x);
	rotation.push_back(vOrientation.y);
	rotation.push_back(vOrientation.z);
	rotation.push_back(vOrientation.w);
}


//void OgreWrapper::setAppearance(){
//	if(curEntity) this->curEntity->setMaterialName(curMaterialName);
//	else if(curManualObject) curManualObject->setMaterialName(0,curMaterialName);
//}

extern "C" OGRE_WRAPPER WrapperConfiguration *  queryPlugin(SGAKernel &wf){
	OgreWrapper * oWrapper = new OgreWrapper();
	return oWrapper->getWrapperConfiguration();
}

extern "C" OGRE_WRAPPER size_t registerPlugin(SGAKernel &wf) {
	wf.getEngineWrapper().addWrapper(
		auto_ptr<EngineWrapperFactory::EngineWrapper> ( new OgreWrapper() )
		);
	return wf.getEngineWrapper().getWrapperCount()-1;
}
