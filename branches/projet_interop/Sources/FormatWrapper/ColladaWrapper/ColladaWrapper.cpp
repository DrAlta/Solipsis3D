// ColladaWrapper.cpp : Defines the exported functions for the DLL application.

#include "ColladaWrapper.h"

/**
return a string that is unique to a given node type
*/
string ColladaWrapper::getNodeName(string name, int type){

	string res = "";

	if(name!="") {
		res = name;
	}
	else{
		/// > transform
		if(type==0){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::TransformNode);
		}
		/// > geometry
		else if (type==1){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::GeometryNode);
		}
		/// > material
		else if (type==2){
			res = SGATools::getUniqueFormatNodeId(_nodeIdx,SGATools::NodeType::MaterialNode);
		}
	}
	_nodeIdx++;
	return res;
}

FCDGeometry * ColladaWrapper::findGeometryLibrary(string name,FCDocument* doc ){

	FCDGeometry * res = NULL;
	int j;

	FCDGeometryLibrary * geolib = doc->GetGeometryLibrary();
	size_t geolibSize = geolib->GetEntityCount();

	for (j=0; j<(int)geolibSize; j++) {
		if ( geolib->GetEntity(j)->GetDaeId().c_str() == name ) return geolib->GetEntity(j);
	}
	return res;
}

FCDMaterial * ColladaWrapper::findMaterialLibrary(string name,FCDocument* doc ){

	FCDMaterial * res = NULL;
	int i;

	FCDMaterialLibrary * matLib = doc->GetMaterialLibrary();
	size_t matLibSize = matLib->GetEntityCount();

	for (i=0; i<(int)matLibSize; i++) {
		if( matLib->GetEntity(i)->GetDaeId().c_str() == name ) return matLib->GetEntity(i);
	}
	return res;
}

void ColladaWrapper::processMaterialInstance( FCDEntityInstance* instance,FCDocument* doc ){

	FCDEntity* entity;
	std::string name;

	entity=instance->GetEntity();
	name=entity->GetDaeId().c_str();
}

void ColladaWrapper::processGeometryInstance( FCDEntityInstance* instance, FCDSceneNode * parentNode ){

	FCDEntity* entity;
	int idxPolygon, idxMaterial;
	std::string name, meshName;
	string shapeIdx;

	entity=instance->GetEntity();
	if(entity){
		name=entity->GetDaeId().c_str();

		FCDGeometry * geom = findGeometryLibrary(name,_doc);
		FCDGeometryInstance* geometry_instance = dynamic_cast<FCDGeometryInstance*>(instance);

		FCDMaterialInstance* materialinstance;
		std::string material_semantic_polygon;
		std::string semantic_material_instance;
		std::string id_material;

		if( geom && geom->IsMesh() ){

			clearDatas();

			FCDGeometryMesh * mesh = geom->GetMesh();
			meshName = mesh->GetDaeId().c_str();
			FCDGeometryPolygons * ptr_polygons;

			int m_num_polygons = (int) mesh->GetPolygonsCount();

			if (!mesh->IsTriangles()){
				FCDGeometryPolygonsTools::Triangulate(mesh);
			}

			m_num_polygons = (int) mesh->GetPolygonsCount();

			for ( idxPolygon=0; idxPolygon<m_num_polygons; idxPolygon++ ) {

				clearDatas();
				ptr_polygons = mesh->GetPolygons(idxPolygon);

				FCDGeometryPolygonsInput * geometrypolygonsinput;

				/** VERTEX **/
				//int nbVertices = (int) geometrypolygonsinput->GetIndexCount();
				int z;
				uint32* indices;
				int idx ;
				float * p;
				int stride;

				FCDGeometrySource * position_source = ptr_polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::POSITION);

				geometrypolygonsinput = ptr_polygons->FindInput(FUDaeGeometryInput::POSITION);
				indices = geometrypolygonsinput->GetIndices();

				int nbVertices= (int) geometrypolygonsinput->GetIndexCount();

				if(position_source){
					for ( z=0; z<nbVertices; z++) {
						idx = (int) indices[z];
						p = &position_source->GetData()[idx*3];
						_vertices.push_back(p[0]);
						_vertices.push_back(p[1]);
						_vertices.push_back(p[2]);
						//_vertices.push_back(position_source->GetData()[z]);
					}
				}

				/** NORMALS **/
				FCDGeometrySource * normal_source = ptr_polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::NORMAL);

				if(normal_source){

					int nbIdxNormal = normal_source->GetDataCount();
					stride = normal_source->GetStride();
					geometrypolygonsinput = ptr_polygons->FindInput(FUDaeGeometryInput::NORMAL);
					indices = geometrypolygonsinput->GetIndices();

					for ( z=0; z<nbVertices; z++) {
						idx = (int) indices[z];
						p = &normal_source->GetData()[idx*stride];
						_normals.push_back(p[0]);
						_normals.push_back(p[1]);
						_normals.push_back(p[2]);
						//_normals.push_back(normal_source->GetData()[z]);
					}
				}

				/** TEXCOORD **/

				FCDGeometrySource * texCoord_source = ptr_polygons->GetParent()->FindSourceByType(FUDaeGeometryInput::TEXCOORD);


				if(texCoord_source){

					int nbTexCoord = texCoord_source->GetDataCount();
					geometrypolygonsinput = ptr_polygons->FindInput(FUDaeGeometryInput::TEXCOORD);
					indices = geometrypolygonsinput->GetIndices();
					stride = texCoord_source->GetStride();

					for ( z=0; z<nbVertices; z++) {
						idx = (int) indices[z];
						p=&texCoord_source->GetData()[idx*stride];
						_textureCoord.push_back(p[0]);
						_textureCoord.push_back(1-p[1]); // point of origin in Collada different than the one used for renderers so this trick is needed to render them correctly
						//if (stride==3) _textureCoord.push_back(p[2]); else m_ptrs_texcoords[i].z=0.0f;
					}
				}

				/** FACES **/
				for ( z=0; z<nbVertices; z++) {
					_faces.push_back(z);
				}

				//if (!mesh->IsTriangles()) FCDGeometryPolygonsTools::Triangulate(mesh);
				//FCDGeometryPolygonsTools::GenerateUniqueIndices(mesh,ptr_polygons);
				//geometrypolygonsinput = ptr_polygons->FindInput(FUDaeGeometryInput::POSITION);
				/*indices = geometrypolygonsinput->GetIndices(); 
				int nbIdx = (int) geometrypolygonsinput->GetIndexCount();
				for ( z=0; z<nbIdx; z++) {
				_faces.push_back(indices[z]);
				}*/

				/** MATERIAL **/
				/** un seul matériau géré pour l'instant **/
				//for (idxMaterial=0; idxMaterial<(int) geometry_instance->GetMaterialInstanceCount(); idxMaterial++) {
				size_t nbMaterial = geometry_instance->GetMaterialInstanceCount();

				if(nbMaterial>0){


					idxMaterial=0;
					materialinstance = geometry_instance->GetMaterialInstance(idxMaterial);

					id_material = materialinstance->GetMaterial()->GetDaeId().c_str();
					FCDMaterial * mat = findMaterialLibrary(id_material,_doc);

					setDefaultMaterial();

					if (mat!=NULL) {
						FCDEffect* fx = mat->GetEffect();
						FCDEffectProfile* profile = fx->FindProfile(FUDaeProfileType::COMMON);
						FCDEffectStandard* standardProfile=dynamic_cast<FCDEffectStandard*>(profile);

						_alpha = standardProfile->GetTranslucencyFactor();

						_emissiveColor.at(0) = standardProfile->GetEmissionColor().x;
						_emissiveColor.at(1) = standardProfile->GetEmissionColor().y;
						_emissiveColor.at(2) = standardProfile->GetEmissionColor().z;
						_emissiveColor.at(3) = standardProfile->GetEmissionColor().w;

						_diffuseColor.at(0) = standardProfile->GetDiffuseColor().x;
						_diffuseColor.at(1) = standardProfile->GetDiffuseColor().y;
						_diffuseColor.at(2) = standardProfile->GetDiffuseColor().z;
						_diffuseColor.at(3) = standardProfile->GetDiffuseColor().w;

						_specularColor.at(0) = standardProfile->GetSpecularColor().x;
						_specularColor.at(1) = standardProfile->GetSpecularColor().y;
						_specularColor.at(2) = standardProfile->GetSpecularColor().z;
						_specularColor.at(3) = standardProfile->GetSpecularColor().w;


						_shininess = standardProfile->GetShininess();

						string materialIdx = ColladaWrapper::getNodeName(id_material,2);
						_kernel->createMaterialProperty(materialIdx,_colladaFileUrl,_ambientIntensity,_shininess,_alpha,_diffuseColor,_emissiveColor,_specularColor);

						// textures
						FCDTexture* texture;
						FCDImage *image;

						// diffuse texture
						if (standardProfile->GetTextureCount(FUDaeTextureChannel::DIFFUSE)>0) {
							texture=standardProfile->GetTexture(FUDaeTextureChannel::DIFFUSE,0);
							if (texture!=NULL) {
								image=texture->GetImage();
								if (image!=NULL) {
									wchar_t orig[512];
									swprintf(orig, L"%s", image->GetFilename().c_str() );
									size_t origsize = wcslen(orig) + 1;
									const size_t newsize = 256;
									size_t convertedChars = 0;
									char nstring[newsize];
									wcstombs_s(&convertedChars, nstring, origsize, orig, _TRUNCATE);
									string sImageUrl = string(nstring);;
									_kernel->createTextureProperty(materialIdx,_colladaFileUrl,sImageUrl);
									/*m_texture_diffuse=m_main->SearchTextureByName(image->GetDaeId().c_str());
									if (m_texture_diffuse!=NULL)
									if (m_texture_diffuse->GetTextureId()!=-1)
									m_has_diffuse_texture=true;*/
								}
							}
						}	

						// reflective texture
						//m_has_reflective_texture=false;
						//CCLTexture* m_texture_reflective=NULL;
						//image=NULL;

						//// reflective texture
						//if (standardProfile->GetTextureCount(FUDaeTextureChannel::REFLECTION)>0) {
						//	//float r=standardProfile->GetReflectivityFactor();
						//	//float r=standardProfile->GetReflectivity();
						//	//float r2=standardProfile->GetReflectivityFactor();
						//	texture=standardProfile->GetTexture(FUDaeTextureChannel::REFLECTION,0);
						//	if (texture!=NULL) {
						//		image=texture->GetImage();
						//		if (image!=NULL) {
						//			m_texture_reflective=m_main->SearchTextureByName(image->GetDaeId().c_str());
						//			if (m_texture_reflective!=NULL)
						//				if (m_texture_reflective->GetTextureId()!=-1)
						//					m_has_reflective_texture=true;
						//		}
						//	}
						//}

						//// transparent textures
						//bool m_has_transparent_texture=false;
						//CCLTexture* m_texture_transparent=NULL;
						//image=NULL;

						//// transparent texture
						//if (standardProfile->GetTextureCount(FUDaeTextureChannel::TRANSPARENT)>0) {
						//	texture=standardProfile->GetTexture(FUDaeTextureChannel::TRANSPARENT,0);
						//	if (texture!=NULL) {
						//		image=texture->GetImage();
						//		if (image!=NULL) {
						//			m_texture_transparent=m_main->SearchTextureByName(image->GetDaeId());
						//			if (m_texture_transparent!=NULL)
						//				if (m_texture_transparent->GetTextureId()!=-1)
						//					m_has_transparent_texture=true;
						//		}
						//	}
						//}

					}
					//}//material
				}

				string pName;
				if(idxPolygon>0){
					std::stringstream out;
					out << idxPolygon;
					pName = name + "_" + out.str();
				}
				else pName = name;

				shapeIdx = ColladaWrapper::getNodeName(pName,1);
				string res = _kernel->createMesh(shapeIdx,_colladaFileUrl,_vertices,_normals,_textureCoord,_colours,_faces);

				if(!res.empty()){
					_kernel->attachShapeToCurrentGroupNode(shapeIdx,_colladaFileUrl);
				}
			}//for
		}//isMesh
		else if ( geom->IsSpline()){
			/**TODO*/
		}
		else if ( geom->IsPSurface()){
			/**TODO*/
		}
		string sName = instance->GetParent()->GetDaeId().c_str();
	}
}




void ColladaWrapper::processInstance(FCDSceneNode* node, FCDocument* doc ){

	FCDEntityInstance* instance;
	//FCDEntity* entity;
	std::string name;
	int iInstance, nbInstance;

	nbInstance = (int)node->GetInstanceCount();

	bool flag_found;
	for (iInstance=0; iInstance<nbInstance; iInstance++) {

		flag_found=false;

		instance=node->GetInstance(iInstance);
		/*entity=instance->GetEntity();
		name=entity->GetDaeId().c_str();*/

		/*the current instance is a geometry */
		if(instance->GetType()==FCDEntityInstance::GEOMETRY){
			processGeometryInstance(instance,node);

		}
	}
}

//void ColladaWrapper::processTransform(FCDSceneNode* node){
string ColladaWrapper::processTransform(FCDSceneNode* node, FCDocument* doc){


	int iTrans;
	FCDTransform * transform;
	ostringstream oss;

	string nodeName =  node->GetDaeId().c_str();

	vector<float> vTranslation(3,0.0);
	vector<float> vScale(3,1.0);
	vector<float> vRotation(4,0.0);
	vRotation.at(3)=1.0;

	getTransformValues(node,vTranslation,vScale,vRotation);

	_kernel->createTransformNode(nodeName,_colladaFileUrl,vTranslation,vScale,vRotation);
	return nodeName;
}

void ColladaWrapper::getTransformValues(FCDSceneNode* node, vector<float> &vTranslation, vector<float> &vScale, vector<float> &vRotation){


	int iTrans;
	FCDTransform * transform;

	int nbTransform = node->GetTransformCount();
	for(iTrans=0; iTrans<nbTransform; iTrans++){

		transform = node->GetTransform(iTrans);

		if (transform->GetType()== FCDTransform::ROTATION) {
			///@todo: ne prend ps correctement en compte la rotation (cf les exemples de fichiers Collada)
			FCDTRotation* trans_rot=dynamic_cast<FCDTRotation*>(transform); 
			float rot_angle = trans_rot->GetAngle();
			if(rot_angle!=0){
				FMVector3 rot_vector = trans_rot->GetAxis();
				vRotation.clear();
				vRotation.push_back(rot_vector.x*sin(rot_angle/2));
				vRotation.push_back(rot_vector.y*sin(rot_angle/2));
				vRotation.push_back(rot_vector.z*sin(rot_angle/2));
				vRotation.push_back(cos(rot_angle/2));
			}
		}
		else if (transform->GetType()== FCDTransform::TRANSLATION) {
			FCDTTranslation* trans_trans=dynamic_cast<FCDTTranslation*>(transform);
			vTranslation.at(0)+=trans_trans->GetTranslation()->x;
			vTranslation.at(1)+=trans_trans->GetTranslation()->y;
			vTranslation.at(2)+=trans_trans->GetTranslation()->z;
		}
		else if (transform->GetType()== FCDTransform::SCALE) {
			FCDTScale* trans_scale=dynamic_cast<FCDTScale*>(transform);
			vScale.at(0)+=trans_scale->GetScale()->x;
			vScale.at(1)+=trans_scale->GetScale()->y;
			vScale.at(2)+=trans_scale->GetScale()->z;
		}
		else if (transform->GetType()== FCDTransform::MATRIX) {
			FCDTMatrix* trans_matrix=dynamic_cast<FCDTMatrix*>(transform);
			FMVector3 fmScale,fmRotation,fmTranslation;
			float invert;
			trans_matrix->ToMatrix().Decompose(fmScale,fmRotation,fmTranslation,invert);

			vTranslation.at(0)+=fmTranslation.x;
			vTranslation.at(1)+=fmTranslation.y;
			vTranslation.at(2)+=fmTranslation.z;

			vScale.at(0)+=fmScale.x;
			vScale.at(1)+=fmScale.y;
			vScale.at(2)+=fmScale.z;

			//Rotation .... quel angle ?
			vRotation.push_back(fmRotation.x*sin(invert/2));
			vRotation.push_back(fmRotation.y*sin(invert/2));
			vRotation.push_back(fmRotation.z*sin(invert/2));
			vRotation.push_back(cos(invert/2));
		}
	}
}

void ColladaWrapper::processDoc(FCDocument* doc){

	_upAxis =_doc->GetAsset()->GetUpAxis();
	FCDocumentTools::StandardizeUpAxisAndLength(_doc,FMVector3::YAxis);

	FCDSceneNode* ptr_root=doc->GetVisualSceneRoot();
	if(ptr_root==NULL){
		printf("ColladaWrapper => The current scene is empty in Collada file \"%s\" \n",_colladaFileUrl.c_str());
		return;
	}

	// Visual scene -------------------------------------------------------
	FCDVisualSceneNodeLibrary* visualScene = doc->GetVisualSceneLibrary();

	int nbEntity = visualScene->GetEntityCount();
	int iEntity,iNode;
	for(iEntity = 0; iEntity<nbEntity; iEntity++){
		FCDSceneNode * sceneNode = visualScene->GetEntity(iEntity);
		if(sceneNode){
			int nbChildren = sceneNode->GetChildrenCount();
			for(iNode=0; iNode<nbChildren; iNode++){
				processVisualSceneNode( sceneNode->GetChild(iNode) );
			}
		}
	}
	// Physics scene -------------------------------------------------------
	processPhysicsScene ( _doc->GetPhysicsSceneLibrary() );
	
}

void ColladaWrapper::processVisualSceneNode ( FCDSceneNode * sNode ){

	if(sNode){
		string nodeId = processTransform(sNode,_doc);
		processInstance(sNode,_doc);
		int nbChildren = sNode->GetChildrenCount();
		if(  nbChildren!= 0 ){
			for( int i=0; i<nbChildren; i++) processVisualSceneNode( sNode->GetChild(i) );
		}
		_kernel->closeGroupNode( nodeId, _colladaFileUrl );
	}
}

void ColladaWrapper::processPhysicsScene ( FCDPhysicsSceneLibrary * physicsScene ){
	
	size_t nbEntity = physicsScene->GetEntityCount();
	FCDPhysicsScene * pScene;
	int modelInstanceIdx;

	for(int iEntity = 0; iEntity<nbEntity; iEntity++){
		
		pScene = physicsScene->GetEntity(iEntity);
		processPhysicsSceneProperties(pScene);
		
		size_t nbPhysicsModel = pScene->GetPhysicsModelInstancesCount();	 
		for(modelInstanceIdx=0; modelInstanceIdx<nbPhysicsModel; modelInstanceIdx++){
			processPhysicsModelInstance( pScene->GetPhysicsModelInstance(modelInstanceIdx) ); 
		}
	}

}

void ColladaWrapper::processPhysicsModelInstance ( FCDPhysicsModelInstance * pMInstance ){

	
	vector<float> vTranslation(3,0.0);
	vector<float> vScale(3,1.0);
	vector<float> vRotation(4,0.0);
	vRotation.at(3)=1.0;
	FCDSceneNode * targetNode;

	//__________________Physics Model_________________________________
	FCDPhysicsModel * pModel = _doc->FindPhysicsModel(pMInstance->GetEntity()->GetDaeId().c_str());
	FCDPhysicsRigidBody * pRigidBody = pModel->GetRigidBody(0);
	FCDParameterAnimatableFloat fMass = pRigidBody->GetParameters()->GetMass();
	bool isDynamic = pRigidBody->GetParameters()->IsDynamic();
	FCDPhysicsMaterial * pMat = pRigidBody->GetParameters()->GetPhysicsMaterial();
	float restitution = pMat->GetRestitution();
	float friction = pMat->GetDynamicFriction();


	string shapeIdx;
	int nbEntityInstance = pMInstance->GetInstanceCount();
	for( int idx = 0; idx<nbEntityInstance; idx++){
		
		FCDEntityInstance * rgInstance =  pMInstance->GetInstance(idx);
		if (rgInstance->GetType() == FCDEntityInstance::PHYSICS_RIGID_BODY ){
			FCDPhysicsRigidBodyInstance* myRBIns = dynamic_cast<FCDPhysicsRigidBodyInstance*>(rgInstance);
			targetNode = myRBIns->GetTargetNode();

			size_t tNodeInsNb =  targetNode->GetInstanceCount();
			size_t tNodeChildrenNb =  targetNode->GetChildrenCount();
			for( int iInstance = 0; iInstance<tNodeInsNb; iInstance++){
				FCDEntityInstance * tnInstance =  targetNode->GetInstance(iInstance);
				if(tnInstance->GetType()==FCDEntityInstance::GEOMETRY){
					shapeIdx = tnInstance->GetEntity()->GetDaeId().c_str();
					break;
				}
			}
			getTransformValues(targetNode,vTranslation,vScale,vRotation);
		}
	}
		 
		 //string modelName = pModelInstance->GetName().c_str();
	_kernel->setNodePhysicsProperties(shapeIdx,_colladaFileUrl,float(fMass),restitution,friction,isDynamic);
		 
}


void ColladaWrapper::processPhysicsSceneProperties( FCDPhysicsScene * pScene ){

	FMVector3 vGravity = pScene->GetGravity();
	std::vector<float> sGravity;

	if(_upAxis==FMVector3::XAxis) {
			sGravity.push_back(vGravity.y); sGravity.push_back(vGravity.x); sGravity.push_back(vGravity.z);
	}
	else if(_upAxis==FMVector3::YAxis){
		sGravity.push_back(vGravity.x); sGravity.push_back(vGravity.y); sGravity.push_back(vGravity.z);
	}
	else{
		sGravity.push_back(vGravity.x); sGravity.push_back(vGravity.z); sGravity.push_back(vGravity.y);
	}
	float timeStep = pScene->GetTimestep();
	_kernel->setWorldPhysicsProperties(sGravity,timeStep);
}

int ColladaWrapper::adaptFormat( const string & fileName, const string & filePath, SGAKernel * kernel){

	wchar_t * fileNameWC = (wchar_t *)malloc( (fileName.length() + 1) * sizeof( wchar_t ));
	int i = mbstowcs(fileNameWC,fileName.c_str(),sizeof(char*)*fileName.length());

	if(i!=0){
		FCollada::Initialize();
		_doc = FCollada::NewTopDocument();
		bool loaded = LoadDocumentFromFile(_doc, fileNameWC);
		//bool loaded = LoadDocumentFromFile(_doc, fileName);
		if(!loaded){
			printf("ColladaWrapper => Unable to load Collada file \"%s\" \n",fileName.c_str());
			return 0;
		}
		string szMeshPrefix = fileName.substr(0,fileName.length()-4);
		size_t lastSlash = szMeshPrefix.find_last_of("/\\");

		_colladaFileName = szMeshPrefix.substr(lastSlash+1);

		size_t lastSlashFile = fileName.find_last_of("/\\") + 1;
		size_t lastSlashPath = filePath.find_last_of("/\\") + 1;
		if( PathIsRelativeA(fileName.c_str())) _colladaFilePath = filePath.substr(0,lastSlashPath) + fileName.substr(0,lastSlashFile);
		else _colladaFilePath = fileName.substr(0,lastSlashFile);

		_kernel = kernel;
		if(_kernel) processDoc(_doc);

		//ColladaDOM loading
		
		_domRoot = _dae.open(fileName);
		if (!_domRoot) {
			cout << "ColladaWrapper => Document import failed.\n";
			return 0;
		}
		else{
			processExternalUris();
		}
	}
	else{
		printf("ColladaWrapper => Error in file name:  \"%s\" \n",fileName.c_str());
		return 0;
	}

	return 1;
}

void ColladaWrapper::processExternalUris(){

	int geomCount = _dae.getDatabase()->typeLookup<domInstance_geometry>().size();
	int cpt;

	for(cpt=0; cpt<geomCount; cpt++){

		domInstance_geometry* geomInst = _dae.getDatabase()->typeLookup<domInstance_geometry>().at(cpt);
		daeBool isExternal = geomInst->getUrl().isExternalReference();
		bool isExternalBool = isExternal;
		if(isExternalBool){
			string fileUri = geomInst->getUrl().pathDir() + geomInst->getUrl().authority();
			string fileUrl = _colladaFilePath + fileUri;
			_kernel->loadFile(fileUrl,_colladaFilePath);
		}
	}

}

FCDTransform * ColladaWrapper::getDerivedTrans ( FCDSceneNode* sceneNode, int iTrans ) {

	FCDSceneNode* curSceneNode;
	FCDTransform * curTrans;
	FCDTransform * res = sceneNode->GetTransform(iTrans)->Clone();
	size_t nbParent =  sceneNode->GetParentCount();

	for( int iParent=0; iParent<nbParent; iParent++){

		curSceneNode = sceneNode->GetParent(0);
		int nbTransform = curSceneNode->GetTransformCount();
		for( int iTransform=0; iTransform<nbTransform; iTransform++){

			curTrans = curSceneNode->GetTransform(iTransform);

			if (curTrans->GetType()== FCDTransform::TRANSLATION){
				FCDTTranslation* cur_trans=dynamic_cast<FCDTTranslation*>(curTrans);
				FCDTTranslation* res_trans=dynamic_cast<FCDTTranslation*>(res);

				res_trans->SetTranslation(
					cur_trans->GetTranslation()->x + res_trans->GetTranslation()->x,
					cur_trans->GetTranslation()->y + res_trans->GetTranslation()->y,
					cur_trans->GetTranslation()->z + res_trans->GetTranslation()->z );
			}
		}
	}
	return res;
}

void ColladaWrapper::setInstanceTransform( FCDSceneNode* sceneNode, const vector<float> translation, const std::vector<float> rotation ){

	FCDTransform * transform;
	FCDTransform * derivedTrans;

	int nbTransform = sceneNode->GetTransformCount();
	int iTrans;
	bool trans_setted = false;
	bool rot_setted = false;

	vector<float> vScale(3,1.0);
	string nodeName =  sceneNode->GetDaeId().c_str();
	string nodeId = ColladaWrapper::getNodeName(nodeName,0);

	for(iTrans=0; iTrans<nbTransform; iTrans++){

		transform = sceneNode->GetTransform(iTrans);

		if (transform->GetType()== FCDTransform::TRANSLATION && !trans_setted){
			derivedTrans = getDerivedTrans(sceneNode,iTrans);
			FCDTTranslation* trans_trans=dynamic_cast<FCDTTranslation*>(derivedTrans);
			trans_trans->SetTranslation(translation.at(0),translation.at(1),translation.at(2));
			trans_setted=true;
		}
		else if (transform->GetType()== FCDTransform::ROTATION && !rot_setted) {
			derivedTrans = getDerivedTrans(sceneNode,iTrans);
			FCDTRotation* trans_rot=dynamic_cast<FCDTRotation*>(derivedTrans); 
			trans_rot->SetRotation( FMVector3(rotation.at(0),rotation.at(1),rotation.at(2)),rotation.at(3));
			rot_setted=true;
		}
		else if (transform->GetType()== FCDTransform::SCALE) {
			FCDTScale* trans_scale=dynamic_cast<FCDTScale*>(transform);
			vScale.at(0)+=trans_scale->GetScale()->x;
			vScale.at(1)+=trans_scale->GetScale()->y;
			vScale.at(2)+=trans_scale->GetScale()->z;
		}
	}
	//_kernel->setTransformNode(nodeId,_colladaFileUrl,translation,rotation,vScale,SGAKernel::TransformSpace::WORLD);
	_kernel->setTransformNode(nodeId,_colladaFileUrl,translation,rotation,vScale);
}

FCDSceneNode * ColladaWrapper::findInstanceParentSceneNode( const string &nodeId ){

	FCDSceneNode * res;
	FCDSceneNode * sceneNode;
	FCDSceneNode * visualSceneNode;
	FCDVisualSceneNodeLibrary* visualScene = _doc->GetVisualSceneLibrary();

	int nbEntity = visualScene->GetEntityCount();
	int iEntity,iNode;

	for(iEntity = 0; iEntity<nbEntity; iEntity++){

		visualSceneNode = visualScene->GetEntity(iEntity);
		if(visualSceneNode){
			int nbChildren = visualSceneNode->GetChildrenCount();
			for(iNode=0; iNode<nbChildren; iNode++){

				sceneNode = visualSceneNode->GetChild(iNode);
				if(sceneNode){
					std::string instanceName;
					int iInstance, nbInstance;
					nbInstance = (int)sceneNode->GetInstanceCount();
					for (iInstance=0; iInstance<nbInstance; iInstance++) {

						instanceName = sceneNode->GetInstance(iInstance)->GetEntity()->GetDaeId().c_str();
						if( instanceName.compare(nodeId)==0 ) return sceneNode;
					}
				}
			}
		}
	}
	return res;
}

void ColladaWrapper::updateNodePosition( const string &nodeId, const vector<float> position, const std::vector<float> rotation ){


	FCDSceneNode* sceneNode;
	FCDVisualSceneNodeLibrary* visualScene = _doc->GetVisualSceneLibrary();
	FCDGeometry * geom = _doc->FindGeometry(fm::string(nodeId.c_str()) );

	if(geom){
		sceneNode = findInstanceParentSceneNode( nodeId );
		//sceneNode = visualScene->FindDaeId(fm::string(nodeId.c_str()));
		if( sceneNode ) setInstanceTransform(sceneNode,position,rotation);
	}
}

std::string ColladaWrapper::getNodeFieldValue ( const std::string &nodeId,  const string &fieldId ){

	std::string res = std::string();
	FCDVisualSceneNodeLibrary* visualScene = _doc->GetVisualSceneLibrary();
	FCDSceneNode* sceneNode;

	sceneNode = visualScene->FindDaeId(fm::string(nodeId.c_str()));

	int nbTransform = sceneNode->GetTransformCount();
	int iTrans;
	FCDTransform * transform;
	stringstream sStream;

	if(sceneNode){

		for(iTrans=0; iTrans<nbTransform; iTrans++){

			transform = sceneNode->GetTransform(iTrans);
			sStream.str("");
			if(fieldId.compare("translate")==0 && transform->GetType()== FCDTransform::TRANSLATION){
				FCDTTranslation* trans_trans=dynamic_cast<FCDTTranslation*>(transform);
				sStream << trans_trans->GetTranslation()->x;
				sStream << ' ' << trans_trans->GetTranslation()->y;
				sStream << ' ' << trans_trans->GetTranslation()->z;
			}
			else if(fieldId.compare("rotate")==0 && transform->GetType()== FCDTransform::ROTATION){
				FCDTRotation * rot_trans=dynamic_cast<FCDTRotation*>(transform);
				sStream << rot_trans->GetAxis().x;
				sStream << ' ' << rot_trans->GetAxis().y;
				sStream << ' ' << rot_trans->GetAxis().z;
				sStream << ' ' << rot_trans->GetAngle();
			}
			else if(fieldId.compare("scale")==0 && transform->GetType()== FCDTransform::SCALE){
				FCDTScale * scale_trans=dynamic_cast<FCDTScale*>(transform);
				sStream << scale_trans->GetScale()->x;
				sStream << ' ' << scale_trans->GetScale()->y;
				sStream << ' ' << scale_trans->GetScale()->z;
			}
		}
		res = sStream.str();
		if(res.empty()){
			if(fieldId.compare("translate")==0) res = "0 0 0";
			else if(fieldId.compare("rotate")==0) res = "0 0 0 0";
			else if(fieldId.compare("scale")==0) res = "1 1 1";		
		}
	}
	return res;
}

void ColladaWrapper::setNodeFieldValue( const std::string &nodeId, const string &fieldId, const string &sValue ){

	//daeElement* nodeVS = _domRoot->getDescendant("node");
	//
	//daeElement* node = nodeVS->getChild(nodeId.c_str());
	//if(node){
	//	//node->setAttribute(daeString(fieldId),daeString(sValue));
	//	std::string sTrans = node->getAttribute("translate");
	//	std::string sRot = node->getAttribute("rotate");
	//	std::string sScale = node->getAttribute("scale");
	//	
	//	vector<float> vScale(3,1.0);
	//	vector<float> vTranslation(3,0.0);
	//	vector<float> vRotation (4,0.0);

	//	_kernel->setTransformNode(nodeId,_colladaFileUrl,vTranslation,vRotation,vScale);
	//}

	FCDVisualSceneNodeLibrary* visualScene = _doc->GetVisualSceneLibrary();
	FCDSceneNode* sceneNode = visualScene->FindDaeId(fm::string(nodeId.c_str()));

	vector<float> vValues = ColladaWrapperTools::stringToFloatArray(sValue);

	vector<float> vScale(3,1.0);
	vector<float> vTranslation(3,0.0);
	vector<float> vRotation (4,0.0);

	bool isSet = false;

	if(sceneNode && !vValues.empty() ){
		int nbTransform = sceneNode->GetTransformCount();
		int iTrans;
		FCDTransform * transform;

		for(iTrans=0; iTrans<nbTransform; iTrans++){

			transform = sceneNode->GetTransform(iTrans);
			if( transform->GetType()== FCDTransform::TRANSLATION ){
				FCDTTranslation* trans_trans=dynamic_cast<FCDTTranslation*>(transform);
				if( fieldId.compare("translate")==0 ){
					trans_trans->SetTranslation(vValues.at(0),vValues.at(1),vValues.at(2));
					isSet=true;
				}
				vTranslation.clear();
				vTranslation.push_back( trans_trans->GetTranslation()->x);
				vTranslation.push_back( trans_trans->GetTranslation()->y);
				vTranslation.push_back( trans_trans->GetTranslation()->z);

			}
			else if( transform->GetType()== FCDTransform::ROTATION ){
				FCDTRotation * rot_trans=dynamic_cast<FCDTRotation*>(transform);
				if( fieldId.compare("rotate")==0 ){
					rot_trans->SetRotation(FMVector3(vValues.at(0),vValues.at(1),vValues.at(2)),vValues.at(3));
					isSet=true;
				}
				vRotation.clear();
				vRotation.push_back( rot_trans->GetAxis().x);
				vRotation.push_back( rot_trans->GetAxis().y);
				vRotation.push_back( rot_trans->GetAxis().z);
				vRotation.push_back( rot_trans->GetAngle() );

			}
			else if( transform->GetType()== FCDTransform::SCALE ){
				FCDTScale * scale_trans=dynamic_cast<FCDTScale*>(transform);
				if( fieldId.compare("scale")==0 ){
					scale_trans->SetScale(vValues.at(0),vValues.at(1),vValues.at(2));
					isSet=true;
				}
				vScale.clear();
				vScale.push_back( scale_trans->GetScale()->x);
				vScale.push_back( scale_trans->GetScale()->y);
				vScale.push_back( scale_trans->GetScale()->z);

			}
		}
	}
	if(!isSet){
		FCDTransform * trans;
		if( fieldId.compare("translate")==0 ){ 
			trans = sceneNode->AddTransform(FCDTransform::TRANSLATION);
			FCDTTranslation* trans_trans=dynamic_cast<FCDTTranslation*>(trans);
			trans_trans->SetTranslation(vValues.at(0),vValues.at(1),vValues.at(2));
		}
		else if( fieldId.compare("rotate")==0 ){
			trans = sceneNode->AddTransform(FCDTransform::ROTATION);
			FCDTRotation* trans_rot=dynamic_cast<FCDTRotation*>(trans);
			trans_rot->SetRotation(FMVector3(vValues.at(0),vValues.at(1),vValues.at(2)),vValues.at(3));
		}
		else if( fieldId.compare("scale")==0 ){
			trans = sceneNode->AddTransform(FCDTransform::SCALE);
			FCDTScale* trans_scale=dynamic_cast<FCDTScale*>(trans);
			trans_scale->SetScale(vValues.at(0),vValues.at(1),vValues.at(2));
		}
			
	}
	_kernel->setTransformNode(nodeId,_colladaFileUrl,vTranslation,vRotation,vScale);
}

void ColladaWrapper::matchNodeFieldValue( const std::string &fromNodeId, const std::string &toNodeId, const std::string &toNodeFileName, const std::vector<std::string> fields ){

	FCDVisualSceneNodeLibrary* visualScene = _doc->GetVisualSceneLibrary();
	FCDSceneNode* sceneNode;
	sceneNode	= visualScene->FindDaeId(fm::string(fromNodeId.c_str()));
	if(!sceneNode){
		FCDEntity * entity = _doc->FindEntity(fm::string(fromNodeId.c_str()));
		if(entity) sceneNode = findInstanceParentSceneNode( fromNodeId );
	}

	int nbFields = fields.size();
	for ( int i=0; i<nbFields; i++ )
		processMatch( sceneNode, toNodeId, toNodeFileName,fields.at(i));
}

void ColladaWrapper::processMatch( FCDSceneNode * sceneNode, const std::string &toNodeId, const std::string &toNodeFileName, const std::string &field){

	if(sceneNode){
		string sceneNodeName = sceneNode->GetDaeId().c_str();
		string pModelId;

		if(field.compare("mass")==0 || field.compare("dynamic")==0 || field.compare("dynamic_friction")==0 || field.compare("restitution")==0 || field.compare("static_friction")==0){

			int pModelCount = _dae.getDatabase()->typeLookup<domInstance_physics_model>().size();
			int cpt;

			for(cpt=0; cpt<pModelCount; cpt++){

				domInstance_physics_model * pModelInst = _dae.getDatabase()->typeLookup<domInstance_physics_model>().at(cpt);
				size_t nbRBInst =  pModelInst->getChildrenByType<domInstance_rigid_body>().getCount();
				pModelId = pModelInst->getUrl().getID();

				for(int i=0; i<nbRBInst; i++){
					domInstance_rigid_body * rBInst = pModelInst->getChildrenByType<domInstance_rigid_body>().get(i);

					daeString targetId = rBInst->getTarget().getID();
					if(sceneNodeName.compare(targetId)==0){
						FCDPhysicsModel * pModel = _doc->FindPhysicsModel(fm::string(pModelId.c_str()));

						if(field.compare("mass")==0){
							float fMass = pModel->GetRigidBody(0)->GetParameters()->GetMass();
							_kernel->setNodePhysicsProperties(toNodeId,toNodeFileName,fMass);
						}
						else if(field.compare("dynamic")==0){
							bool isDynamic = pModel->GetRigidBody(0)->GetParameters()->IsDynamic();
							_kernel->setNodePhysicsProperties(toNodeId,toNodeFileName,-1,-1,-1,isDynamic);
						}
						else{
							FCDPhysicsMaterial * pMat = pModel->GetRigidBody(0)->GetParameters()->GetPhysicsMaterial();
							if(field.compare("dynamic_friction")==0){
								float fDFriction = pMat->GetDynamicFriction();
								_kernel->setNodePhysicsProperties(toNodeId,toNodeFileName,-1,-1,fDFriction);

							}
							else if(field.compare("static_friction")==0){
								float fSFriction = pMat->GetStaticFriction();
								_kernel->setNodePhysicsProperties(toNodeId,toNodeFileName,-1,-1,fSFriction);

							}
							else if(field.compare("restitution")==0){
								float fRestitution = pMat->GetRestitution();
								_kernel->setNodePhysicsProperties(toNodeId,toNodeFileName,-1,fRestitution);

							}
						}
					}
				}
			}
		}
		if(field.compare("dynamic_friction")==0 || field.compare("restitution")==0 || field.compare("static_friction")==0){

		}
	}

}


/// Tells us to register our functionality to an engine kernel
extern "C" COLLADA_WRAPPER_API size_t registerPlugin(SGAKernel &wf) {
	wf.getFormatWrappers().addWrapper(
		auto_ptr<FormatWrapperFactory::FormatWrapper> ( new ColladaWrapper(wf.getCurrentFileName()) )
		);
	return wf.getFormatWrappers().getWrapperCount()-1;
}
/** retrieve the format manages by the plugin 
* @return a string 
*/
extern "C" COLLADA_WRAPPER_API WrapperConfiguration *  queryPlugin(SGAKernel &wf){
	ColladaWrapper * cWrapper = new ColladaWrapper();
	return cWrapper->getWrapperConfiguration();
}