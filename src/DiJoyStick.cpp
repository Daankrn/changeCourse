#include "DiJoyStick.h"

    DiJoyStick::DiJoyStick(){
		g_pEffect = nullptr;
    }
 
    DiJoyStick::~DiJoyStick() {
        clear();
    }
 
    void DiJoyStick::clear() {
        /*if(entry) {
            delete [] entry;
            entry = 0;
        }*/
        maxEntry    = 0;
        nEntry      = 0;
        di          = 0;
    }


    void DiJoyStick::enumerate(LPDIRECTINPUT di, DWORD dwDevType, LPCDIDATAFORMAT lpdf, DWORD dwFlags, int maxEntry) {
        clear();
 
        entry           = new Entry [maxEntry];
        this->di        = di;
        this->maxEntry  = maxEntry;
        nEntry          = 0;
        this->lpdf      = lpdf;
 
        di->EnumDevices(dwDevType, DIEnumDevicesCallback_static, this, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
 
        this->di        = 0;
    }
 
    int DiJoyStick::getEntryCount() {
        return nEntry;
    }
 
    const DiJoyStick::Entry* DiJoyStick::getEntry(int index) {
        const Entry* e = 0;
        if(index >= 0 && index < nEntry) {
            e = &entry[index];
        }
        return e;
    }
 
    void DiJoyStick::update() {
		//cout << nEntry << endl;
        for(int iEntry = 0; iEntry < nEntry; ++iEntry) {
            Entry& e = entry[iEntry];
            LPDIRECTINPUTDEVICE8 d = e.diDevice;
			//cout << "updating " << iEntry << endl;
            if(FAILED(d->Poll())) {
                HRESULT hr = d->Acquire();
				
                while(hr == DIERR_INPUTLOST) {
                    hr = d->Acquire();
                }
				if( g_pEffect )
                    g_pEffect->Start( 1, 0 ); // Start the effect
            } else {
                d->GetDeviceState(sizeof(DIJOYSTATE2), &e.joystate);
            }
        }
    }

	BOOL CALLBACK DiJoyStick::DIEnumDevicesCallback_static(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
		cout << "at DIEnumDevicesCallBackStatic" <<endl;
        return reinterpret_cast<DiJoyStick*>(pvRef)->DIEnumDevicesCallback(lpddi, pvRef);
    }
 
    BOOL DiJoyStick::DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
        if(nEntry < maxEntry) {
            DiJoyStick::Entry e = { 0 };
 
            memcpy(&e.diDeviceInstance, lpddi, sizeof(e.diDeviceInstance));
            e.diDevCaps.dwSize = sizeof(e.diDevCaps);
 
            LPDIRECTINPUTDEVICE8    did = 0;
			HWND hwnd = ofGetWin32Window();
			 DIPROPDWORD dipdw;
            if(SUCCEEDED(di->CreateDevice(lpddi->guidInstance, (LPDIRECTINPUTDEVICE*) &did, 0))) {
                if(SUCCEEDED(did->SetDataFormat(lpdf))) {
                    if(SUCCEEDED(did->GetCapabilities(&e.diDevCaps))) {
						if(SUCCEEDED(did->SetCooperativeLevel( hwnd,
                                                     DISCL_EXCLUSIVE |
                                                     DISCL_FOREGROUND ) ) ){
													// Since we will be playing force feedback effects, we should disable the
													// auto-centering spring.
													dipdw.diph.dwSize = sizeof( DIPROPDWORD );
													dipdw.diph.dwHeaderSize = sizeof( DIPROPHEADER );
													dipdw.diph.dwObj = 0;
													dipdw.diph.dwHow = DIPH_DEVICE;
													dipdw.dwData = FALSE;

													did->SetProperty( DIPROP_AUTOCENTER, &dipdw.diph );
													// and set effects:
													// This application needs only one effect: Applying raw forces.
    DWORD rgdwAxes[2] = { DIJOFS_X, DIJOFS_Y };
    LONG rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf = { 0 };

    DIEFFECT eff;
    ZeroMemory( &eff, sizeof( eff ) );
    eff.dwSize = sizeof( DIEFFECT );
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.dwDuration = INFINITE;
    eff.dwSamplePeriod = 0;
    eff.dwGain = DI_FFNOMINALMAX;
    eff.dwTriggerButton = DIEB_NOTRIGGER;
    eff.dwTriggerRepeatInterval = 0;
    eff.cAxes = 1;
    eff.rgdwAxes = rgdwAxes;
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = 0;
    eff.cbTypeSpecificParams = sizeof( DICONSTANTFORCE );
    eff.lpvTypeSpecificParams = &cf;
    eff.dwStartDelay = 0;
    // Create the prepared effect
    did->CreateEffect( GUID_ConstantForce,
                                              &eff, &g_pEffect, nullptr );
   
													e.diDevice = did;
													entry[nEntry++] = e;
						}
                    }
                }
            }
        }
        return DIENUM_CONTINUE;
    }

//-----------------------------------------------------------------------------
// Name: CoordToForce()
// Desc: Convert a coordinate 0 <= nCoord <= maxInput 
//       to a force value in the range -DI_FFNOMINALMAX to +DI_FFNOMINALMAX.
//-----------------------------------------------------------------------------
INT DiJoyStick::CoordToForce( INT nCoord, INT max )
{
    INT nForce = MulDiv( nCoord, 2 * DI_FFNOMINALMAX, max )
        - DI_FFNOMINALMAX;

    // Keep force within bounds
    if( nForce < -DI_FFNOMINALMAX )
        nForce = -DI_FFNOMINALMAX;

    if( nForce > +DI_FFNOMINALMAX )
        nForce = +DI_FFNOMINALMAX;

    return nForce;
}

//-----------------------------------------------------------------------------
// Name: SetDeviceForcesXY()
// Desc: Apply the X and Y forces to the effect we prepared.
//-----------------------------------------------------------------------------
HRESULT DiJoyStick::SetDeviceForcesXY(int x, int max)
{
    // Modifying an effect is basically the same as creating a new one, except
    // you need only specify the parameters you are modifying
    LONG rglDirection[2] = { 0, 0 };
    DICONSTANTFORCE cf;

  g_nXForce = CoordToForce(x,max);
        // If only one force feedback axis, then apply only one direction and 
        // keep the direction at zero
        cf.lMagnitude = g_nXForce;
        rglDirection[0] = 0;

    DIEFFECT eff;
    ZeroMemory( &eff, sizeof( eff ) );
    eff.dwSize = sizeof( DIEFFECT );
    eff.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    eff.cAxes = 1; //hard coded 1 axis
    eff.rglDirection = rglDirection;
    eff.lpEnvelope = 0;
    eff.cbTypeSpecificParams = sizeof( DICONSTANTFORCE );
    eff.lpvTypeSpecificParams = &cf;
    eff.dwStartDelay = 0;

    // Now set the new parameters and start the effect immediately.
    return g_pEffect->SetParameters( &eff, DIEP_DIRECTION |
                                     DIEP_TYPESPECIFICPARAMS |
                                     DIEP_START );
}