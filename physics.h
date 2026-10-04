#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector.h"
#include "export.h"

EXPORT void Physics_SetGravity(CVector *);
void patch_physics(void);

#ifdef _WIN32
CVector* __fastcall SpideyPhysicsFriction60(
		CVector* velocity,
		void*,
		const CFriction& friction);
#endif

#endif
