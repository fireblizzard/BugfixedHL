#include <cassert>
#include <clocale>

#include "tier0/dbg.h"

#if USE_VGUI2
#include <vgui/VGUI2.h>
#include <vgui/IVGui.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/Frame.h>

#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

#include "IGameUIFuncs.h"
#include "IBaseUI.h"

#include "CHudViewport.h"
#endif

#include "KeyValuesCompat.h"

#include "VGUI2Paths.h"
#include "CClientVGUI.h"
#include "IEngineVgui.h"
#include "CHudBase.h"
#include "cl_util.h"

static SpewOutputFunc_t g_fnDefaultSpewFunc = nullptr;

namespace
{
CClientVGUI g_ClientVGUI;

IGameUIFuncs* g_GameUIFuncs = nullptr;

IBaseUI* g_pBaseUI = nullptr;

IEngineVGui *g_EngineVgui = nullptr;
}

CClientVGUI* clientVGUI()
{
	return &g_ClientVGUI;
}

IGameUIFuncs* gameUIFuncs()
{
	return g_GameUIFuncs;
}

IBaseUI* baseUI()
{
	return g_pBaseUI;
}

IEngineVGui* engineVgui()
{
	return g_EngineVgui;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CClientVGUI, IClientVGUI, ICLIENTVGUI_NAME, g_ClientVGUI );

CClientVGUI::CClientVGUI()
{
}

void CClientVGUI::Initialize( CreateInterfaceFn* pFactories, int iNumFactories )
{
	/*
	*	Factories in the given array:
	*	engine
	*	vgui2
	*	filesystem
	*	chrome HTML
	*	GameUI
	*	client (this library)
	*/

	//4 factories to use.
	assert( static_cast<size_t>( iNumFactories ) >= NUM_FACTORIES - 1 );

	m_FactoryList[ 0 ] = Sys_GetFactoryThis();

	for( size_t uiIndex = 0; uiIndex < NUM_FACTORIES - 1; ++uiIndex )
	{
		m_FactoryList[ uiIndex + 1 ] = pFactories[ uiIndex ];
	}

	// Redirect spew to game console
	g_fnDefaultSpewFunc = GetSpewOutputFunc();
	SpewOutputFunc([](SpewType_t spewType, tchar const *pMsg) -> SpewRetval_t
	{
		if (spewType == SPEW_ASSERT)
		{
			ConPrintf(RGBA(247, 83, 74), "%s", pMsg);	// Print in red
			return SPEW_DEBUGGER;
		}
		else if (spewType == SPEW_ERROR)
			ConPrintf(RGBA(247, 83, 74), "%s", pMsg);	// Print in red
		else
			ConPrintf("%s", pMsg);
		return SPEW_CONTINUE;
	});

	if( !vgui2::VGui_InitInterfacesList( "CLIENT", m_FactoryList, NUM_FACTORIES ) )
	{
		Msg( "Failed to initialize VGUI2\n" );
		return;
	}

	if( !KV_InitKeyValuesSystem( m_FactoryList, NUM_FACTORIES ) )
	{
		Msg( "Failed to initialize IKeyValues\n" );
		return;
	}

	g_GameUIFuncs = ( IGameUIFuncs* ) pFactories[ 0 ]( IGAMEUIFUNCS_NAME, nullptr );
	g_pBaseUI = ( IBaseUI* ) pFactories[ 0 ]( IBASEUI_NAME, nullptr );
	g_EngineVgui = (IEngineVGui* ) pFactories[ 0 ](VENGINE_VGUI_VERSION, nullptr );

	// Add language files
	vgui2::localize()->AddFile(vgui2::filesystem(), UI_LANGUAGE_DIR "/bugfixedhl_%language%.txt");

	//Constructor sets itself as the viewport.
	new CHudViewport();

	g_pViewport->Initialize( pFactories, iNumFactories );
}

void CClientVGUI::Start()
{
#if USE_VGUI2
	g_pViewport->Start();

#if 0
	vgui2::Frame* pFrame = new vgui2::Frame(g_pViewport, "TestFrame");
	pFrame->SetProportional(false);
	pFrame->SetTitleBarVisible(true);
	pFrame->SetMinimizeButtonVisible(false);
	pFrame->SetMaximizeButtonVisible(false);
	pFrame->SetCloseButtonVisible(true);
	pFrame->SetSizeable(true);
	pFrame->SetMoveable(true);
	pFrame->SetVisible(true);
	//pFrame->SetScheme(vgui2::scheme()->LoadSchemeFromFile("ui/resource/SourceScheme.res", "SourceScheme"));
	pFrame->SetSize(500, 500);
	pFrame->SetTitle("Test Frame", true);
	pFrame->Activate();
#endif
#endif
}

void CClientVGUI::SetParent( vgui2::VPANEL parent )
{
	m_vRootPanel = parent;

#if USE_VGUI2
	g_pViewport->SetParent( parent );
#endif
}

int CClientVGUI::UseVGUI1()
{
#if USE_VGUI2
	return g_pViewport->UseVGUI1();
#else
	return true;
#endif
}

void CClientVGUI::HideScoreBoard()
{
#if USE_VGUI2
	g_pViewport->HideScoreBoard();
#endif
}

void CClientVGUI::HideAllVGUIMenu()
{
#if USE_VGUI2
	g_pViewport->HideAllVGUIMenu();
#endif
}

void CClientVGUI::ActivateClientUI()
{
#if USE_VGUI2
	g_pViewport->ActivateClientUI();
#endif
}

void CClientVGUI::HideClientUI()
{
#if USE_VGUI2
	g_pViewport->HideClientUI();
#endif
}

void CClientVGUI::Shutdown()
{
#if USE_VGUI2
	g_pViewport->Shutdown();
#endif
	SpewOutputFunc(g_fnDefaultSpewFunc);
}
