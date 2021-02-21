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
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#pragma once
/*
enum class EAmmoType : unsigned short
{
	NONE = 0,

	// Sniper & Hunter
	_338Magnum,

	// Assault
	_762Nato,
	_556Nato,
	_556NatoBox,

	// Buckshot
	Buckshot,

	// PDWs
	_57mm,

	// High retarding force
	_45acp,
	_50AE,
	_9mm,

	// Equipments
	HEGrenade,
	Flashbang,
	SmokeGrenade,
	C4,

	// util valus.
	MAXTYPE,
};

struct AmmoInfo
{
	EAmmoType m_iId;
	const char* m_pszName;
	unsigned m_iMax;
	unsigned m_iCountPerBox;
	unsigned m_iCostPerBox;
	unsigned m_iPenetrationPower;
	float m_flPenetrationDistance;
};

extern const AmmoInfo g_rgAmmoInfo[static_cast<unsigned>(EAmmoType::MAXTYPE)];*/

class CBasePlayerAmmo: public CBaseEntity
{
public:
	virtual void Spawn() override;
	virtual BOOL AddAmmo(CBaseEntity *pOther);
	virtual CBaseEntity *Respawn() override;

public:
	void EXPORT DefaultTouch(CBaseEntity *pOther);
	void EXPORT Materialize();
};

class C9MMAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class CBuckShotAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C556NatoAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C556NatoBoxAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C762NatoAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C45ACPAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C50AEAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C338MagnumAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C57MMAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};

class C357SIGAmmo: public CBasePlayerAmmo
{
public:
	virtual void Spawn() final;
	virtual void Precache() final;
};
