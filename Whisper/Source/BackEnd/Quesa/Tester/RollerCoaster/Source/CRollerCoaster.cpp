/*
 *  File:       CRollerCoaster.cpp
 *  Summary:	Moves a camera along a roller coaster.
 *  Written by: Jesse Jones
 *
 *  Copyright © 2000-2001 Jesse Jones. 
 *	This code is distributed under the zlib/libpng license (see License.txt for details).  
 *
 *  Change History (most recent first):	
 *
 *		$Log: CRollerCoaster.cpp,v $
 *		Revision 1.6  2001/04/21 02:25:12  jesjones
 *		Updated for the new debug macros.
 *		
 *		Revision 1.5  2001/04/14 07:34:33  jesjones
 *		Ported to Carbon.
 *		
 *		Revision 1.4  2001/04/13 07:42:51  jesjones
 *		Ported to CarbonLib 1.3b2
 *		
 *		Revision 1.3  2001/03/17 08:55:42  jesjones
 *		Added a throttle.
 *		
 *		Revision 1.2  2000/11/09 08:57:35  jesjones
 *		1) Removed double CRs introduced during the initial checkin. 2) Changed the header comments to make it clearer that Whisper is using the zlib license agreement. 3) Added the Log keyword.
 */

#include <XWhisperHeader.h>
#include "CRollerCoaster.h"

#include <QD3DAcceleration.h>

#include <X3DCameras.h>
#include <X3DDrawContexts.h>
#include <X3DLights.h>
#include <X3DPolygon.h>
#include <X3DRenderingLoops.h>
#include <X3DRenderers.h>
#include <X3DSurfaceShaders.h>
#include <X3DTextures.h>
#include <X3DVertexList.h>
#include <XExceptions.h>
#include <XFolderSpec.h>
#include <XFile.h>
#include <XLocker.h>
#include <XMiscUtils.h>

#if WIN
	#include <WSystemInfo.h>
#endif

#include "CBuildTrack.h"

using namespace Whisper;


//-----------------------------------
//	Constants
//
const int32 kSkipAheadPoints 			= 2;
const float	kDistanceFromTrackToCamera 	= 0.5F;
const int32	kGroundSize 				= 60;
const int32	kMaxSplinePoints 			= 40000L;

const wchar_t* kGroundTextureMap = L"Ground.pict";
//const wchar_t* kTrackTextureMap  = L"Rivets Track.pict";
//const wchar_t* kTrackTextureMap  = L"Grooved Track.pict";
const wchar_t* kTrackTextureMap  = L"Red Dot Track.pict";


// ===================================================================================
//	Internal Functions
// ===================================================================================

//----------------------------------------------------------------
//
// GetPictForTexture	
//
//----------------------------------------------------------------
static X3DShader GetPictForTexture(PicHandle pictH, Rect* pictRect)
{
	PRECONDITION(pictH != nil);
		
	int32 width  = pictRect->right  - pictRect->left;	// $$$ need to byte swap on Windows
	int32 height = pictRect->bottom - pictRect->top;
	int16 depth  = 16;
	
	SPixmapStorage pixmap(pictH, width, height, depth);
	X3DPixmapTexture texture(pixmap);
		
	X3DTextureShader shader(texture);

	return shader;
}


//----------------------------------------------------------------
//
// GetWindowSize	
//
//----------------------------------------------------------------
static X2DSize GetWindowSize(OSWindow window)
{	
	PRECONDITION(window != nil);
	
	X2DSize size;
	
#if TARGET_OS_MAC
	Rect bounds;
	GetWindowBounds(window, kWindowContentRgn, &bounds);
	size.width  = bounds.right - bounds.left;
	size.height = bounds.bottom - bounds.top;
		
#else if TARGET_OS_WIN32
	XRect clientRect;
	int success = GetClientRect(window, clientRect);
	ThrowIf(!success);
	
	size.width  = clientRect.GetWidth();
	size.height = clientRect.GetHeight();
#endif

	return size;
}


//----------------------------------------------------------------
//
// CreateCamera	
//
// Creates our camera.
//
//----------------------------------------------------------------
static X3DCamera CreateCamera(float docWidth, float docHeight)
{	
	TQ3Point3D  from = {30.0F, 50.0F, -50.0F};
	TQ3Point3D  to   = {30.0F, 0.0F, 0.0F};
	TQ3Vector3D up   = {0.0F, 1.0F, 0.0F};

	float fieldOfView = 1.2F;
	float hither 	  = 0.2F;
	float yon 		  = 200.0F;
	
	TQ3ViewAngleAspectCameraData data;
	data.cameraData.placement.cameraLocation  = from;
	data.cameraData.placement.pointOfInterest = to;
	data.cameraData.placement.upVector 		  = up;

	data.cameraData.range.hither = hither;
	data.cameraData.range.yon 	 = yon;

	data.cameraData.viewPort.origin.x = -1.0F;
	data.cameraData.viewPort.origin.y = 1.0F;
	data.cameraData.viewPort.width	  = 2.0F;
	data.cameraData.viewPort.height   = 2.0F;
	
	data.fov			 = fieldOfView;
	data.aspectRatioXToY = docWidth / docHeight;

	return X3DViewAngleAspectCamera(data);
}


//----------------------------------------------------------------
//
// CreateShaderFromTexture
//
//----------------------------------------------------------------
static X3DShader CreateShaderFromTexture(const std::wstring& name)
{
	Handle hand = nil;

	{
	XFolderSpec folder(XFolderSpec::GetAppFolder(), L"Textures");
	XFileSpec spec(folder, name);
	
	XFile file(spec);
	file.Open(kReadPermission);	
		const uint32 kPictHeaderBytes = 512;
		
		uint32 len = file.GetLength() - kPictHeaderBytes;
		file.Seek(kSeekFromStart, kPictHeaderBytes);
		
		hand = NewHandle((int32) len);
		ThrowIfNil(hand);
		
		HLock(hand);
		file.Read(*hand, len);
	file.Close();
	}
	
	PicHandle pictH = (PicHandle) hand;
	Rect pictRect = (**pictH).picFrame;
	X3DShader shaderObject = GetPictForTexture(pictH, &pictRect);
	
	DisposeHandle(hand);
	
	return shaderObject;
}

#if __MWERKS__
#pragma mark -
#endif
			
// ===================================================================================
//	class CRollerCoaster
// ===================================================================================

#if WIN
	bool CRollerCoaster::fsRegisteredWindow = false;
#endif

//---------------------------------------------------------------
//
// CRollerCoaster::~CRollerCoaster
//
//---------------------------------------------------------------
CRollerCoaster::~CRollerCoaster()
{
	DisposePtr((Ptr) fSplinePointsPtr);
#if MAC
	DisposeWindow(fWindow);
#elif WIN
	VERIFY(DestroyWindow(fWindow));
#endif
}


//---------------------------------------------------------------
//
// CRollerCoaster::CRollerCoaster
//
//---------------------------------------------------------------
CRollerCoaster::CRollerCoaster() : fInterpolation(kQ3InterpolationStyleVertex), 
								   fBackFacing(kQ3BackfacingStyleBoth),
								   fFillStyle(kQ3FillStyleFilled)
{
	Rect rBounds = {50, 50, 450, 450};
	Str255 title = "\pRollerCoaster";

#if MAC
	fWindow = NewCWindow(nil, &rBounds, title, true, noGrowDocProc, (WindowPtr)-1, true, 0);
	ThrowIfNil(fWindow);

#elif WIN
	if (!fsRegisteredWindow) {
		this->DoRegisterWindow();
		fsRegisteredWindow = true;
	}
	
    fWindow = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE,	// extended style
				             "Coaster Window",          			// class name
				             "RollerCoaster",             			// window name
				             WS_OVERLAPPEDWINDOW, 					// style
				             25, 25, 640, 530, 	 					// frame
				             nil,                					// parent window
				             nil,                					// menu
				             WSystemInfo::GetAppInstance(),         // app instance
				             nil);									// user data
	ThrowIfNil(fWindow);

	(void) ::SetWindowLong(fWindow, GWL_USERDATA, (int32) this);

    ShowWindow(fWindow, SW_SHOWNORMAL); 
#endif

	fSplinePointsPtr = (NubEntryType*) NewPtr(sizeof(NubEntryType) * kMaxSplinePoints);
	ThrowIfNil(fSplinePointsPtr);

	this->DoInitView();
	
	fNumSplinePoints = 0;
	fTrackIndex 	 = 0;
			
	// create ground/track groups 
	X3DShader groundShaderObject = CreateShaderFromTexture(kGroundTextureMap);
	fGroundGroup.AddObject(groundShaderObject);

	this->DoInitGround();

	X3DShader trackShaderObject = CreateShaderFromTexture(kTrackTextureMap);
	fTrackGroup.AddObject(trackShaderObject);
	
	CBuildTrack builder;
	fNumSplinePoints = builder.Build(fTrackGroup, fSplinePointsPtr);
}


//---------------------------------------------------------------
//
// CRollerCoaster::Update
//
//---------------------------------------------------------------
void CRollerCoaster::Update()
{
 	MilliSecond startTime = GetMilliSeconds();

#if MAC
	GrafPtr oldPort;
	GetPort(&oldPort);
	SetPort(GetWindowPort(fWindow));
	
	BeginUpdate(fWindow);
#endif
 	
	X3DRenderLoop render(fView);
	do {		
		fInterpolation.Submit(fView);
		fBackFacing.Submit(fView);
		fFillStyle.Submit(fView);

		fTrackGroup.Submit(fView);
		fGroundGroup.Submit(fView);
	} while (render.IsRendering());

	this->DoMoveCamera();
					
#if MAC
	EndUpdate(fWindow);
	
	SetPort(oldPort);
#endif

 	MilliSecond elapsed = GetMilliSeconds() - startTime;
 	if (elapsed < 100)
 		MilliSecondDelay(100 - elapsed);	// lame throttle
}


//---------------------------------------------------------------
//
// CRollerCoaster::Invalidate
//
//---------------------------------------------------------------
void CRollerCoaster::Invalidate()
{
#if MAC
	Rect bounds;
	GetWindowBounds(fWindow, kWindowContentRgn, &bounds);
	
	InvalWindowRect(fWindow, &bounds);

#elif WIN
	XRect extent;
	(void) GetClientRect(fWindow, extent);
	(void) InvalidateRect(fWindow, extent, false);	
#endif
}

#if __MWERKS__
#pragma mark ~
#endif

//---------------------------------------------------------------
//
// CRollerCoaster::DoRegisterWindow
//
//---------------------------------------------------------------
#if WIN
void CRollerCoaster::DoRegisterWindow()
{
	WNDCLASS  windClass;

    windClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// CS_OWNDC is required to support the Win32DC Draw context 
    windClass.lpfnWndProc   = CRollerCoaster::DoWindowProc;       
    windClass.cbClsExtra    = 0;                      
    windClass.cbWndExtra    = 0;                     
    windClass.hInstance     = WSystemInfo::GetAppInstance();             
    windClass.hIcon         = nil; 
    windClass.hCursor       = nil;
    windClass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    windClass.lpszMenuName  = nil; 
    windClass.lpszClassName = "Coaster Window";              

    (void) RegisterClass(&windClass);
}
#endif


//---------------------------------------------------------------
//
// CRollerCoaster::DoRegisterWindow
//
//---------------------------------------------------------------
#if WIN
LRESULT CALLBACK CRollerCoaster::DoWindowProc(HWND hWnd, UINT message, WPARAM uParam, LPARAM lParam)
{
    switch (message) {
		case WM_PAINT:
			{
			PAINTSTRUCT paintStruct;
			BeginPaint(hWnd, &paintStruct);				

			CRollerCoaster* wind = reinterpret_cast<CRollerCoaster*>(::GetWindowLong(hWnd, GWL_USERDATA));
			wind->Update();

			EndPaint(hWnd, &paintStruct);
			}
			break;

        default:          
            return DefWindowProc(hWnd, message, uParam, lParam);
    }
    
    return 0;
}
#endif


//---------------------------------------------------------------
//
// CRollerCoaster::DoInitView
//
//---------------------------------------------------------------
void CRollerCoaster::DoInitView()
{
	// Create and set draw context.		
	X3DColorARGB clearColor(0.0F, 0.0F, 1.0F);
	bool doubleBuffer = true;
#if MAC
	SMacDrawContextData data(fWindow, clearColor, doubleBuffer);
	X3DMacDrawContext myDrawContext(data);
	
#elif WIN
	uint32 style = GetClassLong(fWindow, GCL_STYLE);
	ASSERT(CS_OWNDC == (style & CS_OWNDC));

	SWin32DCDrawContextData data(GetDC(fWindow), clearColor, doubleBuffer);
	X3DWin32DCDrawContext myDrawContext(data);
#endif
	fView.SetDrawContext(myDrawContext);
	
	// Create and set renderer.
#if 0
	X3DWireframeRenderer myRenderer;
	
#else
	X3DInteractiveRenderer myRenderer;
	myRenderer.SetDoubleBufferBypass(true);						
	myRenderer.SetVendorID(kQAVendor_BestChoice);	// use the hardware renderer if it is installed
#endif
	fView.SetRenderer(myRenderer);
	
	// Create and set camera.
	X2DSize size = GetWindowSize(fWindow);
	X3DCamera myCamera = CreateCamera(size.width, size.height);
	fView.SetCamera(myCamera);
	
	// Create and set lights.
	float brightness = 0.8F;
	X3DAmbientLight myAmbientLight(kRGBWhite, brightness);	//  This light data will be used for point and fill light also. 

	X3DLightGroup myLights;
	myLights.AddObject(myAmbientLight);		
	fView.SetLightGroup(myLights);
}


//---------------------------------------------------------------
//
// CRollerCoaster::DoInitGround
//
//---------------------------------------------------------------
void CRollerCoaster::DoInitGround()
{
	X3DVertexList<4> vertices;
	
	vertices.SetPoint(0, 0, -3, kGroundSize);
	vertices.SetPoint(1, kGroundSize, -3, kGroundSize);
	vertices.SetPoint(2, kGroundSize, -3, -kGroundSize);
	vertices.SetPoint(3, 0, -3, -kGroundSize);
	
//	vertices.GetAttribute(0).AddShadingUV(0, 0);	// $$$ not implemented in Quesa?
//	vertices.GetAttribute(1).AddShadingUV(1, 0);
//	vertices.GetAttribute(2).AddShadingUV(1, 1);
//	vertices.GetAttribute(3).AddShadingUV(0, 1);

	X3DPolygon polygon(vertices.GetNumVertices(), vertices.GetVertices());			
	fGroundGroup.AddObject(polygon);
}


//----------------------------------------------------------------
//
// CRollerCoaster::DoMoveCamera
//
//----------------------------------------------------------------
void CRollerCoaster::DoMoveCamera()
{
	this->DoPutCameraOnTrack();
	
	// have we reached the end of the track? 
	if (fTrackIndex + 1 >= fNumSplinePoints)
		// end-of-track, so wrap back to the beginning 
		fTrackIndex = 0;

	else
		// move to next location on track 
		++fTrackIndex;
}


//----------------------------------------------------------------
//
// CRollerCoaster::DoPutCameraOnTrack
//
//----------------------------------------------------------------
void CRollerCoaster::DoPutCameraOnTrack()
{		
	if (fNumSplinePoints == 0)
		return;

	// calc up vector		
	X3DVector cameraUp(fSplinePointsPtr[fTrackIndex].basePt, 	// calc vector from base to up point 
	                   fSplinePointsPtr[fTrackIndex].upPt); 
	cameraUp = Normalize(cameraUp);
	
	// use up vector to calc "from" position	
	X3DVector cameraFromVect = kDistanceFromTrackToCamera*cameraUp;	// calc how far above track to put camera 

	X3DPoint cameraFrom = fSplinePointsPtr[fTrackIndex].basePt + cameraFromVect;
	
	// set "to" pt					
	int32 desti = fTrackIndex + kSkipAheadPoints;
	if (desti >= (fNumSplinePoints-1))
		desti -= fNumSplinePoints;						// loop back around 

	X3DPoint cameraTo = fSplinePointsPtr[desti].basePt;	// look at base spline point in distance 
			
	// update camera placement	
	X3DCamera camera = fView.GetCamera();
	camera.SetPlacement(cameraFrom, cameraTo, cameraUp);
}

