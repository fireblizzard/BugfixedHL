#ifndef CSCOREPANEL_H
#define CSCOREPANEL_H

#include <cdll_dll.h>
#include <vgui_controls/Frame.h>
#include "vgui2/IViewportPanel.h"
#include "vgui2/ViewportPanelNames.h"

class IViewport;
class CHudScoreBoard;

namespace vgui2
{
	class Label;
	class TextEntry;
}

class CPlayerListPanel;

class CScorePanel : public vgui2::Frame, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CScorePanel, vgui2::Frame);

	// column widths at 640
	enum
	{
		NAME_WIDTH = 184,
		STEAMID_WIDTH = 100,
		EFF_WIDTH = 60,
		SCORE_WIDTH = 60,
		DEATH_WIDTH = 60,
		PING_WIDTH = 80
	};
	// total = 544

public:
	CScorePanel(IViewport *pParent);
	virtual ~CScorePanel();

	void FullUpdate();
	void UpdateClientInfo(int client, bool autoUpdate = true);	// manualUpdate - whether to update player count and resize at the end of client update
	void UpdateAllClients();
	void EnableMousePointer(bool enable);

	//IViewportPanel overrides
	const char *GetName() override
	{
		return VIEWPORT_PANEL_SCORE;
	}

	void SetData(KeyValues *data) override {}
	void Reset() override;
	void Update() override {}
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	virtual void OnKeyCodeTyped(vgui2::KeyCode code);

	bool NeedsUpdate() override
	{
		return false;
	}

	bool HasInputElements() override
	{
		return true;
	}

	void ShowPanel(bool state) override;

	// VGUI functions:
	vgui2::VPANEL GetVPanel() override final
	{
		return BaseClass::GetVPanel();
	}

	bool IsVisible() override final
	{
		return BaseClass::IsVisible();
	}

	void SetParent(vgui2::VPANEL parent) override final
	{
		BaseClass::SetParent(parent);
	}

	friend class CHudScoreBoard;

private:
	struct team_info_t
	{
		char name[MAX_TEAM_NAME] = { 0 };
		int kills = 0;	// Calculated kills
		int deaths = 0;	// Calculated deaths
		int players = 0;// Number of players
	};

	static CScorePanel *m_sSingleton;
	IViewport *m_pViewport;
	CPlayerListPanel *m_pPlayerList = nullptr;
	vgui2::Label *m_pServerNameLabel = nullptr;
	vgui2::Label *m_pMapNameLabel = nullptr;
	vgui2::Label *m_pPlayerCountLabel = nullptr;

	int m_pHeader = 0;
	int m_iPlayerCount = 0;
	int m_iMargin = 100;
	int m_iMinHeight = 320;

	team_info_t m_pTeamInfo[MAX_TEAMS + 1];
	int m_pClientItems[MAX_PLAYERS + 1];
	int m_pClientTeams[MAX_PLAYERS + 1];

	void RecalcItems();
	void UpdateServerName();
	void UpdateMapName();
	void UpdatePlayerCount();
	void AddHeader();
	void Resize();

	static bool StaticPlayerSortFunc(CPlayerListPanel *list, int itemID1, int itemID2);
};

#endif