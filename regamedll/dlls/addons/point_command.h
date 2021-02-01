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

#include <utlvector.h>

const int MAX_POINT_CMDS = 16; // maximum number of commands a single point_[server/client]command entity may be assigned

#define SF_POINT_CMD_NORESET BIT(0) // it is not allowed to be resetting to initial value on remove an entity or change level

class CPointBaseCommand: public CPointEntity {
public:
	virtual ~CPointBaseCommand() override;

public:
	virtual void KeyValue(KeyValueData *pkvd) override;
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) = 0;
	virtual void Execute(edict_t *pEdict, const char *pszFmt, ...) = 0;

protected:

	class command_t
	{
	public:
		command_t(const char *_name, const char *_value = nullptr)
		{
			value[0] = '\0';
			valueInitial[0] = '\0';

			Q_strlcpy(name, _name);

			if (_value)
			{
				Q_strlcpy(value, _value);
			}
		}

		char name[64U], value[64U], valueInitial[64U];
	};

	std::vector<command_t> m_vecCommands;
};

// It issues commands to the client console
class CPointClientCommand: public CPointBaseCommand {
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

private:
	void Execute(edict_t *pEdict, const char *pszFmt, ...);
};

// It issues commands to the server console
class CPointServerCommand: public CPointBaseCommand {
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

private:
	void Execute(edict_t *pEdict, const char *pszFmt, ...);
};
