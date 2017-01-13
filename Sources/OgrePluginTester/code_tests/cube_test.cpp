void Plugin_x3d::getBoxData(CyberX3D::SceneGraph *sg,CyberX3D::BoxNode * bNode){

	float sizeA [3];

	bNode->getSize(sizeA);

	float coordX = sizeA[0];
	float coordY = sizeA[1];
	float coordZ = sizeA[2];

	size.push_back(coordX);
	size.push_back(coordY);
	size.push_back(coordZ);
}

void Plugin_x3d::renderBoxInOgre(){


	MaterialPtr  material = MaterialManager::getSingleton().create( "Test/ColourBoxTest", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_AMBIENT);
	material->setDiffuse(Real(1),Real(0),Real(0),Real(0.5));

	size_t l = -size.at(0)/2;
	size_t t = size.at(1)/2;
	size_t r = size.at(0)/2;
	size_t b = -size.at(1)/2;

	ManualObject* manual = cSceneMgr->createManualObject("manual");
	manual->begin("Test/ColourBoxTest", RenderOperation::OT_TRIANGLE_LIST);
	 
	manual->position(-r, -l, 0.0);
	manual->position( r, -l, 0.0);
	manual->position( r,  l, 0.0);
	manual->position(-r,  l, 0.0);

	 
	manual->end();

	curSceneNode->attachObject(manual);

}

ManualObject* createCubeMesh(Ogre::String name, Ogre::String matName) {

   ManualObject* cube = new ManualObject(name);
   cube->begin(matName);
   
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,1);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,1);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,0);
   cube->position(0.5,0.5,1.0);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(0,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(1,1);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(-0.5,-0.5,1.0);cube->normal(-0.666667,-0.333333,0.666667);cube->textureCoord(1,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(0,1);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(1,1);
   cube->position(0.5,-0.5,1.0);cube->normal(0.408248,-0.816497,0.408248);cube->textureCoord(0,0);
   cube->position(0.5,-0.5,0.0);cube->normal(0.666667,-0.333333,-0.666667);cube->textureCoord(1,0);
   cube->position(-0.5,-0.5,0.0);cube->normal(-0.408248,-0.816497,-0.408248);cube->textureCoord(0,0);
   cube->position(-0.5,0.5,1.0);cube->normal(-0.408248,0.816497,0.408248);cube->textureCoord(1,0);
   cube->position(0.5,0.5,0.0);cube->normal(0.408248,0.816497,-0.408248);cube->textureCoord(0,1);
   cube->position(-0.5,0.5,0.0);cube->normal(-0.666667,0.333333,-0.666667);cube->textureCoord(1,1);
   cube->position(0.5,0.5,1.0);cube->normal(0.666667,0.333333,0.666667);cube->textureCoord(0,0);

   cube->triangle(0,1,2);      cube->triangle(3,1,0);
   cube->triangle(4,5,6);      cube->triangle(4,7,5);
   cube->triangle(8,9,10);      cube->triangle(10,7,8);
   cube->triangle(4,11,12);   cube->triangle(4,13,11);
   cube->triangle(14,8,12);   cube->triangle(14,15,8);
   cube->triangle(16,17,18);   cube->triangle(16,19,17);
   cube->end();

   return cube;

}

void Plugin_x3d::getBoxData(CyberX3D::SceneGraph *sg,CyberX3D::BoxNode * bNode){

	float sizeA[3];

	bNode->getSize(sizeA);
	nVertices=8;

	float coordX = sizeA[0]/2;
	float coordY = sizeA[1]/2;
	float coordZ = sizeA[2]/2;

	size.push_back(coordX);
	size.push_back(coordY);
	size.push_back(coordZ);

	//0
	vertices.push_back(-coordX);
	vertices.push_back(-coordY);
	vertices.push_back(-coordZ);

	//1
	vertices.push_back(-coordX);
	vertices.push_back(coordY);
	vertices.push_back(-coordZ);

	//2
	vertices.push_back(coordX);
	vertices.push_back(coordY);
	vertices.push_back(-coordZ);

	//3
	vertices.push_back(coordX);
	vertices.push_back(-coordY);
	vertices.push_back(-coordZ);

	//4
	vertices.push_back(-coordX);
	vertices.push_back(-coordY);
	vertices.push_back(coordZ);

	//5
	vertices.push_back(-coordX);
	vertices.push_back(coordY);
	vertices.push_back(coordZ);

	//6
	vertices.push_back(coordX);
	vertices.push_back(coordY);
	vertices.push_back(coordZ);

	//7
	vertices.push_back(coordX);
	vertices.push_back(-coordY);
	vertices.push_back(coordZ);

	float faceArray [] =  {
		0,3,1,3,2,1,
		3,7,2,7,6,2,
		7,5,4,7,6,5,
		4,1,0,4,5,1,
		5,1,2,2,6,5,
		4,0,3,3,7,4
		};
	faces.assign(faceArray,faceArray+12*3);

}