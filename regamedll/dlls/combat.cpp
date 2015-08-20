#include "precompiled.h"

#define GERMAN_GIB_COUNT	4
#define HUMAN_GIB_COUNT		6
#define ALIEN_GIB_COUNT		4

/* <5f4cb> ../cstrike/dlls/combat.cpp:52 */
void CGib::LimitVelocity(void)
{
	float length = pev->velocity.Length();

	// ceiling at 1500.  The gib velocity equation is not bounded properly.  Rather than tune it
	// in 3 separate places again, I'll just limit it here.
	if (length > 1500.0)
	{
		// This should really be sv_maxvelocity * 0.75 or something
		pev->velocity = pev->velocity.Normalize() * 1500;
	}
}

/* <60320> ../cstrike/dlls/combat.cpp:63 */
NOBODY void CGib::SpawnStickyGibs(entvars_t *pevVictim, Vector vecOrigin, int cGibs)
{
//	{
//		int i;                                                //    65
//		{
//			class CGib *pGib;                            //    75
//			GetClassPtr<CGib>(CGib *a);  //    75
//			operator*(const Vector *const this,
//					float fl);  //    93
//			operator*(const Vector *const this,
//					float fl);  //   100
//			Instance(entvars_t *pev);  //   106
//			operator*(const Vector *const this,
//					float fl);  //   110
//			operator*(const Vector *const this,
//					float fl);  //   118
//			Vector(Vector *const this,
//				float X,
//				float Y,
//				float Z);  //   124
//			Vector(Vector *const this,
//				float X,
//				float Y,
//				float Z);  //   124
//			LimitVelocity(CGib *const this);  //   128
//			operator*(const Vector *const this,
//					float fl);  //   114
//		} 
//	} 
}

void (*pCGib__SpawnHeadGib)(void);

/* <5ff7f> ../cstrike/dlls/combat.cpp:132 */
void  __declspec(naked) CGib::SpawnHeadGib(entvars_t *pevVictim)
{
	__asm
	{
		jmp pCGib__SpawnHeadGib
	}
}

/* <606c8> ../cstrike/dlls/combat.cpp:190 */
void CGib::SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int human)
{
	int cSplat;
	for (cSplat = 0; cSplat < cGibs; cSplat++)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL);

		if (g_Language == LANGUAGE_GERMAN)
		{
			pGib->Spawn("models/germangibs.mdl");
			pGib->pev->body = RANDOM_LONG(0, GERMAN_GIB_COUNT - 1);
		}
		else
		{
			if (human)
			{
				// human pieces
				pGib->Spawn("models/hgibs.mdl");
				// start at one to avoid throwing random amounts of skulls (0th gib)
				pGib->pev->body = RANDOM_LONG(1, HUMAN_GIB_COUNT - 1);
			}
			else
			{
				// aliens
				pGib->Spawn("models/agibs.mdl");
				pGib->pev->body = RANDOM_LONG(0, ALIEN_GIB_COUNT - 1);
			}
		}

		if (pevVictim)
		{
			// spawn the gib somewhere in the monster's bounding volume
			pGib->pev->origin.x = pevVictim->absmin.x + pevVictim->size.x * (RANDOM_FLOAT(0, 1));
			pGib->pev->origin.y = pevVictim->absmin.y + pevVictim->size.y * (RANDOM_FLOAT(0, 1));
			// absmin.z is in the floor because the engine subtracts 1 to enlarge the box
			pGib->pev->origin.z = pevVictim->absmin.z + pevVictim->size.z * (RANDOM_FLOAT(0, 1)) + 1;

			// make the gib fly away from the attack vector
			pGib->pev->velocity = g_vecAttackDir * -1;

			// mix in some noise
			pGib->pev->velocity.x += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.y += RANDOM_FLOAT(-0.25, 0.25);
			pGib->pev->velocity.z += RANDOM_FLOAT(-0.25, 0.25);

			pGib->pev->velocity = pGib->pev->velocity * RANDOM_FLOAT(300, 400);

			pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
			pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);

			// copy owner's blood color
			pGib->m_bloodColor = (CBaseEntity::Instance(pevVictim))->BloodColor();

			if (pevVictim->health > -50)
				pGib->pev->velocity = pGib->pev->velocity * 0.7;

			else if (pevVictim->health > -200)
				pGib->pev->velocity = pGib->pev->velocity * 2;
			else
				pGib->pev->velocity = pGib->pev->velocity * 4;

			pGib->pev->solid = SOLID_BBOX;
			UTIL_SetSize(pGib->pev, Vector(0, 0, 0), Vector(0, 0, 0));
		}
		pGib->LimitVelocity();
	}
}

/* <5f58a> ../cstrike/dlls/combat.cpp:263 */
BOOL CBaseMonster::__MAKE_VHOOK(HasHumanGibs)(void)
{
	int myClass = Classify();

	if (myClass == CLASS_HUMAN_MILITARY
		|| myClass == CLASS_PLAYER_ALLY
		|| myClass == CLASS_HUMAN_PASSIVE
		|| myClass == CLASS_PLAYER)
		 return TRUE;

	return FALSE;
}

/* <5f5ca> ../cstrike/dlls/combat.cpp:278 */
BOOL CBaseMonster::__MAKE_VHOOK(HasAlienGibs)(void)
{
	int myClass = Classify();
	if (myClass == CLASS_ALIEN_MILITARY
		|| myClass == CLASS_ALIEN_MONSTER
		|| myClass == CLASS_ALIEN_PASSIVE
		|| myClass == CLASS_INSECT
		|| myClass == CLASS_ALIEN_PREDATOR
		|| myClass == CLASS_ALIEN_PREY)
		 return TRUE;

	return FALSE;
}

/* <5f60a> ../cstrike/dlls/combat.cpp:295 */
void CBaseMonster::__MAKE_VHOOK(FadeMonster)(void)
{
	StopAnimation();

	pev->velocity = g_vecZero;
	pev->movetype = MOVETYPE_NONE;
	pev->avelocity = g_vecZero;
	pev->animtime = gpGlobals->time;
	pev->effects |= EF_NOINTERP;

	SUB_StartFadeOut();
}

/* <60a59> ../cstrike/dlls/combat.cpp:310 */
void CBaseMonster::__MAKE_VHOOK(GibMonster)(void)
{
	TraceResult tr;
	BOOL gibbed = FALSE;

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/bodysplat.wav", VOL_NORM, ATTN_NORM);

	// only humans throw skulls !!!UNDONE - eventually monsters will have their own sets of gibs
	if (HasHumanGibs())
	{
		// Only the player will ever get here
		if (CVAR_GET_FLOAT("violence_hgibs") != 0)
		{
			CGib::SpawnHeadGib(pev);

			// throw some human gibs.
			CGib::SpawnRandomGibs(pev, 4, 1);
		}
		gibbed = TRUE;
	}
	else if (HasAlienGibs())
	{
		// Should never get here, but someone might call it directly
		if (CVAR_GET_FLOAT("violence_agibs") != 0)
		{
			// Throw alien gibs
			CGib::SpawnRandomGibs(pev, 4, 0);
		}
		gibbed = TRUE;
	}

	if (!IsPlayer())
	{
		if (gibbed)
		{
			// don't remove players!
			SetThink(&CBaseMonster::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
		else
			FadeMonster();
	}
}

/* <5f65e> ../cstrike/dlls/combat.cpp:355 */
NOBODY Activity CBaseMonster::__MAKE_VHOOK(GetDeathActivity)(void)
{
//	{
//		Activity deathActivity;                               //   357
//		BOOL fTriedDirection;                                 //   358
//		float flDot;                                          //   359
//		TraceResult tr;                                       //   360
//		class Vector vecSrc;                                  //   361
//		operator*(const Vector *const this,
//				float fl);  //   375
//		DotProduct(const Vector  &a,
//				const Vector  &b);  //   375
//		edict(CBaseEntity *const this);  //   461
//		operator*(const Vector *const this,
//				float fl);  //   461
//		operator-(const Vector *const this,
//				const Vector  &v);  //   461
//		edict(CBaseEntity *const this);  //   450
//		operator*(const Vector *const this,
//				float fl);  //   450
//		operator+(const Vector *const this,
//				const Vector  &v);  //   450
//	} 
}

/* <5f848> ../cstrike/dlls/combat.cpp:476 */
NOBODY Activity CBaseMonster::GetSmallFlinchActivity(void)
{
//	{ 
//		Activity flinchActivity;                              //   478
//		BOOL fTriedDirection;                                 //   479
//		float flDot;                                          //   480
//	} 
}

/* <5f8a6> ../cstrike/dlls/combat.cpp:525 */
void CBaseMonster::__MAKE_VHOOK(BecomeDead)(void)
{
	// don't let autoaim aim at corpses.
	pev->takedamage = DAMAGE_YES;

	// give the corpse half of the monster's original maximum health. 
	pev->health = pev->max_health / 2;

	// max_health now becomes a counter for how many blood decals the corpse can place.
	pev->max_health = 5;

	// make the corpse fly away from the attack vector
	pev->movetype = MOVETYPE_TOSS;
}

/* <5f8cc> ../cstrike/dlls/combat.cpp:542 */
BOOL CBaseMonster::ShouldGibMonster(int iGib)
{
	if ((iGib == GIB_NORMAL && pev->health < GIB_HEALTH_VALUE) || (iGib == GIB_ALWAYS))
		return TRUE;

	return FALSE;
}

/* <5f8f8> ../cstrike/dlls/combat.cpp:551 */
void CBaseMonster::CallGibMonster(void)
{
	BOOL fade = FALSE;

	if (HasHumanGibs())
	{
		if (CVAR_GET_FLOAT("violence_hgibs") == 0)
			fade = TRUE;
	}
	else if (HasAlienGibs())
	{
		if (CVAR_GET_FLOAT("violence_agibs") == 0)
			fade = TRUE;
	}

	// do something with the body. while monster blows up
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;

	if (fade)
	{
		FadeMonster();
	}
	else
	{
		// make the model invisible.
		pev->effects = EF_NODRAW;
		GibMonster();
	}

	pev->deadflag = DEAD_DEAD;
	FCheckAITrigger();

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99.0f)
	{
		pev->health = 0;
	}

	if (ShouldFadeOnDeath() && !fade)
		UTIL_Remove(this);
}

/* <5f938> ../cstrike/dlls/combat.cpp:598 */
void CBaseMonster::__MAKE_VHOOK(Killed)(entvars_t *pevAttacker, int iGib)
{
	// unsigned int cCount = 0;
	// BOOL fDone = FALSE;

	if (HasMemory(bits_MEMORY_KILLED))
	{
		if (ShouldGibMonster(iGib))
			CallGibMonster();
		return;
	}

	Remember(bits_MEMORY_KILLED);

	// clear the deceased's sound channels.(may have been firing or reloading when killed)
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);
	m_IdealMonsterState = MONSTERSTATE_DEAD;
	// Make sure this condition is fired too (TakeDamage breaks out before this happens on death)
	SetConditions(bits_COND_LIGHT_DAMAGE);

	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	if (pOwner)
	{
		pOwner->DeathNotice(pev);
	}

	if (ShouldGibMonster(iGib))
	{
		CallGibMonster();
		return;
	}
	else if (pev->flags & FL_MONSTER)
	{
		SetTouch(NULL);
		BecomeDead();
	}

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
	{
		pev->health = 0;
	}

	//pev->enemy = ENT( pevAttacker );//why? (sjb)
	m_IdealMonsterState = MONSTERSTATE_DEAD;
}

/* <5e84e> ../cstrike/dlls/combat.cpp:652 */
void CBaseEntity::SUB_StartFadeOut(void)
{
	if (pev->rendermode == kRenderNormal)
	{
		pev->renderamt = 255.0f;
		pev->rendermode = kRenderTransTexture;
	}

	pev->solid = SOLID_NOT;
	pev->avelocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.1f;

	SetThink(&CBaseEntity::SUB_FadeOut);
}

/* <5ea72> ../cstrike/dlls/combat.cpp:667 */
void CBaseEntity::SUB_FadeOut(void)
{
	if (pev->renderamt > 7)
	{
		pev->renderamt -= 7.0f;
		pev->nextthink = gpGlobals->time + 0.1f;
	}
	else
	{
		pev->renderamt = 0.0f;
		pev->nextthink = gpGlobals->time + 0.2f;
		SetThink(&CBaseEntity::SUB_Remove);
	}
}

/* <5eab1> ../cstrike/dlls/combat.cpp:688 */
void CGib::WaitTillLand(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->velocity == g_vecZero)
	{
		SetThink(&CBaseEntity::SUB_StartFadeOut);
		pev->nextthink = gpGlobals->time + m_lifeTime;

		if (m_bloodColor != DONT_BLEED)
			CSoundEnt::InsertSound(bits_SOUND_MEAT, pev->origin, 384, 25);
	}
	else
		pev->nextthink = gpGlobals->time + 0.5f;
}

/* <5eee0> ../cstrike/dlls/combat.cpp:718 */
void CGib::BounceGibTouch(CBaseEntity *pOther)
{
	if (pev->flags & FL_ONGROUND)
	{
		pev->velocity = pev->velocity * 0.9;
		pev->angles.x = 0;
		pev->angles.z = 0;
		pev->avelocity.x = 0;
		pev->avelocity.z = 0;
	}
	else
	{
		if (g_Language != LANGUAGE_GERMAN && m_cBloodDecals > 0 && m_bloodColor != DONT_BLEED)
		{
			TraceResult tr;
			Vector vecSpot = pev->origin + Vector(0, 0, 8);
			UTIL_TraceLine(vecSpot, vecSpot + Vector(0, 0, -24), ignore_monsters, ENT(pev), &tr);
			UTIL_BloodDecalTrace(&tr, m_bloodColor);
			m_cBloodDecals--;
		}

		if (m_material != matNone && !RANDOM_LONG(0, 2))
		{
			float zvel = fabs(pev->velocity.z);
			float volume = 0.8 * _min(1, zvel / 450);

			CBreakable::MaterialSoundRandom(edict(), (Materials)m_material, volume);
		}
	}
}

// Sticky gib puts blood on the wall and stays put.

/* <5ed6d> ../cstrike/dlls/combat.cpp:761 */
void CGib::StickyGibTouch(CBaseEntity *pOther)
{
	Vector vecSpot;
	TraceResult tr;

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 10;

	if (!FClassnameIs(pOther->pev, "worldspawn"))
	{
		pev->nextthink = gpGlobals->time;
		return;
	}

	vecSpot = pev->origin + pev->velocity * 32;

	UTIL_TraceLine(pev->origin, vecSpot, ignore_monsters, ENT(pev), &tr);
	UTIL_BloodDecalTrace(&tr, m_bloodColor);

	pev->velocity = tr.vecPlaneNormal * -1;
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->velocity = g_vecZero;
	pev->avelocity = g_vecZero;
	pev->movetype = MOVETYPE_NONE;
}

/* <5fb0b> ../cstrike/dlls/combat.cpp:789 */
void CGib::Spawn(const char *szGibModel)
{
	pev->movetype = MOVETYPE_BOUNCE;

	// deading the bounce a bit
	pev->friction = 0.55;

	// sometimes an entity inherits the edict from a former piece of glass,
	// and will spawn using the same render FX or rendermode! bad!
	pev->renderamt = 255.0;
	pev->rendermode = kRenderNormal;
	pev->renderfx = kRenderFxNone;

	/// hopefully this will fix the VELOCITY TOO LOW crap
	pev->solid = SOLID_SLIDEBOX;

	if (pev->classname)
		RemoveEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	MAKE_STRING_CLASS("gib", pev);
	AddEntityHashValue(pev, STRING(pev->classname), CLASSNAME);

	SET_MODEL(ENT(pev), szGibModel);
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	pev->nextthink = gpGlobals->time + 4.0f;
	m_lifeTime = 25.0f;

	SetThink(&CGib::WaitTillLand);
	SetTouch(&CGib::BounceGibTouch);

	m_material = matNone;

	// how many blood decals this gib can place (1 per bounce until none remain).
	m_cBloodDecals = 5;
}

/* <60aea> ../cstrike/dlls/combat.cpp:815 */
int CBaseMonster::__MAKE_VHOOK(TakeHealth)(float flHealth, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;

	// clear out any damage types we healed.
	// UNDONE: generic health should not heal any
	// UNDONE: time-based damage

	m_bitsDamageType &= ~(bitsDamageType & ~DMG_TIMEBASED);
	return CBaseEntity::TakeHealth(flHealth, bitsDamageType);
}

// The damage is coming from inflictor, but get mad at attacker
// This should be the only function that ever reduces health.
// bitsDamageType indicates the type of damage sustained, ie: DMG_SHOCK
//
// Time-based damage: only occurs while the monster is within the trigger_hurt.
// When a monster is poisoned via an arrow etc it takes all the poison damage at once.

/* <60d7a> ../cstrike/dlls/combat.cpp:845 */
int CBaseMonster::__MAKE_VHOOK(TakeDamage)(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	float flTake;
	Vector vecDir;

	if (pev->takedamage == DAMAGE_NO)
		return 0;

	if (!IsAlive())
	{
		return DeadTakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}

	if (pev->deadflag == DEAD_NO)
	{
		// no pain sound during death animation.
		PainSound();
	}

	//!!!LATER - make armor consideration here!
	flTake = flDamage;

	// set damage type sustained
	m_bitsDamageType |= bitsDamageType;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = Vector(0, 0, 0);

	if (!FNullEnt(pevInflictor))
	{
		CBaseEntity *pInflictor = CBaseEntity::Instance(pevInflictor);

		if (pInflictor)
		{
			vecDir = (pInflictor->Center() - Vector(0, 0, 10) - Center()).Normalize();
			vecDir = g_vecAttackDir = vecDir.Normalize();
		}
	}

	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// TODO: remove after combining shotgun blasts?
	if (IsPlayer())
	{
		if (pevInflictor)
			pev->dmg_inflictor = ENT(pevInflictor);

		pev->dmg_take += flTake;
	}

	pev->health -= flTake;

	if (m_MonsterState == MONSTERSTATE_SCRIPT)
	{
		SetConditions(bits_COND_LIGHT_DAMAGE);
		return 0;
	}

	if (pev->health <= 0.0f)
	{
		g_pevLastInflictor = pevInflictor;

		if (bitsDamageType & DMG_ALWAYSGIB)
			Killed(pevAttacker, GIB_ALWAYS);

		else if (bitsDamageType & DMG_NEVERGIB)
			Killed(pevAttacker, GIB_NEVER);
		else
			Killed(pevAttacker, GIB_NORMAL);

		g_pevLastInflictor = NULL;
		return 0;
	}
	if ((pev->flags & FL_MONSTER) && !FNullEnt(pevAttacker))
	{
		if (pevAttacker->flags & (FL_MONSTER | FL_CLIENT))
		{
			if (pevInflictor)
			{
				if (m_hEnemy == NULL || pevInflictor == m_hEnemy->pev || !HasConditions(bits_COND_SEE_ENEMY))
					m_vecEnemyLKP = pevInflictor->origin;
			}
			else
				m_vecEnemyLKP = pev->origin + (g_vecAttackDir * 64);

			MakeIdealYaw(m_vecEnemyLKP);

			if (flDamage > 20.0f)
				SetConditions(bits_COND_LIGHT_DAMAGE);

			if (flDamage >= 20.0f)
				SetConditions(bits_COND_HEAVY_DAMAGE);
		}
	}

	return 1;
}

// DeadTakeDamage - takedamage function called when a monster's corpse is damaged.

/* <60b32> ../cstrike/dlls/combat.cpp:981 */
int CBaseMonster::DeadTakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	Vector vecDir = Vector(0, 0, 0);

	if (!FNullEnt(pevInflictor))
	{
		CBaseEntity *pInflictor = CBaseEntity::Instance(pevInflictor);
		if (pInflictor)
		{
			vecDir = (pInflictor->Center() - Vector(0, 0, 10) - Center()).Normalize();
			vecDir = g_vecAttackDir = vecDir.Normalize();
		}
	}

// turn this back on when the bounding box issues are resolved.
#if 0

	pev->flags &= ~FL_ONGROUND;
	pev->origin.z += 1;

	// let the damage scoot the corpse around a bit.
	if (!FNullEnt(pevInflictor) && (pevAttacker->solid != SOLID_TRIGGER))
	{
		pev->velocity = pev->velocity + vecDir * -DamageForce(flDamage);
	}

#endif
	
	// kill the corpse if enough damage was done to destroy the corpse and the damage is of a type that is allowed to destroy the corpse.
	if (bitsDamageType & DMG_GIB_CORPSE)
	{
		if (pev->health <= flDamage)
		{
			pev->health = -50;
			Killed(pevAttacker, GIB_ALWAYS);
			return 0;
		}

		// Accumulate corpse gibbing damage, so you can gib with multiple hits
		pev->health -= flDamage * 0.1;
	}

	return 1;
}

/* <61120> ../cstrike/dlls/combat.cpp:1027 */
float CBaseMonster::DamageForce(float damage)
{
	float_precision force = damage * ((32 * 32 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5;

	if (force > 1000.0)
	{
		force = 1000.0;
	}
	return force;
}

/* <61174> ../cstrike/dlls/combat.cpp:1044 */
void RadiusFlash(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType)
{
	CBaseEntity *pEntity = NULL;
	TraceResult tr;
	float flAdjustedDamage, falloff;
	Vector vecSpot;
	float flRadius = 1500;

	if (flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1;

	int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);
	
	vecSrc.z += 1;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, 1500.0)) != NULL)
	{
		TraceResult tr2;
		Vector vecLOS;
		float flDot;
		float fadeTime;
		float fadeHold;
		int alpha;
		CBasePlayer *pPlayer;
		float currentHoldTime;

		if (!pEntity->IsPlayer())
			continue;

		pPlayer = (CBasePlayer *)pEntity;

		if (pPlayer->pev->takedamage == DAMAGE_NO || pPlayer->pev->deadflag != DEAD_NO)
			continue;

		if (bInWater && pPlayer->pev->waterlevel == 0)
			continue;

		if (!bInWater && pPlayer->pev->waterlevel == 3)
			continue;

		vecSpot = pPlayer->BodyTarget(vecSrc);
		UTIL_TraceLine(vecSrc, vecSpot, dont_ignore_monsters, ENT(pevInflictor), &tr);

		if (tr.flFraction != 1.0f && tr.pHit != pPlayer->pev->pContainingEntity)
			continue;

		UTIL_TraceLine(vecSpot, vecSrc, dont_ignore_monsters, tr.pHit, &tr2);

		if (tr2.flFraction >= 1.0)
		{
			if (tr.fStartSolid)
			{
				tr.vecEndPos = vecSrc;
				tr.flFraction = 0;
			}

			flAdjustedDamage = flDamage - (vecSrc - tr.vecEndPos).Length() * falloff;

			if (flAdjustedDamage < 0)
				flAdjustedDamage = 0;

			UTIL_MakeVectors(pPlayer->pev->v_angle);
			vecLOS = vecSrc - pPlayer->EarPosition();
			flDot = DotProduct(vecLOS, gpGlobals->v_forward);

			if (flDot < 0)
			{
				alpha = 200;
				fadeTime = flAdjustedDamage * 1.75;
				fadeHold = flAdjustedDamage / 3.5;
			}
			else
			{
				alpha = 255;
				fadeTime = flAdjustedDamage * 3;
				fadeHold = flAdjustedDamage / 1.5;
			}

			currentHoldTime = pPlayer->m_blindStartTime + pPlayer->m_blindHoldTime - gpGlobals->time;
							
			if (currentHoldTime > 0.0 && alpha == 255)
				fadeHold += currentHoldTime;

			if (pPlayer->m_blindStartTime != 0.0f && pPlayer->m_blindFadeTime != 0.0f)
			{
				if ((pPlayer->m_blindStartTime + pPlayer->m_blindFadeTime + pPlayer->m_blindHoldTime) > gpGlobals->time)
				{
					if (pPlayer->m_blindFadeTime > fadeTime)
						fadeTime = pPlayer->m_blindFadeTime;

					if (pPlayer->m_blindAlpha >= alpha)
						alpha = pPlayer->m_blindAlpha;
				}
			}

			UTIL_ScreenFade(pPlayer, Vector(255, 255, 255), fadeTime, fadeHold, alpha, 0);

			for (int i = 1; i <= gpGlobals->maxClients; i++)
			{
				CBasePlayer *pObserver = (CBasePlayer *)UTIL_PlayerByIndex(i);

				if (!pObserver || !pObserver->IsObservingPlayer(pPlayer))
					continue;

				if (!fadetoblack.value)
					UTIL_ScreenFade(pPlayer, Vector(255, 255, 255), fadeTime, fadeHold, alpha, 0);
			}

			pPlayer->Blind(fadeTime * 0.33, fadeHold, fadeTime, alpha);

			if (TheBots)
			{
				TheBots->OnEvent(EVENT_PLAYER_BLINDED_BY_FLASHBANG, pPlayer);
			}
		}
	}
}

/* <5f042> ../cstrike/dlls/combat.cpp:1185 */
float GetAmountOfPlayerVisible(Vector vecSrc, CBaseEntity *entity)
{
	float retval = 0.0f;
	TraceResult tr;
	Vector spot;
	Vector2D dir;
	Vector2D perp;

	const float topOfHead = 25.0f;
	const float standFeet = 34.0f;
	const float crouchFeet = 14.0f;
	const float edgeOffset = 13.0f;

	if (!entity->IsPlayer())
	{
		UTIL_TraceLine(vecSrc, entity->pev->origin, ignore_monsters, NULL, &tr);

		if (tr.flFraction == 1.0f)
			retval = 1.0f;

		return retval;
	}

	UTIL_TraceLine(vecSrc, entity->pev->origin, ignore_monsters, NULL, &tr);

	if (tr.flFraction == 1.0f)
		retval += 0.4f;

	spot = entity->pev->origin + Vector(0, 0, topOfHead);
	UTIL_TraceLine(vecSrc, spot, ignore_monsters, NULL, &tr);

	if (tr.flFraction == 1.0f)
		retval += 0.2f;

	spot = entity->pev->origin;
	if (entity->pev->flags & FL_DUCKING)
		spot.z -= crouchFeet;
	else
		spot.z -= standFeet;

	UTIL_TraceLine(vecSrc, spot, ignore_monsters, NULL, &tr);

	if (tr.flFraction == 1.0f)
		retval += 0.2f;

	dir = (entity->pev->origin - vecSrc).Make2D();
	dir.NormalizeInPlace();

	perp.x = -dir.y * edgeOffset;
	perp.y = dir.x * edgeOffset;

	spot = entity->pev->origin + Vector(perp.x, perp.y, 0);

	UTIL_TraceLine(vecSrc, spot, ignore_monsters, NULL, &tr);

	if (tr.flFraction == 1.0f)
		retval += 0.1;

	spot = entity->pev->origin - Vector(perp.x, perp.y, 0);

	UTIL_TraceLine(vecSrc, spot, ignore_monsters, NULL, &tr);

	if (tr.flFraction == 1.0f)
		retval += 0.1;

	return retval;
}

/* <5dc27> ../cstrike/dlls/combat.cpp:1262 */
void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	CBaseEntity *pEntity = NULL;
	TraceResult tr;
	float flAdjustedDamage, falloff;
	Vector vecSpot;

	if (flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1.0;

	int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

	// in case grenade is lying on the ground
	vecSrc.z += 1;

	if (!pevAttacker)
		pevAttacker = pevInflictor;

	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			// UNDONE: this should check a damage mask, not an ignore
			if (iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore)
				continue;

			// blast's don't tavel into or out of water
			if (bInWater && pEntity->pev->waterlevel == 0)
				continue;

			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			bool useLOS = false;
			float damageRatio = 1.0f;

			if ((bitsDamageType & DMG_EXPLOSION) && UTIL_IsGame("czero"))
			{
				useLOS = true;
				damageRatio = GetAmountOfPlayerVisible(vecSrc, pEntity);
			}

			float length = (vecSrc - pEntity->pev->origin).Length();

			if (useLOS)
			{
				if (!flRadius)
					flRadius = flDamage;

				if (!flDamage)
					flRadius = 0;

				flAdjustedDamage = (flRadius - length) * (flRadius - length) * 1.25 / (flRadius * flRadius) * (damageRatio * flDamage) * 1.5;
			}
			else
				flAdjustedDamage = flDamage - length * falloff;

			if (flAdjustedDamage < 0)
				flAdjustedDamage = 0;

			pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);
		}
	}
}

/* <5dcf9> ../cstrike/dlls/combat.cpp:1349 */
void RadiusDamage2(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	CBaseEntity *pEntity = NULL;
	TraceResult tr;
	float flAdjustedDamage, falloff;
	Vector vecSpot;

	if (flRadius)
		falloff = flDamage / flRadius;
	else
		falloff = 1;

	int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

	vecSrc.z += 1;

	if (!pevAttacker)
		pevAttacker = pevInflictor;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore)
				continue;

			if (bInWater && !pEntity->pev->waterlevel)
				continue;

			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			vecSpot = pEntity->BodyTarget(vecSrc);
			UTIL_TraceLine(vecSrc, vecSpot, dont_ignore_monsters, ENT(pevInflictor), &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
			{
				if (tr.fStartSolid)
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0;
				}

				flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;

				if (flAdjustedDamage < 0)
					flAdjustedDamage = 0;

				else if (flAdjustedDamage > 75)
					flAdjustedDamage = 75;

				if (tr.flFraction == 1.0f)
					pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);

				else
				{
					ClearMultiDamage();
					pEntity->TraceAttack(pevInflictor, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
					ApplyMultiDamage(pevInflictor, pevAttacker);
				}
			}
		}
	}
}

/* <5e0d2> ../cstrike/dlls/combat.cpp:1442 */
void CBaseMonster::RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType)
{
	if (flDamage > 80)
		::RadiusDamage(pev->origin, pevInflictor, pevAttacker, flDamage, flDamage * 3.5, iClassIgnore, bitsDamageType);
	else
		RadiusDamage2(pev->origin, pevInflictor, pevAttacker, flDamage, iClassIgnore, bitsDamageType);
}

/* <5e0d2> ../cstrike/dlls/combat.cpp:1448 */
NOXREF void CBaseMonster::RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType)
{
	if (flDamage > 80)
		::RadiusDamage(vecSrc, pevInflictor, pevAttacker, flDamage, flDamage * 3.5, iClassIgnore, bitsDamageType);
	else
		RadiusDamage2(vecSrc, pevInflictor, pevAttacker, flDamage, iClassIgnore, bitsDamageType);
}

/* <61949> ../cstrike/dlls/combat.cpp:1454 */
NOBODY CBaseEntity *CBaseMonster::CheckTraceHullAttack(float flDist, int iDamage, int iDmgType)
{
//	{
//		TraceResult tr;                                       //  1456
//		class Vector vecStart;                                //  1463
//		class Vector vecEnd;                                  //  1465
//		Vector(Vector *const this,
//			const Vector &v);  //  1463
//		operator*(const Vector *const this,
//				float fl);  //  1465
//		operator+(const Vector *const this,
//				const Vector &v);  //  1465
//		{ 
//			class CBaseEntity *pEntity;                  //  1471
//			Instance(edict_t *pent);  //  1471
//		} 
//	}
	return NULL;
}

/* <61ae6> ../cstrike/dlls/combat.cpp:1490 */
NOBODY BOOL CBaseMonster::__MAKE_VHOOK(FInViewCone)(CBaseEntity *pEntity)
{
//	{
//		class Vector2D vec2LOS;                               //  1492
//		float flDot;                                          //  1493
//		operator-(const Vector *const this,
//				const Vector &v);  //  1497
//		Normalize(const class Vector2D *const this);  //  1498
//		DotProduct(const class Vector2D  &a,
//				const class Vector2D  &b);  //  1500
//	}
	return FALSE;
}

/* <61be6> ../cstrike/dlls/combat.cpp:1517 */
NOBODY BOOL CBaseMonster::__MAKE_VHOOK(FInViewCone)(Vector *pOrigin)
{
//	{
//		class Vector2D vec2LOS;                               //  1519
//		float flDot;                                          //  1520
//		operator-(const Vector *const this,
//				const Vector &v);  //  1524
//		Normalize(const class Vector2D *const this);  //  1525
//		DotProduct(const class Vector2D  &a,
//				const class Vector2D  &b);  //  1527
//	}
	return FALSE;
}

/* <5ecb4> ../cstrike/dlls/combat.cpp:1543 */
NOBODY BOOL CBaseEntity::__MAKE_VHOOK(FVisible)(CBaseEntity *pEntity)
{
//	{
//		TraceResult tr;                                       //  1545
//		class Vector vecLookerOrigin;                         //  1546
//		class Vector vecTargetOrigin;                         //  1547
//	} 
//	FVisible(CBaseEntity *const this,
//		class CBaseEntity *pEntity);  //  1543
	return FALSE;
}

/* <5e9bb> ../cstrike/dlls/combat.cpp:1576 */
NOBODY BOOL CBaseEntity::__MAKE_VHOOK(FVisible)(Vector &vecOrigin)
{
//	{
//		TraceResult tr;                                       //  1578
//		class Vector vecLookerOrigin;                         //  1579
//	}
	return FALSE;
}

/* <5e872> ../cstrike/dlls/combat.cpp:1600 */
void CBaseEntity::__MAKE_VHOOK(TraceAttack)(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	Vector vecOrigin = ptr->vecEndPos - vecDir * 4;

	if (pev->takedamage == DAMAGE_YES)
	{
		AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);

		int blood = BloodColor();
		if (blood != DONT_BLEED)
		{
			// a little surface blood.
			SpawnBlood(vecOrigin, blood, flDamage);
			TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
		}
	}
}

/* <61ce5> ../cstrike/dlls/combat.cpp:1647 */
void CBaseMonster::__MAKE_VHOOK(TraceAttack)(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	Vector vecOrigin = ptr->vecEndPos - vecDir * 4;

	if (pev->takedamage == DAMAGE_YES)
	{
		m_LastHitGroup = ptr->iHitgroup;

		switch (ptr->iHitgroup)
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			flDamage *= 3;
			break;
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
			flDamage *= 1.5;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			flDamage *= 1.0;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			flDamage *= 0.75;
			break;
		case HITGROUP_SHIELD:
			flDamage = 0;
			break;
		default:
			break;
		}

		AddMultiDamage(pevAttacker, this, flDamage, bitsDamageType);

		int blood = BloodColor();
		if (blood != DONT_BLEED)
		{
			// a little surface blood.
			SpawnBlood(vecOrigin, blood, flDamage);
		}
	}
}

/* <61df9> ../cstrike/dlls/combat.cpp:1704 */
NOBODY void CBaseEntity::FireBullets(ULONG cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq, int iDamage, entvars_t *pevAttacker)
{
//	{
//		int tracerCount;                                      //  1706
//		int tracer;                                           //  1707
//		TraceResult tr;                                       //  1708
//		class Vector vecRight;                                //  1709
//		class Vector vecUp;                                   //  1710
//		bool m_bCreatedShotgunSpark;                          //  1711
//		Vector(Vector *const this,
//			const Vector &v);  //  1709
//		Vector(Vector *const this,
//			const Vector &v);  //  1710
//		{
//			ULONG iShot;                                  //  1722
//			{
//				int spark;                            //  1724
//				float x;                              //  1727
//				float y;                              //  1727
//				float z;                              //  1727
//				class Vector vecDir;                  //  1734
//				class Vector vecEnd;                  //  1737
//				operator*(float fl,
//						const Vector &v);  //  1736
//				operator*(float fl,
//						const Vector &v);  //  1736
//				operator+(const Vector *const this,
//						const Vector &v);  //  1736
//				operator+(const Vector *const this,
//						const Vector &v);  //  1736
//				operator*(const Vector *const this,
//						float fl);  //  1739
//				operator+(const Vector *const this,
//						const Vector &v);  //  1739
//				{
//					class Vector vecTracerSrc;    //  1745
//					operator*(const Vector *const this,
//							float fl);  //  1749
//					operator*(const Vector *const this,
//							float fl);  //  1749
//					operator+(const Vector *const this,
//							const Vector &v);  //  1749
//					operator+(const Vector *const this,
//							const Vector &v);  //  1749
//					operator+(const Vector *const this,
//							const Vector &v);  //  1749
//					MESSAGE_BEGIN(int msg_dest,
//							int msg_type,
//							const float *pOrigin,
//							edict_t *ed);  //  1765
//				} 
//				{
//					class CBaseEntity *pEntity;  //  1780
//					Instance(edict_t *pent);  //  1780
//					Vector(Vector *const this,
//						const Vector &v);  //  1784
//					Vector(Vector *const this,
//						const Vector &v);  //  1786
//					Vector(Vector *const this,
//						const Vector &v);  //  1786
//					{
//						float flDamage;       //  1802
//						Vector(Vector *const this,
//							const Vector &v);  //  1793
//						Vector(Vector *const this,
//							const Vector &v);  //  1830
//						Vector(Vector *const this,
//							const Vector &v);  //  1822
//						Vector(Vector *const this,
//							const Vector &v);  //  1824
//						Vector(Vector *const this,
//							const Vector &v);  //  1824
//						Vector(Vector *const this,
//							const Vector &v);  //  1814
//						Vector(Vector *const this,
//							const Vector &v);  //  1816
//						Vector(Vector *const this,
//							const Vector &v);  //  1816
//						Vector(Vector *const this,
//							const Vector &v);  //  1806
//						Vector(Vector *const this,
//							const Vector &v);  //  1810
//						Vector(Vector *const this,
//							const Vector &v);  //  1797
//						Vector(Vector *const this,
//							const Vector &v);  //  1839
//						Vector(Vector *const this,
//							const Vector &v);  //  1840
//						Vector(Vector *const this,
//							const Vector &v);  //  1840
//						FNullEnt(const edict_t *pent);  //  1842
//						VARS(edict_t *pent);  //  1842
//						Vector(Vector *const this,
//							const Vector &v);  //  1833
//						Vector(Vector *const this,
//							const Vector &v);  //  1833
//					} 
//				} 
//				Vector(Vector *const this,
//					const Vector &v);  //  1851
//				Vector(Vector *const this,
//					const Vector &v);  //  1851
//			} 
//		} 
//	} 
}

/* <62693> ../cstrike/dlls/combat.cpp:1856 */
NOXREF char *vstr(float *v)
{
	static int idx = 0;
	static char string[ 16 ][ 1024 ];

	idx = (idx + 1) % 0xF;
	Q_sprintf(string[ idx ], "%.4f %.4f %.4f", v[0], v[1], v[2]);

	return string[ idx ];
}

Vector (*pFireBullets3)(Vector, Vector, float, float, int, int, int, float, entvars_t *, bool, int);

/* <62709> ../cstrike/dlls/combat.cpp:1869 */
Vector __declspec(naked) CBaseEntity::FireBullets3(Vector vecSrc, Vector vecDirShooting, float vecSpread, float flDistance, int iPenetration, int iBulletType, int iDamage, float flRangeModifier, entvars_t *pevAttacker, bool bPistol, int shared_rand)
{
	UNTESTED
	//TODO: crash a test to czero

	__asm
	{
		jmp pFireBullets3
	}
/*	int iOriginalPenetration = iPenetration;
	int iPenetrationPower;

	//int iSparksAmount;
	int iCurrentDamage = iDamage;

	float flPenetrationDistance;
	float flCurrentDistance;

	TraceResult tr;
	TraceResult tr2;

	CBaseEntity *pEntity;
	Vector vecDir;
	Vector vecEnd;

	bool bHitMetal = false;

	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;

	switch (iBulletType)
	{
		case BULLET_PLAYER_9MM:
		{
			iPenetrationPower = 21;
			flPenetrationDistance = 800;
			break;
		}
		case BULLET_PLAYER_45ACP:
		{
			iPenetrationPower = 15;
			flPenetrationDistance = 500;
			break;
		}
		case BULLET_PLAYER_50AE:
		{
			iPenetrationPower = 30;
			flPenetrationDistance = 1000;
			break;
		}
		case BULLET_PLAYER_762MM:
		{
			iPenetrationPower = 39;
			flPenetrationDistance = 5000.0f;
			break;
		}
		case BULLET_PLAYER_556MM:
		{
			iPenetrationPower = 35;
			flPenetrationDistance = 4000;
			break;
		}
		case BULLET_PLAYER_338MAG:
		{
			iPenetrationPower = 45;
			flPenetrationDistance = 8000;
			break;
		}
		case BULLET_PLAYER_57MM:
		{
			iPenetrationPower = 30;
			flPenetrationDistance = 2000;
			break;
		}
		case BULLET_PLAYER_357SIG:
		{
			iPenetrationPower = 25;
			flPenetrationDistance = 800;
			break;
		}
		default:
		{
			iPenetrationPower = 0;
			flPenetrationDistance = 0;
			break;
		}
	}

	if (!pevAttacker)
		pevAttacker = pev; // the default attacker is ourselves

	gMultiDamage.type = (DMG_BULLET | DMG_NEVERGIB);

	float x, y, z;

	if (IsPlayer())
	{
		// Use player's random seed.
		// get circular gaussian spread
		x = UTIL_SharedRandomFloat(shared_rand, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
		y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);
	}
	else
	{
		do
		{
			x = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
			y = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
			z = x * x + y * y;
		}
		while (z > 1);
	}

	vecDir = vecDirShooting + x * vecSpread * vecRight + y * vecSpread * vecUp;
	vecEnd = vecSrc + vecDir * flDistance;

	float flDamageModifier = 0.5;

	while (iPenetration != 0)
	{
		ClearMultiDamage();
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		if (tr.flFraction != 1)
			TheBots->OnEvent(EVENT_BULLET_IMPACT, this, NULL);

		switch (UTIL_TextureHit(&tr, vecSrc, vecEnd))
		{
			case CHAR_TEX_METAL:
			{
				bHitMetal = true;
				iPenetrationPower *= 0.15;
				flDamageModifier = 0.2;
				break;
			}
			case CHAR_TEX_CONCRETE:
			{
				iPenetrationPower *= 0.25;
				break;
			}
			case CHAR_TEX_GRATE:
			{
				bHitMetal = true;
				iPenetrationPower *= 0.5;
				flDamageModifier = 0.4;
				break;
			}
			case CHAR_TEX_VENT:
			{
				bHitMetal = true;
				iPenetrationPower *= 0.5;
				flDamageModifier = 0.45;
				break;
			}
			case CHAR_TEX_TILE:
			{
				iPenetrationPower *= 0.65;
				flDamageModifier = 0.3;
				break;
			}
			case CHAR_TEX_COMPUTER:
			{
				bHitMetal = true;
				iPenetrationPower *= 0.4;
				flDamageModifier = 0.45;
				break;
			}
			case CHAR_TEX_WOOD:
			{
				flDamageModifier = 0.6;
				break;
			}
			default:
				break;
		}
		if (tr.flFraction != 1.0)
		{
			pEntity = CBaseEntity::Instance(tr.pHit);
			iPenetration--;

			flCurrentDistance = tr.flFraction * flDistance;
			iCurrentDamage *= pow(flRangeModifier, flCurrentDistance / 500);

			if (flCurrentDistance > flPenetrationDistance)
				iPenetration = 0;

			if (tr.iHitgroup == HITGROUP_SHIELD)
			{
				if (RANDOM_LONG(0, 1))
					EMIT_SOUND(pEntity->edict(), CHAN_VOICE, "weapons/ric_metal-1.wav", VOL_NORM, ATTN_NORM);
				else
					EMIT_SOUND(pEntity->edict(), CHAN_VOICE, "weapons/ric_metal-2.wav", VOL_NORM, ATTN_NORM);

				UTIL_Sparks(tr.vecEndPos);

				pEntity->pev->punchangle.x = iCurrentDamage * RANDOM_FLOAT(-0.15, 0.15);
				pEntity->pev->punchangle.z = iCurrentDamage * RANDOM_FLOAT(-0.15, 0.15);

				if (pEntity->pev->punchangle.x < 4)
					pEntity->pev->punchangle.x = -4;//TODO:?? 4 ?

				if (pEntity->pev->punchangle.z < -5)
					pEntity->pev->punchangle.z = -5;

				else if (pEntity->pev->punchangle.z > 5)
					pEntity->pev->punchangle.z = 5;

				break;
			}

			float flDistanceModifier;
			//if (VARS(tr.pHit)->solid != SOLID_BSP || !iPenetration)
			if (pEntity->pev->solid != SOLID_BSP || !iPenetration)
			{
				iPenetrationPower = 42;
				flDamageModifier = 0.75;
				flDistanceModifier = 0.75;
			}
			else
				flDistanceModifier = 0.5;

			DecalGunshot(&tr, iBulletType, (!bPistol && RANDOM_LONG(0, 3)), pev, bHitMetal);
				
			vecSrc = tr.vecEndPos + (vecDir * iPenetrationPower);
			flDistance = (flDistance - flCurrentDistance) * flDistanceModifier;
			vecEnd = vecSrc + (vecDir * flDistance);

			pEntity->TraceAttack(pevAttacker, iCurrentDamage, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB);
			iCurrentDamage *= flDamageModifier;
		}
		else
			iPenetration = 0;

		ApplyMultiDamage(pev, pevAttacker);
	}

	return Vector(x * vecSpread, y * vecSpread, 0);*/
}

/* <5eb17> ../cstrike/dlls/combat.cpp:2075 */
void CBaseEntity::__MAKE_VHOOK(TraceBleed)(float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (BloodColor() == DONT_BLEED)
		return;

	if (!flDamage)
		return;

	if (!(bitsDamageType & (DMG_CRUSH | DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_CLUB | DMG_MORTAR)))
		return;

	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir;
	float flNoise;
	int cCount;
	int i;

	if (flDamage < 10.0f)
	{
		flNoise = 0.1f;
		cCount = 1;
	}
	else if (flDamage < 25.0f)
	{
		flNoise = 0.2f;
		cCount = 2;
	}
	else
	{
		flNoise = 0.3f;
		cCount = 4;
	}

	for (i = 0; i < cCount; i++)
	{
		// trace in the opposite direction the shot came from (the direction the shot is going)
		vecTraceDir = vecDir * -1.0f;

		vecTraceDir.x += RANDOM_FLOAT(-flNoise, flNoise);
		vecTraceDir.y += RANDOM_FLOAT(-flNoise, flNoise);
		vecTraceDir.z += RANDOM_FLOAT(-flNoise, flNoise);

		UTIL_TraceLine(ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * -172.0f, ignore_monsters, ENT(pev), &Bloodtr);
		if (Bloodtr.flFraction != 1.0f)
		{
			if (!RANDOM_LONG(0, 2))
				UTIL_BloodDecalTrace(&Bloodtr, BloodColor());
		}
	}
}

/* <62e0e> ../cstrike/dlls/combat.cpp:2145 */
NOXREF void CBaseMonster::MakeDamageBloodDecal(int cCount, float flNoise, TraceResult *ptr, Vector &vecDir)
{
	// make blood decal on the wall! 
	TraceResult Bloodtr;
	Vector vecTraceDir; 
	int i;

	if (!IsAlive())
	{
		// dealing with a dead monster.
		if (pev->max_health <= 0)
		{
			// no blood decal for a monster that has already decalled its limit.
			return;
		}
		else
			pev->max_health--;
	}

	for (i = 0 ; i < cCount ; i++)
	{
		vecTraceDir = vecDir;

		vecTraceDir.x += RANDOM_FLOAT(-flNoise, flNoise);
		vecTraceDir.y += RANDOM_FLOAT(-flNoise, flNoise);
		vecTraceDir.z += RANDOM_FLOAT(-flNoise, flNoise);

		UTIL_TraceLine(ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * 172, ignore_monsters, ENT(pev), &Bloodtr);

		if (Bloodtr.flFraction != 1.0)
		{
			UTIL_BloodDecalTrace(&Bloodtr, BloodColor());
		}
	}
}

/* <62f3e> ../cstrike/dlls/combat.cpp:2197 */
void CBaseMonster::BloodSplat(const Vector &vecSrc, const Vector &vecDir, int HitLocation, int iVelocity)
{
	if (HitLocation != HITGROUP_HEAD)
		return;

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_BLOODSTREAM);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecDir.x);
		WRITE_COORD(vecDir.y);
		WRITE_COORD(vecDir.z);
		WRITE_BYTE(223);
		WRITE_BYTE(iVelocity + RANDOM_LONG(0, 100));
	MESSAGE_END();
}

#ifdef HOOK_GAMEDLL

void CBaseMonster::KeyValue(KeyValueData *pkvd)
{
	KeyValue_(pkvd);
}

void CBaseMonster::TraceAttack(entvars_t *pevAttacker,float flDamage,Vector vecDir,TraceResult *ptr,int bitsDamageType)
{
	TraceAttack_(pevAttacker, flDamage, vecDir, ptr, bitsDamageType);
}

int CBaseMonster::TakeDamage(entvars_t *pevInflictor,entvars_t *pevAttacker,float flDamage,int bitsDamageType)
{
	return TakeDamage_(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

int CBaseMonster::TakeHealth(float flHealth,int bitsDamageType)
{
	return TakeHealth_(flHealth, bitsDamageType);
}

void CBaseMonster::Killed(entvars_t *pevAttacker,int iGib)
{
	Killed_(pevAttacker, iGib);
}

float CBaseMonster::ChangeYaw(int speed)
{
	return ChangeYaw_(speed);
}

BOOL CBaseMonster::HasHumanGibs(void)
{
	return HasHumanGibs_();
}

BOOL CBaseMonster::HasAlienGibs(void)
{
	return HasHumanGibs_();
}

void CBaseMonster::FadeMonster(void)
{
	FadeMonster_();
}

void CBaseMonster::GibMonster(void)
{
	GibMonster_();
}

Activity CBaseMonster::GetDeathActivity(void)
{
	return GetDeathActivity_();
}

void CBaseMonster::BecomeDead(void)
{
	BecomeDead_();
}

BOOL CBaseMonster::ShouldFadeOnDeath(void)
{
	return ShouldFadeOnDeath_();
}

int CBaseMonster::IRelationship(CBaseEntity *pTarget)
{
	return IRelationship_(pTarget);
}

void CBaseMonster::MonsterInitDead(void)
{
	MonsterInitDead_();
}

void CBaseMonster::Look(int iDistance)
{
	Look_(iDistance);
}

CBaseEntity *CBaseMonster::BestVisibleEnemy(void)
{
	return BestVisibleEnemy_();
}

BOOL CBaseMonster::FInViewCone(CBaseEntity *pEntity)
{
	return FInViewCone_(pEntity);
}

BOOL CBaseMonster::FInViewCone(Vector *pOrigin)
{
	return FInViewCone_(pOrigin);
}

#endif // HOOK_GAMEDLL