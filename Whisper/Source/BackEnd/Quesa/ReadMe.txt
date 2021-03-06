Quesa is an open source replacement for QD3D built on top of OpenGL. You can find more
information at <http://quesa.DesignCommunity.com/quesa.html>. This package contains
wrappers for almost everything in QD3D/Quesa. These wrappers automate reference counting,
do exhaustive error checking, and provide a simpler API. They do this with *very* little
performance overhead.

Here's an example of how things look with the wrapper classes:

void CRollerCoaster::DoInitView()
{
	// Create and set draw context.		
	X3DColorARGB clearColor(0.0F, 0.0F, 1.0F);
	bool doubleBuffer = true;
	SMacDrawContextData data(fWindow, clearColor, doubleBuffer);
	X3DMacDrawContext myDrawContext(data);
	fView.SetDrawContext(myDrawContext);
	
	// Create and set renderer.
	X3DInteractiveRenderer myRenderer;
	myRenderer.SetDoubleBufferBypass(true);						
	myRenderer.SetVendorID(kQAVendor_BestChoice);	// use the hardware renderer if it is installed
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

Here's how this code would look with the raw API calls:

static TQ3ViewObject QD3DSupport_NewView(WindowPtr theWindow)
{
	TQ3Status				myStatus;
	TQ3ViewObject			myView;
	TQ3DrawContextObject	myDrawContext;
	TQ3RendererObject		myRenderer;
	TQ3GroupObject			myLights;
	TQ3CameraObject			myCamera;

	myView = Q3View_New();
	if (myView == nil)
		goto bail;

	//	Create and set draw context.
	if ((myDrawContext = QD3DSupport_NewDrawContext(theWindow)) == nil )
		goto bail;
		
	if ((myStatus = Q3View_SetDrawContext(myView, myDrawContext)) == kQ3Failure )
		goto bail;

	Q3Object_Dispose( myDrawContext );
	
	//	Create and set renderer.	
	if ((myRenderer = Q3Renderer_NewFromType(kQ3RendererTypeInteractive)) != nil )
	{
		if ((myStatus = Q3View_SetRenderer(myView, myRenderer)) == kQ3Failure )
			goto bail;
			
		// these two lines set us up to use the best possible renderer,
		// including  hardware if it is installed.
		Q3InteractiveRenderer_SetDoubleBufferBypass (myRenderer, kQ3True);						
		Q3InteractiveRenderer_SetPreferences(myRenderer, kQAVendor_BestChoice, 0);

	}
	else
		goto bail;

	Q3Object_Dispose( myRenderer );
	
	//	Create and set camera.
	if ( (myCamera = QD3DSupport_NewCamera((float) (theWindow->portRect.right - theWindow->portRect.left),
											(float) (theWindow->portRect.bottom - theWindow->portRect.top))) == nil )
		goto bail;

	if ((myStatus = Q3View_SetCamera(myView, myCamera)) == kQ3Failure )
		goto bail;
	Q3Object_Dispose( myCamera );
	
	//	Create and set lights.
	if ((myLights = QD3DSupport_NewLights()) == nil )
		goto bail;
		
	if ((myStatus = Q3View_SetLightGroup(myView, myLights)) == kQ3Failure )
		goto bail;
		
	Q3Object_Dispose(myLights);

	//	Done!!!
	return ( myView );
	
bail:
	//	If any of the above failed, then don't return a view.
	return ( nil );
}

Note that the Whisper code is much easier to read and does a better job handling errors
because it will properly clean up in the event of an error.

