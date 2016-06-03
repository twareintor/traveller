

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <irrlicht.h>
#include <iostream>
#include <windows.h>
#include <vector>	// 
#include "txtFilePro.h"

// see modern houses: "http://tf3dm.com/download-page.php?url=the-city-39441"
// program constants
#define		ID_CAM01	10
#define		ID_CAM02	20
#define		ID_CAM03	30
// video driver constants
#define		FULL_SCR	false		// 
#define		ST_BUF		true		// stencil buffer
// astronomical constants related to the scene:
#define		Y_MAX		100000.f
#define		N_STARS		1000
// geometry and math constants
#define		PI			3.14159f

using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

u32 detectJoystick(IrrlichtDevice* device, core::array<SJoystickInfo>& joystickInfo);

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

// // // BUG: A bug somewhere causes 

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
	IrrlichtDevice *device =
		createDevice(driverType, core::dimension2d<u32>(nWidth, nHeight), 16U, FULL_SCR, ST_BUF, false, &receiver);
	if (device == 0)
		return 1; // could not create selected driver.
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
	gui::IGUIFont* font = env->getFont("c:/media/fonthaettenschweiler.bmp");
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
		"c:/media/terrain_heightmap_734.png",
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
	nodTer->setMaterialTexture(0, driver->getTexture("c:/media/terrain_texture_734.png"));
	// nodTer->setMaterialTexture(1, driver->getTexture("c:/media/detailmap3.jpg"));
	// nodTer->setMaterialType(video::EMT_DETAIL_MAP);
	// nodTer->scaleTexture(1.0f, 20.0f);
	nodTer->getMaterial(0).DiffuseColor.set(255, 147, 127, 22);
	nodTer->getMaterial(0).EmissiveColor.set(255, 12, 8, 0);
	// nodTer->setMaterialFlag(video::EMF_BACK_FACE_CULLING, true);
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
	for(u32 i = 1; i<64; i++)
	{
		std::ostringstream	ossShp;
		ossShp << "C:/media/trees_733/lp73" << 3+(int)(rand()%5) << ".obj";
		scene::IAnimatedMesh* mshTree = smgr->getMesh(ossShp.str().c_str());
		scene::IAnimatedMeshSceneNode* nodTree = 0;
		if(mshTree)
		{
			nodTree = smgr->addAnimatedMeshSceneNode(mshTree);
			nodTree->setScale(core::vector3df(180.f - 20.f*(f32)i, 80.f + 2.f*(f32)i, 180.f - 20.f*(f32)i));
			f32		xTree = nodTer->getTerrainCenter().X + 420.f*(f32)i*(f32)(rand()%12);
			f32		zTree = nodTer->getTerrainCenter().Z + 452.f*(f32)i*(f32)(rand()%12); 
			f32		yTree = nodTer->getHeight(xTree, zTree)-2.f;
			nodTree->setPosition(core::vector3df(xTree, yTree, zTree));
			nodTree->setRotation(core::vector3df((f32)(rand()%22), (f32)(rand()%360), (f32)(rand()%12)));
			nodTree->setMaterialFlag(video::EMF_LIGHTING, true);
			// nodTree->setMaterialFlag(video::EMF_COLOR_MATERIAL, true);
			// nodTree->setMaterialFlag(video::EMF_GOURAUD_SHADING, true);
			nodTree->setMaterialTexture(0, driver->getTexture("c:/media/treetex01.jpg"));
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
	
	// add a citadel // choose another mesh, this causes shadow artifacts
	/*************************************
	scene::IMesh* mshTzi = smgr->getMesh("c:/media/");
	scene::IMeshSceneNode* nodTzi = 0;
	if(mshTzi)
	{
		
		nodTzi = smgr->addMeshSceneNode(mshTzi);
		nodTzi->setScale(core::vector3df(720.f, 720.f, 720.f));
		scene::IMeshBuffer* mbfTzi = 0;
		for(u32 i = 0; i<nodTzi->getMaterialCount(); i++)
		{
			mbfTzi = mshTzi->getMeshBuffer(i);
			manip->recalculateNormals(mbfTzi);
		}
		nodTzi->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
		f32		xCit = nodTer->getTerrainCenter().X + 220.f;
		f32		zCit = nodTer->getTerrainCenter().Z + 352.f; 
		f32		yCit = nodTer->getHeight(xCit, zCit)-2.f;
		nodTzi->setPosition(core::vector3df(xCit, yCit, zCit));
		nodTzi->setMaterialFlag(video::EMF_LIGHTING, true);
		nodTzi->setMaterialFlag(video::EMF_ANTI_ALIASING, true);
		nodTzi->addShadowVolumeSceneNode();
		nodTzi->setVisible(false);
	}
	******************************************/
	
	// Animals: Animals:
	core::array<scene::IAnimatedMesh*> mshAnimals;
	core::array<scene::IAnimatedMeshSceneNode*> nodAnimals;
	mshAnimals.reallocate(2);
	nodAnimals.reallocate(2);
	mshAnimals[0] = smgr->getMesh("c:/media/horse/horse.md2");				// free for any use
	mshAnimals[1] = smgr->getMesh("c:/media/bird_final/bird_final.md2");	// free for any use
	for(u16 i = 0; i<nodAnimals.allocated_size(); i++)
	{
		if(mshAnimals[i])
		{
			nodAnimals[i] = smgr->addAnimatedMeshSceneNode(mshAnimals[i]);
			
		}
	}

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
	nodBil->setMaterialTexture(0, driver->getTexture("c:/media/particlewhite.bmp"));
	nodBil->setVisible(true);
	// the Moon:
	scene::IAnimatedMesh* mshMoon = smgr->addSphereMesh("Moon", 4700.f);
	scene::IAnimatedMeshSceneNode* nodMoon = smgr->addAnimatedMeshSceneNode(mshMoon, nodCen, -1, core::vector3df((Y_MAX-10000.f), -14000.f, 0.f));
	nodMoon->setRotation(core::vector3df(00.f, -90.f, 00.f));
	nodMoon->setMaterialFlag(video::EMF_LIGHTING, false);
	nodMoon->setMaterialFlag(video::EMF_GOURAUD_SHADING, false);		
	nodMoon->getMaterial(0).EmissiveColor.set(255, 90, 80, 20);
	nodMoon->setMaterialTexture(0, driver->getTexture("c:/media/Moon_tex.jpg"));
	// Stars
	scene::ISceneNode* nodCes = smgr->addEmptySceneNode(); // the stars will be placed at equal distance around it
	scene::ISceneNode* nodCet = smgr->addEmptySceneNode(); // the stars will be placed at equal distance around it
	nodCes->setParent(nodCen);
	nodCet->setParent(nodCen);
	nodCet->setRotation(core::vector3df(1.f, 1.f, 1.f));	// for the text to be "under" the stars
	f32 fDS, fTh, fFi, fR;		// DS = Magnitude, Th = Theta, Fi = phi, R = Radius...
	STAR* pStar = (STAR*)GlobalAlloc(GPTR, N_STARS*sizeof(STAR));
	u32 nStars = getStarData(pStar);		// BUG HERE... SOLVED
	f32 fMa = (pStar+nStars-1)->fMag-pStar->fMag;	// magnitude
	// std::pair<scene::IBillboardSceneNode*, scene::IBillboardTextSceneNode*> paiStar[N_STARS]; // next time I'll use it
	scene::IBillboardSceneNode* nodStar[N_STARS];
	scene::ITextSceneNode* txtInf[N_STARS];			// information about the stars: this time only the name
	for (u32 i = 0; i < N_STARS; i++)
	{
		if(i>nStars) break;
		fDS = 1200.f-200.f*(pStar+i)->fMag;		// the appearent "diameter" of the star
		fTh = ((360.f/24.f*((f32)(pStar+i)->nRA+1.f/60.f*(f32)(pStar+i)->nM))*2*PI)/360.f;
		fFi = -(((pStar+i)->fB)*2*PI)/360.f;
		/************************************
		fDS = 1000.f-800*(f32)(rand()%1000+1)/1000.f;	// simulate magnitude: 
		fTh = (f32)(rand()%1000+1)*PI*2.f/1000.f;
		fFi = (f32)(rand()%1000+1)*PI*2.f/1000.f;
		****************************************/
		fR = Y_MAX/1.f; // 
		nodStar[i] = smgr->addBillboardSceneNode(nodCes, core::dimension2d<f32>(fDS, fDS), 
			core::vector3df(fR*(f32)cos(fFi)*(f32)cos(fTh), fR*(f32)cos(fFi)*(f32)sin(fTh), fR*(f32)sin(fFi)), 
			/* index = */ 0x1000+i);
		nodStar[i]->setMaterialFlag(video::EMF_LIGHTING, false);
		nodStar[i]->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		switch((pStar+i)->szSpectralType[0])
		{
		case 'B':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_blue.bmp"));	break;
		case 'O':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_blue.bmp"));	break;
		case 'M':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_red.bmp"));		break;
		case 'K':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_red.bmp"));		break;
		case 'F':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_yellow.bmp"));	break;
		case 'G':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_yellow.bmp"));	break;
		case 'A':	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_white.bmp"));	break;
		default:	nodStar[i]->setMaterialTexture(0, driver->getTexture("c:/media/star_white.bmp"));	break;
		}
		// nodStar[i]->setVisible(true); // not necessary here!
		// show the name of the main stars:
		LPWSTR wszName = (LPWSTR)GlobalAlloc(GPTR, 0x40);
		size_t nRet;
		mbstowcs_s(&nRet, wszName, 0x40, (pStar+i)->szName, strlen((pStar+i)->szName)+1);
		core::dimension2d<u32> txtSize = font->getDimension(wszName);
		txtInf[i] = smgr->addTextSceneNode(font, wszName, video::SColor(102, 102, 42, 02), nodCet, nodStar[i]->getPosition(), -1);
		// txtInf[i]->setMaterialFlag(video::EMF_LIGHTING, false);
		GlobalFree(wszName);	// free temporary WString
	}

	// Animals: animated: add a running horse in the scene:


	// FOR DEBUG AND DEVELOPMENT: add a mirror to render the back-scene in:
	// the "mirror"
	scene::ISceneNode* nodMirr = smgr->addCubeSceneNode(1.f);		// 
	nodMirr->setScale(core::vector3df(16.f/3.f, 9.f/3.f, .1f));
	nodMirr->setParent(camMain);
	nodMirr->setPosition(core::vector3df(10.f, 3.5f, 10.f));
	nodMirr->setRotation(core::vector3df(0.f, 180.f, 0.f));
	nodMirr->setMaterialFlag(video::EMF_LIGHTING, false);
	nodMirr->getMaterial(0).EmissiveColor.set(255);
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
	video::SLight ld = light01->getLightData();			
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
				nodO->setPosition(nodO->getPosition()-ds*dirMove-core::vector3df(0.f, dv*(1.f-da), 0.f)); // move along the pos...target direction...
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
				ld.DiffuseColor.g = 0.4f*fAt;
				ld.DiffuseColor.b = 0.4f*fAt;
				light01->setLightData(ld);
				// std::cout << fAt << "; " << ld.DiffuseColor.b<<"\n";		// FOR DEBUG ONLY
			}
			if (receiver.IsKeyDown(irr::KEY_ADD))
			{
				rotCen = nodCen->getRotation();
				nodCen->setRotation(rotCen - core::vector3df(0.f, 0.f, 0.1f)*MOVEMENT_SPEED*frameDeltaTime);
				// this simulates the "attenuation" of g, b compnents of the light to obtain red light on evening or morning
				ld.DiffuseColor.r = 0.3f*(1-fAt/2.f)*(fAt>0.0001f);
				ld.DiffuseColor.g = 0.3f*fAt;
				ld.DiffuseColor.b = 0.3f*fAt;
				light01->setLightData(ld);
			}
			if (receiver.IsKeyDown(irr::KEY_F11))		// FOR DEBUG ONLY						#DEBUG#3209
			{
				f32 fStep = 0.5f;
			}
			if (receiver.IsKeyDown(irr::KEY_F12))		// FOR DEBUG ONLY						#DEBUG#3209
			{
				Sleep(400);
			}
			u32 nAt = (u32)(255 * fAt);	// for a nice special effect - light attenuation day/ night...
			nodSun->setScale(core::vector3df(2.f-fAt, 2.f-fAt, 2.f-fAt));	// make the Sun appearing bigger mornings and evenings
			nodMoon->setScale(core::vector3df(2.f-fAt, 2.f-fAt, 2.f-fAt));	// make the Sun appearing bigger mornings and evenings
			nodSun->getMaterial(0).EmissiveColor.set(255, 255, nAt, nAt);		// make the Sun appearing red at sunrise or evening
			if (fAt > 0.2f)	// change the billboard surrouning the Sun red, or yellow according to the appearance of the Sun
			{
				nodBil->setMaterialTexture(0, driver->getTexture("c:/media/particlewhite.bmp"));
			}
			else
			{
				nodBil->setMaterialTexture(0, driver->getTexture("c:/media/particlered.bmp"));
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
	GlobalFree(pStar);
	met->drop();
	// rt->drop();		// when UNCOMMENTED, the program crashes by exit!!!
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

