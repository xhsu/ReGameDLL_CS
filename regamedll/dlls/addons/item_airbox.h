/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/

#pragma once

class CItemAirBox: public CArmoury
{
public:
	~CItemAirBox() final;

public:
	void	Spawn		(void) final;
	void	Precache	(void) final;
	void	Restart		(void) final;
	void	Touch		(CBaseEntity *pOther) final;
	void	KeyValue	(KeyValueData *pkvd) final;
	int		ObjectCaps	(void) final { return (CArmoury::ObjectCaps() | FCAP_MUST_RESET); }

protected:
	void EXPORT MoveUp();

private:
	EntityHandle<CSprite> m_hSprite;

	float	m_flyup{ 0.0f };
	float	m_delay{ 0.0f };

	Vector	m_rendercolor	{ g_vecZero };
	int		m_renderfx		{ 0 };
	int		m_rendermode	{ 0 };
	float	m_renderamt		{ 0.0f };
	float	m_scale			{ 0.0f };
	float	m_frameRate		{ 0.0f };

	string_t	m_iszSpriteName{ 0 };
};
