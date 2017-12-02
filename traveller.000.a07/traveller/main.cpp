

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <irrlicht.h>
#include <iostream>
#include <windows.h>
#include <vector>	// 
#include <time.h>	// for random seed
#include "CxSky.h"


// program constants
#define		ID_CAM01	10			// different cameras with differnt id-s
#define		ID_CAM02	20
#define		ID_CAM03	30
// video driver constants
#define		FULL_SCR	true		// 
#define		ST_BUF		true		// stencil buffer
// astronomical constants related to the scene:
#define		Y_MAX		100000.f	// where the sky is
#define		N_STARS		1000		// maximal number of stars to be displayed// actually the first 300 available...
#define		N_TREES		128			// maximal number of trees to be displayed
#define		MIN_HEIGHT_TREES	8450.f	// the minimum height where the trees are to be found in the terrain
// geometry and math constants
#define		PI			3.14159f	

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif


u32 detectJoystick(IrrlichtDevice* device, core::array<SJoystickInfo>& joystickInfo);

VOID pushRndValues(core::array<core::vector3df>* ppn, scene::ITerrainSceneNode* nodTer = 0);

VOID pop_RndValues(core::array<core::vector3df>* ppn);

DWORD seedST();

class MyEventReceiver : public IEventReceiver
{
public:
	// This is the one method that we have to implement
	virtual bool OnEvent(const SEvent& event)
	{
		// Remember whether each key is down or up
		if (event.EventType == irr::EET_KEY_INPUT_EVENT)
		{
			KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;
		}
		if(event.EventType == irr::EET_JOYSTICK_INPUT_EVENT	&& event.JoystickEvent.Joystick == 0)
		{
			JoystickState = event.JoystickEvent;
		}
		return false;
	}
	const SEvent::SJoystickEvent & GetJoystickState(void) const
	{
		return JoystickState;
	}
	// This is used to check whether a key is being held down
	virtual bool IsKeyDown(EKEY_CODE keyCode) const
	{
		return KeyIsDown[keyCode];
	}
	MyEventReceiver()
	{
		for (u32 i = 0; i<KEY_KEY_CODES_COUNT; ++i)
			KeyIsDown[i] = false;
	}

private:
	// We use this array to store the current state of each key
	bool KeyIsDown[KEY_KEY_CODES_COUNT];
	SEvent::SJoystickEvent JoystickState;
};

// // // BUG: A bug somewhere causes ...

int main()
{
	// ask user for driver
	video::E_DRIVER_TYPE driverType;
	printf("Please select the driver you want for this example:\n"\
		" (a) OpenGL 1.5\n (b) Direct3D 9.0c\n (c) Direct3D 8.1\n"\
		" (d) Burning's Software Renderer\n (e) Software Renderer\n"\
		" (f) NullDevice\n (otherKey) exit\n\n");
	char i;
	std::cin >> i;
	switch (i)
	{
	case 'a': driverType = video::EDT_OPENGL;			break;
	case 'b': driverType = video::EDT_DIRECT3D9;		break;
	case 'c': driverType = video::EDT_DIRECT3D8;		break;
	case 'd': driverType = video::EDT_BURNINGSVIDEO;	break;
	case 'e': driverType = video::EDT_SOFTWARE;			break;
	case 'f': driverType = video::EDT_NULL;				break;
	default: return 1;
	}

	// get screen dimensions:
	int nWidth = GetSystemMetrics(0);
	int nHeight = GetSystemMetrics(1);
	// create device and exit if creation failed
	MyEventReceiver receiver;
	IrrlichtDevice* device =
		createDevice(driverType, core::dimension2d<u32>(nWidth, nHeight), 16U, FULL_SCR, ST_BUF, false, &receiver);
	if (device == 0)
		return 1; // could not create selected driver.
	
	// get working directory and set the default path to it:
	io::path iopExePath =device->getFileSystem()->getWorkingDirectory(); printf("exe dir =\"%s\"\n", iopExePath.c_str());
	device->getFileSystem()->changeWorkingDirectoryTo(iopExePath.c_str());

	 // Joystick: 
	core::array<SJoystickInfo> joystickInfo;
	u32 nSticks = detectJoystick(device, joystickInfo);
	// and the rest of the necessary operations the Irrlicht engine needs...
	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);
	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);
	
	// driver->setAmbientLight(video::SColorf(0.1, 0.1, 0.1));		// doesn't work
	smgr->setAmbientLight(video::SColorf(.08f, .07f, .14f, 0.f));
	
	// static text:
	gui::IGUIEnvironment* env = device->getGUIEnvironment();
	gui::IGUISkin* skin = env->getSkin();
	gui::IGUIFont* font = env->getFont("../../../../media/fonthaettenschweiler.bmp");
	if (font)
	{
		skin->setFont(font);
	}
	else
	{
		std::cout << "cannot create font ..." << std::endl;
	}
	// gui::IGUIStaticText* txtMsg = env->addStaticText(L"Application started: Traveller", core::rect<s32>(150.f, 20.f, 470.f, 80.f));
	// txtMsg->setOverrideColor(video::SColor(100, 255, 255, 255));
	// txtMsg->enableOverrideColor(true);

	// Terrain:
	scene::ITerrainSceneNode* nodTer = smgr->addTerrainSceneNode(
		"../../../../media/terrain_heightmap_733.png",
		0,					// parent node
		-1,					// node id
		core::vector3df(0.f, 0.f, 0.f),		// position
		core::vector3df(0.f, 0.f, 0.f),		// rotation
		core::vector3df(240.f, 52.f, 240.f),	// scale
		video::SColor ( 255, 255, 255, 255 ),	// vertexColor
		5,					// maxLOD
		scene::ETPS_17,				// patchSize
		4					// smoothFactor
		);
	nodTer->setMaterialFlag(video::EMF_LIGHTING, true);
	nodTer->setMaterialTexture(0, driver->getTexture("../../../../media/terrain_texture_734.png"));
	// nodTer->setMaterialTexture(1, driver->getTexture("../../../../media/terrain_texture_012.jpg"));
	nodTer->setMaterialTexture(1,  driver->getTexture("../../../../media/beachsand.jpg"));
	// video::SMaterial& sand = nodTer->getMaterial(1);
	nodTer->scaleTexture(0.1f, 0.1f);
	// nodTer->setMaterialType(video::EMT_DETAIL_MAP);
	// nodTer->getMaterial(0).DiffuseColor.set(255, 147, 127, 22);
	// nodTer->getMaterial(0).EmissiveColor.set(255, 12, 8, 0);
	// nodTer->setMaterialFlag(video::EMF_FOG_ENABLE, true);
	// Meta-triangle-selector:
	scene::IMetaTriangleSelector* met = smgr->createMetaTriangleSelector();
	// Triangle Selector for terraint:
	scene::ITriangleSelector* selTer = smgr->createTerrainTriangleSelector(nodTer, 0);
	nodTer->setTriangleSelector(selTer);
	met->addTriangleSelector(selTer);
	selTer->drop();
	// Tree(s):		// 
	scene::IMeshManipulator* manip = smgr->getMeshManipulator();
	// scene::IVertexManipulator* maniv = 0;
	// ##BUG##381 NOT ALL TREES ARE PUT INSIDE THE TERRAIN!!!!!! RESOLVE!!!!!!!!!
	for(u32 i = 1; i<N_TREES; i++)
	{
		std::ostringstream	ossShp;
		ossShp << "../../../../media/trees_733/lp73" << 3+(int)(rand()%5) << ".obj"; srand((u32)LOWORD(seedST()));
		scene::IAnimatedMesh* mshTree = smgr->getMesh(ossShp.str().c_str());
		scene::IAnimatedMeshSceneNode* nodTree = 0;
		if(mshTree)
		{
			nodTree = smgr->addAnimatedMeshSceneNode(mshTree);
			f32 fr = (32.f-(f32)(rand()%16))*4.2f; srand((u32)LOWORD(seedST()));
			f32 fh = (64.f-(f32)(rand()%18))*3.7f; srand((u32)LOWORD(seedST()));
			nodTree->setScale(core::vector3df(fr, fh, fr));
			core::aabbox3d<f32> boxTer = nodTer->getBoundingBox(); // how big is the terrain????
			core::vector3df edges[8];
			boxTer.getEdges(edges);
			f32		xTree;
			f32		zTree;
			f32		yTree = 0; 
			while (yTree<MIN_HEIGHT_TREES)
			{
				xTree = nodTer->getPosition().X+(edges[4].X-edges[0].X)/1000.f*float(rand()%1000); srand((u32)LOWORD(seedST()));
				zTree = nodTer->getPosition().Z+(edges[2].Z-edges[0].Z)/1000.f*float(rand()%1000); srand((u32)LOWORD(seedST()));
				yTree = nodTer->getHeight(xTree, zTree)-27.f;
			}
			printf("tree(%g, %g, %g)\n", xTree, yTree, zTree);
			nodTree->setPosition(core::vector3df(xTree, yTree, zTree));	srand((u32)LOWORD(seedST()));
			nodTree->setRotation(core::vector3df((f32)(rand()%22), (f32)(rand()%360), (f32)(rand()%12))); 
			nodTree->setMaterialFlag(video::EMF_LIGHTING, true);
			nodTree->setMaterialFlag(video::EMF_COLOR_MATERIAL, true);
			nodTree->setMaterialFlag(video::EMF_GOURAUD_SHADING, true);
			nodTree->setMaterialTexture(0, driver->getTexture("../../../../media/treetex01.jpg"));
			nodTree->setMaterialFlag(video::EMF_LIGHTING, true);
			nodTree->setMaterialFlag(video::EMF_ANTI_ALIASING, true);
			nodTree->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
			nodTree->addShadowVolumeSceneNode();
			// triangle selectors, then add them to the meta...
			scene::ITriangleSelector* selTree = smgr->createTriangleSelector(mshTree, nodTree);
			nodTree->setTriangleSelector(selTree);
			met->addTriangleSelector(selTree);
			selTree->drop();
		}
	}
	
	// Animals: Animals:
	core::array<scene::IAnimatedMesh*> mshAnimals;
	core::array<scene::IAnimatedMeshSceneNode*> nodAnimals;
	const u16 ANIMALS = 2;
	mshAnimals.reallocate(ANIMALS);
	nodAnimals.reallocate(ANIMALS);
	mshAnimals[0] = smgr->getMesh("../../../../media/horse/horse.md2");				// free for any use
	mshAnimals[1] = smgr->getMesh("../../../../media/bird_final/bird_final.md2");	// free for any use
	for(u16 i = 0; i<nodAnimals.allocated_size(); i++) 
	{
		if(mshAnimals[i])
		{
			nodAnimals[i] = smgr->addAnimatedMeshSceneNode(mshAnimals[i]);
			nodAnimals[i]->setScale(core::vector3df(4.5f, 4.5f, 4.5f));
			if(1==i) nodAnimals[i]->setScale(core::vector3df(.1f, .1f, .1f));	// the bird is already big enough
			float xAni = nodTer->getTerrainCenter().X+(float)(i*325);
			float zAni = nodTer->getTerrainCenter().Z+450.f+(float)(i*325);
			float yAni = nodTer->getHeight(xAni, zAni);
			printf("(%g, %g, %g)\n", xAni, yAni, zAni);
			nodAnimals[i]->setPosition(core::vector3df(xAni, yAni+(float)(i*920), zAni));
			nodAnimals[i]->setMaterialFlag(video::EMF_LIGHTING, true);
			nodAnimals[i]->addShadowVolumeSceneNode(mshAnimals[i]);

			// nodAnimals[i]->setDebugDataVisible(true);
			// nodAnimals[0]->setAnimationSpeed(100.f);	// the horse
			// nodAnimals[1]->setAnimationSpeed(240.f);	// the bird
			if(met)
			{
				scene::ITriangleSelector* tri = smgr->createTriangleSelector(mshAnimals[i], nodAnimals[i]);
				if(!nodAnimals[i]->getTriangleSelector()) nodAnimals[i]->setTriangleSelector(tri);
				met->addTriangleSelector(tri);
				tri->drop();
			}
		}
	}

	// animation for the animals:
	core::array<scene::ISceneNodeAnimator*> anims;
	anims.reallocate(nodAnimals.allocated_size());	// 
	printf("there are %d or %d animals in the scene\n", nodAnimals.allocated_size(), nodAnimals.size());
	core::array<core::vector3df>* ppoints;	// an array of points to create spline for every animal
	ppoints = (core::array<core::vector3df>*)GlobalAlloc(GPTR, anims.allocated_size()*sizeof(core::array<core::vector3df>));
	for(u16 i=0; i<anims.allocated_size(); i++)
	{
		/////////////////// ####BUG HERE#######////////////////////
		// Correct it!!!!! ///////////////////////////
		/**************************
		pushRndValues(&ppoints[i], nodTer);
		anims[i] = smgr->createFollowSplineAnimator(0, ppoints[i]);
		anims[i]->animateNode(nodAnimals[i], 0);
		*****************************/
		// anims[i] = smgr->createFollowSplineAnimator(0, ppoints[i])
	}

	GlobalFree(ppoints);
	//

	// fog
	f32 fogEnd = 5000.f;
	f32 fogDen = 0.01f;
	video::SColor colFog = video::SColor(0, 128, 128, 128);
	// driver->setFog(colFog, video::EFT_FOG_LINEAR, 10.0, fogEnd, fogDen, true, false);	// only on request
	// Sun, Sunlight, Moon, Stars, .... : 
	// Center of the world:
	scene::ISceneNode* nodZero =smgr->addEmptySceneNode();
	scene::ISceneNode* nodCen = smgr->addEmptySceneNode();			// to rotate the light01 like being the sun...
	nodCen->setParent(nodZero);
	nodZero->setRotation(core::vector3df(25.5f, 0.f, 0.f));
	scene::ILightSceneNode* light01 = smgr->addLightSceneNode(nodCen, core::vector3df(-Y_MAX, 0.f, 0.f), video::SColorf(0.4f, 0.f, 0.f), Y_MAX*1.9f, -1);
	// because is very far away, appears to be directional light but is not; however, directional light were a better solution...
	light01->setVisible(true);
	light01->enableCastShadow(true);
	// 
	
	// the main camera:
	SKeyMap keyMap[] = 
	{
		{EKA_MOVE_FORWARD,		KEY_UP		}, 		{EKA_MOVE_FORWARD,		KEY_KEY_W	}, 
		{EKA_MOVE_BACKWARD,		KEY_DOWN	}, 		{EKA_MOVE_BACKWARD,		KEY_KEY_S	}, 
		{EKA_STRAFE_LEFT,		KEY_LEFT	}, 		{EKA_STRAFE_LEFT,		KEY_KEY_A	}, 
		{EKA_STRAFE_RIGHT,		KEY_RIGHT	}, 		{EKA_STRAFE_RIGHT,		KEY_KEY_D	},
		{EKA_JUMP_UP,			KEY_SPACE	}, 		{EKA_JUMP_UP,			KEY_INSERT	},
	};
	// // the camera 
	scene::ICameraSceneNode* camMain = 0;
	camMain = smgr->addCameraSceneNodeFPS(0, 90.0f, 0.5f, ID_CAM01, keyMap, sizeof(keyMap), false, 5.f, false, true);
	camMain->setPosition(nodTer->getTerrainCenter()+core::vector3df(0.f, 300.f, 0.f));		// 
	camMain->setTarget(core::vector3df(0.f, 0.f, 10.f));
	camMain->setNearValue(10.f);
	camMain->setFarValue(Y_MAX + 10000.f);
	// now the collider for this camera: 
	scene::ISceneNodeAnimator* cra = smgr->createCollisionResponseAnimator(met, camMain, 
		core::vector3df(40, 100, 40), core::vector3df(0, -10, 0), core::vector3df(0, 90.f, 0));
	selTer->drop();
	camMain->addAnimator(cra);
	cra->drop();
	// another camera to be controlled by the stick...
	scene::ISceneNode* nodO = smgr->addEmptySceneNode(0);			// origin     -
	nodO->setPosition(camMain->getPosition()+core::vector3df(-800.f, 1500.f, -800.f));									// | 
	scene::ISceneNode* nodR = smgr->addEmptySceneNode(nodO);		// Roll direction, and vector
	nodR->setPosition(core::vector3df(1.f, 0.f, 0.f));			
	scene::ISceneNode* nodY = smgr->addEmptySceneNode(nodO);		// yaw and up vector,  |
	nodY->setPosition(core::vector3df(0.f, 1.f, 0.f));							//-
	scene::ISceneNode* nodP = smgr->addEmptySceneNode(nodO);		// Pitch vector
	nodP->setPosition(core::vector3df(0.f, 0.f, 1.f));
	scene::ICameraSceneNode* camStick = 0;
	camStick = smgr->addCameraSceneNode(0, nodO->getAbsolutePosition(), 10.f*nodR->getAbsolutePosition(), ID_CAM02);
	camStick->bindTargetAndRotation(true);
	camStick->setUpVector(nodY->getPosition());
	camStick->setVisible(false);
	smgr->setActiveCamera(camMain);
	std::cout<<"Camera position: ";
	std::cout<<camMain->getPosition().X<<", "<<camMain->getPosition().Y<<", "<<camMain->getPosition().Z<<std::endl; // DEBUG ONLY
	// the Sun:
	scene::IAnimatedMesh* mshSun = smgr->addSphereMesh("Sun", 6000.f);
	scene::IAnimatedMeshSceneNode* nodSun = smgr->addAnimatedMeshSceneNode(mshSun, light01, -1, core::vector3df(0.f, 0.f, 0.f));
	nodSun->setMaterialFlag(video::EMF_LIGHTING, true);
	nodSun->getMaterial(0).EmissiveColor.set(255, 240, 80, 40);
	scene::ISceneNode* nodBil = smgr->addBillboardSceneNode(nodSun, core::dimension2d<f32>(30000.f, 30000.f));
	nodBil->setMaterialFlag(video::EMF_LIGHTING, false);
	nodBil->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	nodBil->setMaterialTexture(0, driver->getTexture("../../../../media/particlewhite.bmp"));
	nodBil->setVisible(true);
	// the Moon:
	scene::IAnimatedMesh* mshMoon = smgr->addSphereMesh("Moon", 4700.f);
	scene::IAnimatedMeshSceneNode* nodMoon = smgr->addAnimatedMeshSceneNode(mshMoon, nodCen, -1, core::vector3df((Y_MAX-10000.f), -14000.f, 0.f));
	nodMoon->setRotation(core::vector3df(00.f, -90.f, 00.f));
	nodMoon->setMaterialFlag(video::EMF_LIGHTING, false);
	nodMoon->setMaterialFlag(video::EMF_GOURAUD_SHADING, false);		
	nodMoon->getMaterial(0).EmissiveColor.set(255, 90, 80, 20);
	nodMoon->setMaterialTexture(0, driver->getTexture("../../../../media/Moon_tex.jpg"));
	// Stars
	scene::ISceneNode* nodCes = smgr->addEmptySceneNode(); // the stars will be placed at equal distance around it
	scene::ISceneNode* nodCet = smgr->addEmptySceneNode(); // the stars will be placed at equal distance around it
	nodCes->setParent(nodCen);
	nodCet->setParent(nodCen);
	nodCet->setRotation(core::vector3df(1.f, 1.f, 1.f));	// for the text to be "under" the stars
	f32 fDS, fTh, fFi, fR;		// DS = Magnitude, Th = Theta, Fi = phi, R = Radius...
	PSTAR pStar = (PSTAR)GlobalAlloc(GPTR, N_STARS*sizeof(STAR));
	u32 nStars = getStarData(pStar);		// BUG HERE... SOLVED
	f32 fMa = (pStar+nStars-1)->fMag-pStar->fMag;	// magnitude
	// std::pair<scene::IBillboardSceneNode*, scene::IBillboardTextSceneNode*> paiStar[N_STARS]; // next time I'll use it
	scene::IBillboardSceneNode* nodStar[N_STARS];
	scene::ITextSceneNode* txtInf[N_STARS];			// information about the stars: this time only the name
	for (u32 i = 0; i < N_STARS; i++)
	{
		if(i>nStars) break;
		fDS = 1200.f-200.f*(pStar+i)->fMag;		// the appearent "diameter" of the star // simulates the magnitude
		fTh = ((360.f/24.f*((f32)(pStar+i)->nRA+1.f/60.f*(f32)(pStar+i)->nM))*2*PI)/360.f;
		fFi = -(((pStar+i)->fB)*2*PI)/360.f;
		fR = Y_MAX/1.f; // 
		nodStar[i] = smgr->addBillboardSceneNode(nodCes, core::dimension2d<f32>(fDS, fDS), 
			core::vector3df(fR*(f32)cos(fFi)*(f32)cos(fTh), fR*(f32)cos(fFi)*(f32)sin(fTh), fR*(f32)sin(fFi)), 
			/* index = */ 0x1000+i);
		nodStar[i]->setMaterialFlag(video::EMF_LIGHTING, false);
		nodStar[i]->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		switch((pStar+i)->szSpectralType[0])
		{
		case 'B':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_blue.bmp"));	break;
		case 'O':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_blue.bmp"));	break;
		case 'M':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_red.bmp"));	break;
		case 'K':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_red.bmp"));	break;
		case 'F':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_yellow.bmp"));	break;
		case 'G':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_yellow.bmp"));	break;
		case 'A':	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_white.bmp"));	break;
		default:	nodStar[i]->setMaterialTexture(0, driver->getTexture("../../../../media/star_blue.bmp"));	break;
		}
		// nodStar[i]->setVisible(true); // not necessary here!
		// show the name of the main stars:
		LPWSTR wszName = (LPWSTR)GlobalAlloc(GPTR, 0x40);
		size_t nRet;
		mbstowcs_s(&nRet, wszName, 0x40, (pStar+i)->szName, strlen((pStar+i)->szName)+1);
		core::dimension2d<u32> txtSize = font->getDimension(wszName);
		txtInf[i] = smgr->addTextSceneNode(font, wszName, video::SColor(204, 204, 204, 02), nodCet, nodStar[i]->getPosition(), -1);
		txtInf[i]->setScale(core::vector3df(2.f, 2.f, 2.f));
		// txtInf[i]->setMaterialFlag(video::EMF_LIGHTING, false);
		GlobalFree(wszName);	// free temporary WString
	}

	// Animals: animated: put a running horse on the scene:

	// FOR DEBUG AND DEVELOPMENT: add a mirror to render the back-scene in:
	// the "mirror"
	scene::ISceneNode* nodMirr = smgr->addMeshSceneNode(smgr->getMesh("../../../../media/trees_733/mirror_000.obj"));		// 
	nodMirr->setScale(core::vector3df(16.f/6.f, 9.f/6.f, 1.f));
	nodMirr->setParent(camMain);
	nodMirr->setRotation(core::vector3df(0.f, 180.f, 0.f));
	nodMirr->setPosition(core::vector3df(0.f, 0.f, 10.f));
	nodMirr->setPosition(core::vector3df(10.f, 3.5f, 10.f));
	nodMirr->setMaterialFlag(video::EMF_LIGHTING, false);
	nodMirr->getMaterial(0).EmissiveColor.set(255);
	nodMirr->getMaterial(0).getTextureMatrix(0).setTextureScale(1.f, -1.f);
	
	// create render target
	video::ITexture* rt = 0;
	scene::ICameraSceneNode* camMirr = smgr->addCameraSceneNode(camMain); // look backwards
	camMirr->setTarget(camMain->getPosition()-camMain->getTarget());
	camMirr->setNearValue(camMain->getNearValue());
	camMirr->setFarValue(camMain->getFarValue());
	smgr->setActiveCamera(camMain);
	if (driver->queryFeature(video::EVDF_RENDER_TO_TARGET))
	{
		rt = driver->addRenderTargetTexture(core::dimension2d<u32>(256, 256), "RTT1");
		nodMirr->setMaterialTexture(0, rt);
	}
	else
	{
		MessageBoxA(NULL, "creating render target failed", "", MB_OK);
	}
	nodMirr->setVisible(false);
	// all other needed operations:s
	device->getCursorControl()->setVisible(false);
	// special effects: light on evening..
	video::SLight& ld = light01->getLightData();			
	// the current data of the light // to simulate the evening light effect
	/******************************
	std::cout << "light automatic culling = " << light01->getAutomaticCulling() << std::endl;
	std::cout << "light data: " << light01->getLightData().DiffuseColor.r << ", ";
	std::cout << light01->getLightData().DiffuseColor.g << ", ";
	std::cout << light01->getLightData().DiffuseColor.b << ", "; 
	std::cout << std::endl;
	***********************************/
	bool bFog = false;		// make fog or not
	// some other variables for frame and movement processing, status scene variables						
	int lastFPS = -1;
	u32 then = device->getTimer()->getTime();
	const f32 MOVEMENT_SPEED = 50.f;
	u32 iMat = 0;			// material of the main node = the city, // FOR DEBUG ONLY
	f32 iSca = 1.f;			// scale to adjust  // FOR DEBUG ONLY
	f32 ds = 0.f;			// moving along with the joystick...
	f32 dt = 0.f;			// theta rotating the joystick camera
	f32 df = 0.f;			// phi rotating the joystick camera
	f32 dv = 0.f;			// unit elevation distance vertically
	f32 da = 0.f;			// the same as above, but for the Z-axis of the stick
	bool bStick = false;	// true when using joystick, false otherwise...
	core::vector3df dirMove;
	core::vector3df dirRot;
	core::vector3df rotCen;
	dirMove = camStick->getTarget()-camStick->getPosition();
	f32 rCam = sqrt(dirMove.X*dirMove.X+dirMove.Y*dirMove.Y+dirMove.Z*dirMove.Z);
	bool bAllStars = false;	// when true, show all stars, day and night // FOR DEBUG, ETC
	// dirMove = dirMove.normalize();
	// nodCet->setVisible(false);	// It's not enough to set invisible the parrent node of the billboards, 
	//		they are still visible!
	// mirror:
	bool bMirr = false;
	while(device->run())
	{
		if (device->isWindowActive())
		{
			nodZero->setPosition(camMain->getPosition());	// the sun, stars and moon moves with the camera 
			// 				to create the illusion of infinity, this does not affect the fps rate too much
			// Work out a frame delta time.
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
			then = now;
			f32 fAt = max(0.f, (light01->getAbsolutePosition().Y+0.25f*Y_MAX)*(1.f-0.25f)/Y_MAX);
			u32 nAt;
			if (receiver.IsKeyDown(irr::KEY_KEY_1))			{
				fogEnd-=10.f*MOVEMENT_SPEED*frameDeltaTime;
			}
			if (receiver.IsKeyDown(irr::KEY_KEY_2))
			{
				fogEnd += 10.f*MOVEMENT_SPEED*frameDeltaTime;
			}
			if (receiver.IsKeyDown(irr::KEY_F7))
			{
				std::cout << "fog params: \t" << fogEnd << "\t" << fogDen << std::endl;
			}
			if (receiver.IsKeyDown(irr::KEY_F9))
			{
				std::cout<<"My position: ";
				std::cout<<"("<<camMain->getPosition().X<<", "<<camMain->getPosition().Y-180.f<<", "<<camMain->getPosition().Z<<")"<<std::endl; // DEBUG ONLY
				Sleep(400);
			}
			if (receiver.IsKeyDown(irr::KEY_KEY_R))
			{
				camMain->setPosition(camMain->getPosition() + core::vector3df(0.f, 1.f, 0.f)*MOVEMENT_SPEED*frameDeltaTime);
			}
			if (receiver.IsKeyDown(irr::KEY_KEY_F))
			{
				camMain->setPosition(camMain->getPosition() - core::vector3df(0.f, 1.f, 0.f)*MOVEMENT_SPEED*frameDeltaTime);
			}
			if (receiver.IsKeyDown(irr::KEY_PRIOR))
			{
				camMain->setPosition(camMain->getPosition() + core::vector3df(0.f, 1.f, 0.f)*MOVEMENT_SPEED*frameDeltaTime);
			}
			if (receiver.IsKeyDown(irr::KEY_NEXT))
			{
				camMain->setPosition(camMain->getPosition() - core::vector3df(0.f, 1.f, 0.f)*MOVEMENT_SPEED*frameDeltaTime);
			}
			if (receiver.IsKeyDown(irr::KEY_KEY_B))
			{
				core::matrix4 matVOld = driver->getTransform(video::ETS_VIEW);
				matVOld = 0.5f*matVOld; 
				driver->setTransform(video::ETS_VIEW, matVOld);
			}
			if (receiver.IsKeyDown(irr::KEY_F1))
			{
				
			}
			if (joystickInfo.size() > 0)
			{
				if(receiver.IsKeyDown(irr::KEY_F2))			// for Joystick and the camera controlled with joystick...
				{
					bStick = true;
					smgr->setActiveCamera(camStick);
					camStick->setVisible(bStick);
					camMain->setVisible(!bStick);
				}
				const SEvent::SJoystickEvent& joystickData = receiver.GetJoystickState();
				ds = (f32)joystickData.Axis[SEvent::SJoystickEvent::AXIS_Y]/32767.f;
				dt = (f32)joystickData.Axis[SEvent::SJoystickEvent::AXIS_X]/32767.f;
				dv = (f32)joystickData.Axis[SEvent::SJoystickEvent::AXIS_R]/32767.f;
				da = (f32)joystickData.Axis[SEvent::SJoystickEvent::AXIS_Z]/32767.f;
				if(fabs(ds)<0.05f)	ds = 0.f;
				if(fabs(dt)<0.05f)	dt = 0.f;
				if(fabs(dv)<0.05f)	dv = 0.f;
				if(fabs(da)<0.05f)	da = 0.f;
				nodR->updateAbsolutePosition();
				dirMove = nodR->getAbsolutePosition()-nodO->getAbsolutePosition();
				//		move along the pos...target direction...
				nodO->setPosition(nodO->getPosition()-ds*dirMove-core::vector3df(0.f, dv*(1.f-da), 0.f)); 
				// nodO->setRotation(nodO->getRotation()+core::vector3df(dt, dt, 0.f));
				camStick->setPosition(nodO->getAbsolutePosition()); // move along the pos...target direction...
				camStick->setTarget(nodR->getAbsolutePosition());
				// camStick->setUpVector(nodY->getPosition());
				camStick->updateAbsolutePosition();
			}
			// driver->setFog(colFog, video::EFT_FOG_LINEAR, 10.0, fogEnd, fogDen, true, false);
			if (receiver.IsKeyDown(irr::KEY_SUBTRACT))
			{
				core::vector3df rotCen = nodCen->getRotation();
				nodCen->setRotation(rotCen + core::vector3df(0.f, 0.f, 0.1f)*MOVEMENT_SPEED*frameDeltaTime);
				// this simulates the "attenuation" of g, b compnents of the light to obtain red light on evening or morning
				ld.DiffuseColor.r = 0.4f*(1-fAt/2.f)*(fAt>0.0001f);
				ld.DiffuseColor.g = 0.4f*fAt*(fAt>0.0001f);
				ld.DiffuseColor.b = 0.4f*fAt*(fAt>0.0001f);
				light01->setLightData(ld);
				// std::cout << fAt << "; " << ld.DiffuseColor.b<<"\n";		// FOR DEBUG ONLY
			}
			if (receiver.IsKeyDown(irr::KEY_ADD))
			{
				rotCen = nodCen->getRotation();
				nodCen->setRotation(rotCen - core::vector3df(0.f, 0.f, 0.1f)*MOVEMENT_SPEED*frameDeltaTime);
				// this simulates the "attenuation" of g, b compnents of the light to obtain red light on evening or morning
				ld.DiffuseColor.r = 0.3f*(1-fAt/2.f)*(fAt>0.0001f);
				ld.DiffuseColor.g = 0.3f*fAt*(fAt>0.0001f);
				ld.DiffuseColor.b = 0.3f*fAt*(fAt>0.0001f);
				light01->setLightData(ld);
			}
			nAt = (u32)(255 * fAt);	// for a nice special effect - light attenuation day/ night...
			nodSun->setScale(core::vector3df(2.f-fAt, 2.f-fAt, 2.f-fAt));	// make the Sun appearing bigger mornings and evenings
			nodMoon->setScale(core::vector3df(2.f-fAt, 2.f-fAt, 2.f-fAt));	// make the Moon too...
			nodSun->getMaterial(0).EmissiveColor.set(255, 255, nAt, nAt);		// make the Sun appearing red at sunrise or evening

			if (fAt > 0.2f)	// change the billboard surrouning the Sun red, or yellow according to the appearance of the Sun
			{
				nodBil->setMaterialTexture(0, driver->getTexture("../../../../media/particlewhite.bmp"));
			}
			else
			{
				nodBil->setMaterialTexture(0, driver->getTexture("../../../../media/particlered.bmp"));
			}
			if(receiver.IsKeyDown(irr::KEY_KEY_L))
			{
				bAllStars = true;
			}
			if(receiver.IsKeyDown(irr::KEY_KEY_K))
			{
				bAllStars = false;
			}
			///////// to be added in extra function for optimization
			nodCes->setVisible(fAt<0.2f||bAllStars);
			for(int i = 0; i<nStars; i++)
			{
				nodStar[i]->setVisible(i<(1.f-fAt/0.2f)*nStars||bAllStars);
				txtInf[i]->setVisible(i<(1.f-fAt/0.2f)*nStars||bAllStars);
			}
			nodCet->setVisible(false);
			if(receiver.IsKeyDown(irr::KEY_KEY_V))
			{
				nodCet->setVisible(true);
			}
			/////////
			if (receiver.IsKeyDown(irr::KEY_F10))	// TOGGLE FOG
			{
				camMain->updateAbsolutePosition();
				core::vector3df pos = camMain->getAbsolutePosition();
				// std::cout << "Camera abs. position: " << pos.X << ", " << pos.Y << ", " << pos.Z << std::endl;
				Sleep(400);
			}
			if (receiver.IsKeyDown(irr::KEY_KEY_M)) // Mirror on/ off
			{
				bMirr = !bMirr;
				nodMirr->setVisible(bMirr);
				Sleep(500);
			}
			// colFog = nodSun->getMaterial(0).EmissiveColor;
			// now drawing the current frame: 
			driver->beginScene(true, true, video::SColor(255, nAt/2, nAt/2, nAt));	// make sky color difference: day-evening-night
			if(rt && bMirr)
			{	// this renders the image on the mirror onto the mirror surface
				driver->setRenderTarget(rt, true, true, video::SColor(255, nAt/2, nAt/2, nAt));
				nodMirr->setVisible(false);
				camMirr->setTarget(camMain->getPosition()-camMain->getTarget());
				smgr->setActiveCamera(camMirr);
				smgr->drawAll();
				nodMirr->setVisible(true);
				smgr->setActiveCamera(camMain);
				driver->setRenderTarget(0, true, true, video::SColor(255, nAt/2, nAt/2, nAt));
			}
			smgr->drawAll();
			// then end scene... 
			driver->endScene();
			int fps = driver->getFPS();
			if (lastFPS != fps)
			{
				core::stringw str = L"Traveler [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}
		else
			device->yield();
	}
	// releasing alocated resources:
	for(u16 i=0; i<anims.allocated_size(); i++)
	{
		pop_RndValues(&ppoints[i]);
	}
	GlobalFree(ppoints);
	// releasing alocated resources:
	GlobalFree(pStar);
	met->drop();
	device->drop();

	return 0;
}


u32 detectJoystick(IrrlichtDevice* device, core::array<SJoystickInfo>& joystickInfo)
{
	if(device->activateJoysticks(joystickInfo))
	{
		std::cout << "Joystick support is enabled and " << joystickInfo.size() << " joystick(s) are present." << std::endl;
		for(u32 joystick = 0; joystick < joystickInfo.size(); ++joystick)
		{
			std::cout << "Joystick " << joystick << ":" << std::endl;
			std::cout << "\tName: '" << joystickInfo[joystick].Name.c_str() << "'" << std::endl;
			std::cout << "\tAxes: " << joystickInfo[joystick].Axes << std::endl;
			std::cout << "\tButtons: " << joystickInfo[joystick].Buttons << std::endl;
			std::cout << "\tHat is: ";
			switch(joystickInfo[joystick].PovHat)
			{
			case SJoystickInfo::POV_HAT_PRESENT:
				std::cout << "present" << std::endl;
				break;
			case SJoystickInfo::POV_HAT_ABSENT:
				std::cout << "absent" << std::endl;
				break;
			case SJoystickInfo::POV_HAT_UNKNOWN:
			default:
				std::cout << "unknown" << std::endl;
				break;
			}
		}
	}
	else
	{
		std::cout << "Joystick support is not enabled." << std::endl;
	}
	return joystickInfo.size();
}

VOID pushRndValues(core::array<core::vector3df>* ppn, scene::ITerrainSceneNode* nodTer)
{
	srand(time(NULL));
	u16 nPoints = rand()%24;
	std::cout<<"nPoints = "<<nPoints<<std::endl;
	float r;
	float th = 0;
	core::vector3df cen = core::vector3df(0.f, 0.f, 0.f);
	if(nodTer)
	{
		cen = nodTer->getTerrainCenter();
	}
	for(u16 i=0; i<nPoints; i++)
	{
		srand(time(NULL));
		float x1, y1, z1;
		th+=(float)(12+(rand()%10));
		r = rand()%4000;
		printf("(r, th) = (%g, %g)\n", r, th);
		x1 = cen.X + r*cosf(th);
		z1 = cen.Z + r*sinf(th);
		y1 = 0;
		
		if(nodTer) y1 = nodTer->getHeight(x1, z1);
		
		printf("vec = (%g, %g, %g)\n", x1, y1, z1);
		std::cout<<"i="<<i<<"\t";
		std::cout<<"/";
		ppn->push_back(core::vector3df(x1, y1, z1));
		std::cout<<"\\";
	}
	std::cout<<std::endl;
}

VOID pop_RndValues(core::array<core::vector3df>* ppn)
{
	ppn->clear();
}

DWORD seedST()
{ 
	DWORD dwOut;
	Sleep(11);
	LPSYSTEMTIME lpst = (LPSYSTEMTIME)GlobalAlloc(GPTR, sizeof(SYSTEMTIME));
	LPFILETIME  lpft = (LPFILETIME)GlobalAlloc(GPTR, sizeof(FILETIME));
	GetSystemTime(lpst);	SystemTimeToFileTime(lpst, lpft);
	dwOut = lpft->dwLowDateTime;
	GlobalFree(lpft);
	GlobalFree(lpst);
	return dwOut;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

